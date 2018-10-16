/**
 * memory.h - Header of module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
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

class FuncMemory
{
    public:
        static std::unique_ptr<FuncMemory>
            create_hierarchied_memory( uint32 addr_bits = 32,
                                         uint32 page_bits = 10,
                                         uint32 offset_bits = 12);

        Addr startPC() const { return startPC_addr; }
        void set_startPC(Addr value) { startPC_addr = value; }

        virtual size_t memcpy_host_to_guest( Addr dst, const Byte* src, size_t size) = 0;
        virtual size_t memcpy_guest_to_host( Byte* dst, Addr src, size_t size) const = 0;
        virtual void duplicate_to( FuncMemory* target) const = 0;
        virtual std::string dump() const = 0;

        size_t memcpy_host_to_guest_noexcept( Addr dst, const Byte* src, size_t size) noexcept;
        size_t memcpy_guest_to_host_noexcept( Byte* dst, Addr src, size_t size) const noexcept;

        template<typename T, Endian endian> T read( Addr addr) const;
        template<typename T, Endian endian> T read( Addr addr, T mask) const { return read<T, endian>( addr) & mask; }

        template<typename T, Endian endian> void write( T value, Addr addr);
        template<typename T, Endian endian> void write( T value, Addr addr, T mask)
        {
            T combined_value = ( value & mask) | ( read<T, endian>( addr) & ~mask);
            write<T, endian>( combined_value, addr);
        }

        template<typename Instr> void load_store( Instr* instr);

        virtual ~FuncMemory() = default;
        FuncMemory( const FuncMemory&) = default;
        FuncMemory( FuncMemory&&) = default;
        FuncMemory& operator=( const FuncMemory&) = default;
        FuncMemory& operator=( FuncMemory&&) = default;
    private:
        Addr startPC_addr = 0;
        template<typename Instr> void load( Instr* instr) const;
        template<typename Instr> void store( const Instr& instr);
};

inline size_t FuncMemory::memcpy_host_to_guest_noexcept( Addr dst, const Byte* src, size_t size) noexcept try
{
    return memcpy_host_to_guest( dst, src, size);
}
catch (...)
{
    return 0;
}

inline size_t FuncMemory::memcpy_guest_to_host_noexcept( Byte *dst, Addr src, size_t size) const noexcept try
{
    return memcpy_guest_to_host( dst, src, size);
}
catch (...)
{
    return 0;
}

template<typename T, Endian endian>
T FuncMemory::read( Addr addr) const
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init) Initialized by memcpy
    std::array<Byte, bytewidth<T>> bytes;
    memcpy_guest_to_host( bytes.data(), addr, bytes.size());
    return pack_array<T, endian>( bytes);
}

template<typename T, Endian endian>
void FuncMemory::write( T value, Addr addr)
{
    const auto& bytes = unpack_array<T, endian>( value);
    memcpy_host_to_guest( addr, bytes.data(), bytes.size());
}

template<typename Instr>
void FuncMemory::load( Instr* instr) const
{
    static const constexpr Endian endian = Instr::endian;
    using DstType = decltype( std::declval<Instr>().get_v_dst());
    auto mask = bitmask<DstType>( instr->get_mem_size() * CHAR_BIT);
    auto value = read<DstType, endian>( instr->get_mem_addr(), mask);
    instr->set_v_dst( value);
}

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
        write<DstType, endian>( instr.get_v_src2(), instr.get_mem_addr(), instr.get_mask());
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
