/**
 * memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012-2018 uArchSim iLab project
 */

// Generic C++
#include <cassert>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <utility>

// MIPT-MIPS modules
#include <infra/macro.h>

#include "memory.h"

FuncMemory::FuncMemory( const std::string& executable_file_name,
                        uint32 addr_bits,
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
        throw FuncMemoryBadMapping("Too many (" + std::to_string(page_cnt) + ") pages");

    if ( offset_bits >= min_sizeof<uint32, size_t>() * 8)
        throw FuncMemoryBadMapping("Each page is too large " + std::to_string(page_size) + " bytes");

    memory.resize(set_cnt);

    const auto& sections_array = ElfSection::getAllElfSections( executable_file_name);

    for ( const auto& section : sections_array)
    {
        if ( section.get_name() == ".text")
            startPC_addr = section.get_start_addr();

        for ( size_t offset = 0; offset < section.get_size(); ++offset)
            write<uint8>( section.get_byte(offset), section.get_start_addr() + offset);
    }
}

template<typename T>
T FuncMemory::read( Addr addr, T mask) const
{
    assert( addr <= addr_mask);

    T value = 0;

    // Endian specific
    for ( size_t i = 0; i < bitwidth<T> / 8; ++i) {
        if (( mask & 0xFFu) == 0xFFu) {
            auto byte = NO_VAL8;
            if ( check( addr + i))
                byte = read_byte( addr + i);
            value |= static_cast<T>(static_cast<T>(byte) << (i * 8));
        }
        // NOLINTNEXTLINE(misc-suspicious-semicolon)
        if constexpr ( bitwidth<T> > 8) {
            mask >>= 8u;
        }
    }

    return value;
}

template uint8 FuncMemory::read<uint8>( Addr addr, uint8 mask) const;
template uint16 FuncMemory::read<uint16>( Addr addr, uint16 mask) const;
template uint32 FuncMemory::read<uint32>( Addr addr, uint32 mask) const;
template uint64 FuncMemory::read<uint64>( Addr addr, uint64 mask) const;
template uint128 FuncMemory::read<uint128>( Addr addr, uint128 mask) const;

template<typename T>
void FuncMemory::write( T value, Addr addr, T mask)
{
    assert( addr != 0);
    assert( addr <= addr_mask);
    assert( mask != 0);

    // Endian specific
    for ( size_t i = 0; i < bitwidth<T> / 8; ++i) {
        if ((mask & 0xFFu) == 0xFFu) {
            alloc( addr + i);
            write_byte( addr + i, static_cast<uint8>(value & 0xFFu));
        }
        // NOLINTNEXTLINE(misc-suspicious-semicolon)
        if constexpr ( bitwidth<T> > 8) {
            mask >>= 8;
            value >>= 8;
        }
    }
}

template void FuncMemory::write<uint8>( uint8 value, Addr addr, uint8 mask);
template void FuncMemory::write<uint16>( uint16 value, Addr addr, uint16 mask);
template void FuncMemory::write<uint32>( uint32 value, Addr addr, uint32 mask);
template void FuncMemory::write<uint64>( uint64 value, Addr addr, uint64 mask);
template void FuncMemory::write<uint128>( uint128 value, Addr addr, uint128 mask);

void FuncMemory::alloc( Addr addr)
{
    auto& set = memory[get_set(addr)];
    if ( set.empty())
        set.resize(page_cnt);

    auto& page = set[get_page(addr)];
    if ( page.empty())
        page.resize(page_size, 0);
}

bool FuncMemory::check( Addr addr) const
{
    const auto& set = memory[get_set(addr)];
    return !set.empty() && !set[get_page(addr)].empty();
}

std::string FuncMemory::dump() const
{
    std::ostringstream oss;
    oss << std::setfill( '0') << std::hex;

    for ( size_t set_n = 0; set_n < memory.size(); ++set_n)
    {
        const auto& set = memory[ set_n];
        for ( size_t page_n = 0; page_n < set.size(); ++page_n)
        {
            const auto& page = set[ page_n];
            for ( size_t byte_n = 0; byte_n < page.size(); ++byte_n)
            {
                const auto& byte = page[ byte_n];
                if ( byte != 0)
                    oss << "addr 0x" << get_addr( set_n, page_n, byte_n)
                        << ": data 0x" << byte << std::endl;
            }
        }
    }

    return oss.str();
}
