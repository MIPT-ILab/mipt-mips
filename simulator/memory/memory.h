/**
 * memory.h - Header of module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * @author Pavel Kryukov <pavel.kryukov@phystech.edu>
 * Copyright 2012-2018 uArchSim iLab project
 */

#ifndef FUNC_MEMORY__FUNC_MEMORY_H
#define FUNC_MEMORY__FUNC_MEMORY_H

// uArchSim modules
#include <infra/endian.h>
#include <infra/exception.h>
#include <infra/macro.h>
#include <infra/types.h>

// Generic C++
#include <array>
#include <memory>
#include <string>

struct FuncMemoryBadMapping final : Exception
{
    explicit FuncMemoryBadMapping( const std::string& msg)
        : Exception( "Invalid FuncMemory mapping", msg)
    { }
};

struct FuncMemoryOutOfRange final : Exception
{
    explicit FuncMemoryOutOfRange( Addr addr, Addr mask)
        : Exception( "Out of memory range",
            std::string( "address: ") + std::to_string(addr) + "; max address: " + std::to_string(mask))
    { }
};

class DestructableMemory
{
public:
    DestructableMemory() = default;
    virtual ~DestructableMemory() = default;
    DestructableMemory( const DestructableMemory&) = default;
    DestructableMemory( DestructableMemory&&) = default;
    DestructableMemory& operator=( const DestructableMemory&) = default;
    DestructableMemory& operator=( DestructableMemory&&) = default;
};

class WriteableMemory;

class ReadableMemory : public DestructableMemory
{
public:
    virtual size_t memcpy_guest_to_host( Byte* dst, Addr src, size_t size) const noexcept = 0;
    virtual void duplicate_to( std::shared_ptr<WriteableMemory> target) const = 0;
    virtual std::string dump() const = 0;
    virtual size_t strlen( Addr addr) const = 0;
    std::string read_string( Addr addr) const;
    std::string read_string_limited( Addr addr, size_t size) const;

    template<typename T, Endian endian> T read( Addr addr) const noexcept;
    template<typename T, Endian endian> T read( Addr addr, T mask) const noexcept { return read<T, endian>( addr) & mask; }
protected:
    template<typename Instr> void load( Instr* instr) const;
private:
    std::string read_string_by_size( Addr addr, size_t size) const;
};

template<typename T, Endian endian>
T ReadableMemory::read( Addr addr) const noexcept
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init) Initialized by memcpy
    std::array<Byte, bytewidth<T>> bytes;
    memcpy_guest_to_host( bytes.data(), addr, bytes.size());
    return pack_array<T, endian>( bytes);
}

template<typename Instr>
void ReadableMemory::load( Instr* instr) const
{
    static const constexpr Endian endian = Instr::endian;
    using DstType = decltype( std::declval<Instr>().get_v_dst());
    auto mask = bitmask<DstType>( instr->get_mem_size() * CHAR_BIT);
    auto value = read<DstType, endian>( instr->get_mem_addr(), mask);
    instr->set_v_dst( value);
}

class ZeroMemory : public ReadableMemory
{
public:
    size_t memcpy_guest_to_host( Byte* dst, Addr /* src */, size_t size) const noexcept final;
    void duplicate_to( std::shared_ptr<WriteableMemory> /* target */) const final { }
    std::string dump() const final { return std::string( "empty memory\n"); }
    size_t strlen( Addr /* addr */) const final { return 0; }
};

class WriteableMemory : public DestructableMemory
{
public:
    virtual size_t memcpy_host_to_guest( Addr dst, const Byte* src, size_t size) = 0;

    size_t memcpy_host_to_guest_noexcept( Addr dst, const Byte* src, size_t size) noexcept try
    {
        return memcpy_host_to_guest( dst, src, size);
    }
    catch (...)
    {
        return 0;
    }

    template<typename T, Endian endian>
    void write( T value, Addr addr)
    {
        const auto& bytes = unpack_array<T, endian>( value);
        memcpy_host_to_guest( addr, bytes.data(), bytes.size());
    }

    void write_string( const std::string& value, Addr addr);
    void write_string_limited( const std::string& value, Addr addr, size_t size);
private:
    void write_string_by_size( const std::string& value, Addr addr, size_t size);
};

// NOLINTNEXTLINE(fuchsia-multiple-inheritance) Both are pure virtual actually
class FuncMemory : public ReadableMemory, public WriteableMemory
{
public:
    static std::shared_ptr<FuncMemory>
        create_hierarchied_memory( uint32 addr_bits = 32,
				 uint32 page_bits = 10,
				 uint32 offset_bits = 12);

    static std::shared_ptr<FuncMemory>
        create_plain_memory( uint32 addr_bits = 20);

    template<typename T, Endian endian> void masked_write( T value, Addr addr, T mask)
    {
        T combined_value = ( value & mask) | ( this->read<T, endian>( addr) & ~mask);
        write<T, endian>( combined_value, addr);
    }

    template<typename Instr> void load_store( Instr* instr);
private:
    template<typename Instr> void store( const Instr& instr);
};

template<typename Instr>
void FuncMemory::store( const Instr& instr)
{
    static const constexpr Endian endian = Instr::endian;
    using DstType = decltype( std::declval<Instr>().get_v_dst());
    if ( instr.get_mem_addr() == 0)
        throw Exception("Store data to zero is an unhandled trap");

    if ( ~instr.get_mask() == 0)
        write<DstType, endian>( instr.get_v_src2(), instr.get_mem_addr());
    else
        masked_write<DstType, endian>( instr.get_v_src2(), instr.get_mem_addr(), instr.get_mask());
}

template<typename Instr>
void FuncMemory::load_store(Instr* instr)
{
    if ( instr->is_load())
        load( instr);
    else if ( instr->is_store())
        store( *instr);
}

#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H
