/**
 * memory.h - Header of module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * @author Pavel Kryukov <pavel.kryukov@phystech.edu>
 * Copyright 2012-2019 uArchSim iLab project
 */

#ifndef FUNC_MEMORY_FUNC_MEMORY_H
#define FUNC_MEMORY_FUNC_MEMORY_H

#include <infra/endian.h>
#include <infra/exception.h>
#include <infra/macro.h>
#include <infra/types.h>

#include <array>
#include <cassert>
#include <memory>
#include <string>
#include <vector>

struct FuncMemoryBadMapping final : Exception
{
    explicit FuncMemoryBadMapping( const std::string& msg);
    ~FuncMemoryBadMapping() final;
    FuncMemoryBadMapping( const FuncMemoryBadMapping&) = delete;
    FuncMemoryBadMapping( FuncMemoryBadMapping&&) = delete;
    FuncMemoryBadMapping& operator=( const FuncMemoryBadMapping&) = delete;
    FuncMemoryBadMapping& operator=( FuncMemoryBadMapping&&) = delete;
};

struct FuncMemoryOutOfRange final : Exception
{
    explicit FuncMemoryOutOfRange( Addr addr, Addr mask);
    ~FuncMemoryOutOfRange() final;
    FuncMemoryOutOfRange( const FuncMemoryOutOfRange&) = delete;
    FuncMemoryOutOfRange( FuncMemoryOutOfRange&&) = delete;
    FuncMemoryOutOfRange& operator=( const FuncMemoryOutOfRange&) = delete;
    FuncMemoryOutOfRange& operator=( FuncMemoryOutOfRange&&) = delete;
private:
    static std::string generate_string( Addr addr, Addr mask);
};

class WriteableMemory;

class ReadableMemory
{
public:
    ReadableMemory();
    virtual ~ReadableMemory();
    ReadableMemory( const ReadableMemory&) = delete;
    ReadableMemory( ReadableMemory&&) = delete;
    ReadableMemory& operator=( const ReadableMemory&) = delete;
    ReadableMemory& operator=( ReadableMemory&&) = delete;

    static std::shared_ptr<ReadableMemory> create_zero_memory();

    virtual size_t memcpy_guest_to_host( std::byte* dst, Addr src, size_t size) const noexcept = 0;
    virtual void duplicate_to( std::shared_ptr<WriteableMemory> target) const = 0;
    virtual std::string dump() const = 0;
    virtual size_t strlen( Addr addr) const = 0;
    std::string read_string( Addr addr) const;
    std::string read_string_limited( Addr addr, size_t size) const;

    template<typename T, std::endian endian> T read( Addr addr) const noexcept;
    template<typename T, std::endian endian> T read( Addr addr, T mask) const noexcept { return read<T, endian>( addr) & mask; }
protected:
    template<typename Instr> void load( Instr* instr) const;
private:
    std::string read_string_by_size( Addr addr, size_t size) const;
};

template<typename T, std::endian endian>
T ReadableMemory::read( Addr addr) const noexcept
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init) Initialized by memcpy
    std::array<std::byte, bytewidth<T>> bytes;
    memcpy_guest_to_host( bytes.data(), addr, bytes.size());
    return pack_array<T, endian>( bytes);
}

template<typename Instr>
void ReadableMemory::load( Instr* instr) const
{
    using DstType = decltype( std::declval<Instr>().get_v_dst( 0));
    auto mask = bitmask<DstType>( instr->get_mem_size() * CHAR_BIT);
    auto value = instr->get_endian() == std::endian::little
        ? read<DstType, std::endian::little>( instr->get_mem_addr(), mask)
        : read<DstType, std::endian::big>( instr->get_mem_addr(), mask);
    instr->load( value);
}

class WriteableMemory
{
public:
    WriteableMemory();
    virtual ~WriteableMemory();
    WriteableMemory( const WriteableMemory&) = delete;
    WriteableMemory( WriteableMemory&&) = delete;
    WriteableMemory& operator=( const WriteableMemory&) = delete;
    WriteableMemory& operator=( WriteableMemory&&) = delete;

    virtual size_t memcpy_host_to_guest( Addr dst, const std::byte* src, size_t size) = 0;

    size_t memcpy_host_to_guest_noexcept( Addr dst, const std::byte* src, size_t size) noexcept try
    {
        return memcpy_host_to_guest( dst, src, size);
    }
    catch (...)
    {
        return 0;
    }

    template<typename T, std::endian endian>
    void write( T value, Addr addr)
    {
        const auto& bytes = unpack_array<T, endian>( value);
        memcpy_host_to_guest( addr, bytes.data(), bytes.size());
    }

    template<typename T, std::endian endian>
    void write_integer( T value, Addr addr, size_t size);

    void write_string( const std::string& value, Addr addr);
    void write_string_limited( const std::string& value, Addr addr, size_t size);

    void memset( Addr addr, std::byte value, size_t size);
private:
    void write_string_by_size( const std::string& value, Addr addr, size_t size);
};

// NOLINTNEXTLINE(fuchsia-multiple-inheritance) Both are pure virtual actually
class ReadableAndWriteableMemory : public ReadableMemory, public WriteableMemory
{
public:
    ReadableAndWriteableMemory();
    ~ReadableAndWriteableMemory() override;
    ReadableAndWriteableMemory( const ReadableAndWriteableMemory&) = delete;
    ReadableAndWriteableMemory( ReadableAndWriteableMemory&&) = delete;
    ReadableAndWriteableMemory& operator=( const ReadableAndWriteableMemory&) = delete;
    ReadableAndWriteableMemory& operator=( ReadableAndWriteableMemory&&) = delete;
};

class FuncMemory : public ReadableAndWriteableMemory
{
public:
    FuncMemory();
    ~FuncMemory() override;
    FuncMemory( const FuncMemory&) = delete;
    FuncMemory( FuncMemory&&) = delete;
    FuncMemory& operator=( const FuncMemory&) = delete;
    FuncMemory& operator=( FuncMemory&&) = delete;

    static std::shared_ptr<FuncMemory> create_hierarchied_memory( uint32 addr_bits, uint32 page_bits, uint32 offset_bits);
    static std::shared_ptr<FuncMemory> create_default_hierarchied_memory()
    {
        return create_hierarchied_memory( 36, 10, 12);
    }

    static std::shared_ptr<FuncMemory> create_plain_memory( uint32 addr_bits);
    static std::shared_ptr<FuncMemory> create_4M_plain_memory()
    {
        return create_plain_memory( 22);
    }

    template<typename T, std::endian endian> void masked_write( T value, Addr addr, T mask)
    {
        T combined_value = ( value & mask) | ( this->read<T, endian>( addr) & ~mask);
        write<T, endian>( combined_value, addr);
    }

    template<typename Instr> void load_store( Instr* instr);
private:
    template<typename Instr, std::endian endian> void store( const Instr& instr);
    template<typename Instr, std::endian endian> void masked_store( const Instr& instr);
};

template<typename Instr, std::endian endian>
void FuncMemory::store( const Instr& instr)
{
    using SrcType = decltype( std::declval<Instr>().get_v_src( 1));
    if ( instr.get_mem_addr() == 0)
        throw Exception("Store data to zero is a cricital error");

    auto full_mask = bitmask<SrcType>( instr.get_mem_size() * CHAR_BIT);
    if ( ( instr.get_mask() & full_mask) == full_mask)
        write_integer<SrcType, endian>( instr.get_v_src( 1), instr.get_mem_addr(), instr.get_mem_size());
    else
        masked_write<SrcType, endian>( instr.get_v_src( 1), instr.get_mem_addr(), instr.get_mask());
}

template<typename Instr>
void FuncMemory::load_store( Instr* instr)
{
    if ( instr->is_load()) {
        load( instr);
    }
    else if ( instr->is_store()) {
        if ( instr->get_endian() == std::endian::little)
            store<Instr, std::endian::little>( *instr);
        else
            store<Instr, std::endian::big>( *instr);
    }
}

class FuncMemoryReplicant : public FuncMemory
{
public:
    explicit FuncMemoryReplicant( std::shared_ptr<FuncMemory> memory) : primary( std::move( memory)) { }
    void add_replica( const std::shared_ptr<FuncMemory>& memory)
    {
        replicas.emplace_back( memory);
        primary->duplicate_to( memory);
    }

    size_t memcpy_guest_to_host( std::byte* dst, Addr src, size_t size) const noexcept final
    {
        return primary->memcpy_guest_to_host( dst, src, size);
    }

    size_t memcpy_host_to_guest( Addr dst, const std::byte* src, size_t size) final
    {
        auto result = primary->memcpy_host_to_guest( dst, src, size);
        for ( auto& e : replicas)
            e->memcpy_host_to_guest( dst, src, size);
        return result;
    }

    void duplicate_to( std::shared_ptr<WriteableMemory> target) const final
    {
        primary->duplicate_to( target);
    }

    std::string dump() const final
    {
        return primary->dump();
    }

    size_t strlen( Addr addr) const final
    {
        return primary->strlen( addr);
    }
private:
    std::shared_ptr<FuncMemory> primary;
    std::vector<std::shared_ptr<FuncMemory>> replicas;
};

#endif // #ifndef FUNC_MEMORY_FUNC_MEMORY_H
