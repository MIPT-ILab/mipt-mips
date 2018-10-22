/**
 * plain_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Pavel Kryukov <kryukov@frtk.ru>
 * Copyright 2018 MIPT-MIPS
 */

#include "memory.h"

#include <iomanip>
#include <sstream>
#include <vector>

class PlainMemory : public FuncMemory
{
    public:
        explicit PlainMemory ( uint32 addr_bits);

        std::string dump() const final;
        size_t memcpy_host_to_guest( Addr dst, const Byte* src, size_t size) final;
        size_t memcpy_guest_to_host( Byte* dst, Addr src, size_t size) const noexcept final;
        void duplicate_to( FuncMemory* target) const final;
    private:
        std::vector<Byte> arena;
};

std::unique_ptr<FuncMemory>
FuncMemory::create_plain_memory( uint32 addr_bits)
{
    return std::make_unique<PlainMemory>( addr_bits);
}

PlainMemory::PlainMemory( uint32 addr_bits) try
    : arena( 1ull << addr_bits)
{
}
catch (const std::bad_alloc&)
{
    throw FuncMemoryBadMapping("Too many address guest address bits");
}

void PlainMemory::duplicate_to( FuncMemory* target) const
{
    target->memcpy_host_to_guest( 0, arena.data(), arena.size());
}

size_t PlainMemory::memcpy_host_to_guest( Addr dst, const Byte* src, size_t size)
{
    if ( size > arena.size())
        throw FuncMemoryOutOfRange( dst + size, arena.size());

    if ( dst > arena.size())
        throw FuncMemoryOutOfRange( dst, arena.size());

    if ( dst > arena.size() - size)
        throw FuncMemoryOutOfRange( dst + size, arena.size());

    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic) low-level access
    std::copy( src, src + size, arena.begin() + dst);
    return size;
}

size_t PlainMemory::memcpy_guest_to_host( Byte* dst, Addr src, size_t size) const noexcept
{
    size_t valid_size = std::min<size_t>( size, arena.size() - src);
    std::copy( arena.begin() + src, arena.begin() + src + valid_size, dst);
    return valid_size;
}

std::string PlainMemory::dump() const
{
    std::ostringstream oss;
    oss << std::setfill( '0') << std::hex;

    for ( auto it = arena.begin(); it != arena.end(); ++it)
        if ( uint32( *it) != 0)
            oss << "addr 0x" << std::distance(arena.begin(), it)
                << ": data 0x" << uint32( *it) << std::endl;

    return oss.str();
}
