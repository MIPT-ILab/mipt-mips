/**
 * memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012-2018 uArchSim iLab project
 */

#include "memory.h"

// MIPT-MIPS modules
#include <infra/macro.h>

// Generic C++
#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <utility>

FuncMemory::FuncMemory( uint32 addr_bits,
                        uint32 page_bits,
                        uint32 offset_bits) :
    page_bits( page_bits),
    offset_bits( offset_bits),
    set_bits( addr_bits - offset_bits - page_bits),
    addr_mask( bitmask<Addr>( std::min<uint32>( addr_bits, bitwidth<Addr>))),
    offset_mask( bitmask<Addr>( offset_bits)),
    page_mask ( bitmask<Addr>( page_bits) << offset_bits),
    set_mask ( bitmask<Addr>( set_bits) << ( page_bits + offset_bits)),
    page_cnt ( 1ull << page_bits ),
    set_cnt ( 1ull << set_bits ),
    page_size ( 1ull << offset_bits)
{
    using namespace std::literals::string_literals;
    if ( set_bits >= min_sizeof<uint32, size_t>() * 8)
        throw FuncMemoryBadMapping("Too many ("s + std::to_string(set_cnt) + ") sets");

    if ( page_bits >= min_sizeof<uint32, size_t>() * 8)
        throw FuncMemoryBadMapping("Too many ("s + std::to_string(page_cnt) + ") pages");

    if ( offset_bits >= min_sizeof<uint32, size_t>() * 8)
        throw FuncMemoryBadMapping("Each page is too large ("s + std::to_string(page_size) + " bytes)");

    memory.resize(set_cnt);
}

size_t FuncMemory::memcpy_host_to_guest( Addr dst, const Byte* src, size_t size)
{
    assert( dst != 0);
    assert( dst <= addr_mask);

    size_t offset = 0;
    for (; offset < size; ++offset)
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic) Low level access
        alloc_and_write_byte( dst + offset, src[offset]);
    return offset;
}

size_t FuncMemory::memcpy_host_to_guest_noexcept( Addr dst, const Byte* src, size_t size) noexcept try
{
    return memcpy_host_to_guest( dst, src, size);
}
catch (...)
{
    return 0;
}

size_t FuncMemory::memcpy_guest_to_host( Byte *dst, Addr src, size_t size) const
{
    assert( src != 0);

    size_t offset = 0;
    for (; offset < size; ++offset)
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic) Low level access
        dst[offset] = check_and_read_byte( src + offset);
    return offset;
}

size_t FuncMemory::memcpy_guest_to_host_noexcept( Byte *dst, Addr src, size_t size) const noexcept try
{
    return memcpy_guest_to_host( dst, src, size);
}
catch (...)
{
    return 0;
}

void FuncMemory::alloc_and_write_byte( Addr addr, Byte value)
{
    alloc( addr);
    write_byte( addr, value);
}

void FuncMemory::alloc( Addr addr)
{
    auto& set = memory[get_set(addr)];
    if ( set.empty())
        set.resize(page_cnt);

    auto& page = set[get_page(addr)];
    if ( page.empty())
        page.resize(page_size, Byte());
}

bool FuncMemory::check( Addr addr) const
{
    const auto& set = memory[get_set(addr)];
    return !set.empty() && !set[get_page(addr)].empty();
}

void FuncMemory::duplicate_to( FuncMemory* target) const
{
    target->set_startPC( startPC());
    for ( auto set_it = memory.begin(); set_it != memory.end(); ++set_it)
        for ( auto page_it = set_it->begin(); page_it != set_it->end(); ++page_it)
            target->memcpy_host_to_guest( get_addr( set_it, page_it, page_it->begin()),
                                          page_it->data(),
                                          page_it->size());
}

std::string FuncMemory::dump() const
{
    std::ostringstream oss;
    oss << std::setfill( '0') << std::hex;

    for ( auto set_it = memory.begin(); set_it != memory.end(); ++set_it)
        for ( auto page_it = set_it->begin(); page_it != set_it->end(); ++page_it)
            for ( auto byte_it = page_it->begin(); byte_it != page_it->end(); ++byte_it)
                if ( uint32( *byte_it) != 0)
                    oss << "addr 0x" << get_addr( set_it, page_it, byte_it)
                        << ": data 0x" << uint32( *byte_it) << std::endl;

    return oss.str();
}

Addr FuncMemory::get_addr(const Mem::const_iterator& set_it, const Set::const_iterator& page_it, const Page::const_iterator& byte_it) const
{
    return get_addr( std::distance( memory.begin(), set_it),
                     std::distance( set_it->begin(), page_it),
                     std::distance( page_it->begin(), byte_it));
}

inline size_t FuncMemory::get_set( Addr addr) const
{
    return ( addr & set_mask) >> ( page_bits + offset_bits);
}

inline size_t FuncMemory::get_page( Addr addr) const
{
    return ( addr & page_mask) >> offset_bits;
}

inline size_t FuncMemory::get_offset( Addr addr) const
{
    return ( addr & offset_mask);
}

inline Addr FuncMemory::get_addr( Addr set, Addr page, Addr offset) const
{
    return (set << (page_bits + offset_bits)) | (page << offset_bits) | offset;
}

inline Byte FuncMemory::read_byte( Addr addr) const
{
    return memory[get_set(addr)][get_page(addr)][get_offset(addr)];
}

Byte FuncMemory::check_and_read_byte( Addr addr) const
{
    return check( addr) ? read_byte( addr) : Byte{};
}

inline void FuncMemory::write_byte( Addr addr, Byte value)
{
    memory[get_set(addr)][get_page(addr)][get_offset(addr)] = value;
}
