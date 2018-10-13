/**
 * plain_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Pavel Kryukov <kryukov@frtk.ru>
 * Copyright 2018 MIPT-MIPS
 */

#include "memory.h"

#include <cassert>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <vector>

class PlainMemory : public FuncMemory
{
    public:
        explicit PlainMemory ( uint32 addr_bits);

        std::string dump() const final;
        size_t memcpy_host_to_guest( Addr dst, const Byte* src, size_t size) final;
        size_t memcpy_guest_to_host( Byte* dst, Addr src, size_t size) const final;
        void duplicate_to( FuncMemory* target) const final;
    private:
        std::size_t arena_size;
        std::unique_ptr<Byte[]> arena;
        Byte* guest_to_host(Addr addr);
        const Byte* guest_to_host(Addr addr) const;
};

std::unique_ptr<FuncMemory>
FuncMemory::create_plain_memory( uint32 addr_bits)
{
    return std::make_unique<PlainMemory>( addr_bits);
}

PlainMemory::PlainMemory( uint32 addr_bits) try
    : arena_size( 1ull << addr_bits)
    , arena( std::make_unique<Byte[]>( arena_size))
{
}
catch (const std::bad_alloc&)
{
    throw FuncMemoryBadMapping("Too many address guest address bits");
}

void PlainMemory::duplicate_to( FuncMemory* target) const
{
    target->memcpy_host_to_guest( 0, arena.get(), arena_size);
}

size_t PlainMemory::memcpy_host_to_guest( Addr dst, const Byte* src, size_t size)
{
    assert( dst <= arena_size);
    assert( dst + size <= arena_size);
    std::memcpy( guest_to_host( dst), src, size);
    return size;
}

size_t PlainMemory::memcpy_guest_to_host( Byte* dst, Addr src, size_t size) const
{
    assert( src <= arena_size);
    assert( src + size <= arena_size);
    std::memcpy( dst, guest_to_host( src), size);
    return size;
}

Byte* PlainMemory::guest_to_host(Addr addr)
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic) low level access
    return arena.get() + addr;
}

const Byte* PlainMemory::guest_to_host(Addr addr) const
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic) low level access
    return arena.get() + addr;
}

std::string PlainMemory::dump() const
{
    std::ostringstream oss;
    oss << std::setfill( '0') << std::hex;

    for ( Addr i = 0; i < arena_size; ++i)
        if ( uint32( arena[i]) != 0)
            oss << "addr 0x" << i
                << ": data 0x" << uint32( arena[i]) << std::endl;

    return oss.str();
}
