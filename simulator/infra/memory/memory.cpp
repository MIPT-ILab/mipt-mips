/**
 * memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C++
#include <cassert>
#include <sstream>
#include <iomanip>
#include <iostream>

// MIPT-MIPS modules
#include <infra/macro.h>

#include "memory.h"

union uint64_8
{
    uint8 bytes[sizeof(uint64) / sizeof(uint8)];
    uint64 val;
    explicit uint64_8( uint64 value) : val( value) { }
};

FuncMemory::FuncMemory( const std::string& executable_file_name,
                        uint32 addr_bits,
                        uint32 page_bits,
                        uint32 offset_bits) :
    page_bits( page_bits),
    offset_bits( offset_bits),
    set_bits( addr_bits - offset_bits - page_bits),
    addr_mask( addr_bits >= 64 ? MAX_VAL32 : ( 1ull << addr_bits) - 1),
    offset_mask( ( 1ull << offset_bits) - 1),
    page_mask ( ( ( 1ull << page_bits) - 1) << offset_bits),
    set_mask ( (( 1ull << set_bits) - 1) << ( page_bits + offset_bits)),
    page_cnt ( 1ull << page_bits ),
    set_cnt ( 1ull << set_bits ),
    page_size ( 1ull << offset_bits)
{
    if ( set_bits >= min_sizeof<uint32, size_t>() * 8) {
        std::cerr << "ERROR. Memory is divided to too many (" << set_cnt << ") sets\n";
        std::exit( EXIT_FAILURE);
    }
    if ( page_bits >= min_sizeof<uint32, size_t>() * 8) {
        std::cerr << "ERROR. Each set is divided to too many (" << page_cnt << ") pages\n";
        std::exit( EXIT_FAILURE);
    }
    if ( offset_bits >= min_sizeof<uint32, size_t>() * 8) {
        std::cerr << "ERROR. Each page is too large (" << page_size << " bytes)\n";
        std::exit( EXIT_FAILURE);
    }

    memory.resize(set_cnt);

    const auto& sections_array = ElfSection::getAllElfSections( executable_file_name);

    if ( sections_array.empty()) {
        std::cerr << "ERROR. No ELF sections read from " << executable_file_name << "\n";
        std::exit( EXIT_FAILURE);
    }

    for ( const auto& section : sections_array)
    {
        if ( section.get_name() == ".text")
            startPC_addr = section.get_start_addr();

        for ( size_t offset = 0; offset < section.get_size(); ++offset)
            write( section.get_byte(offset), section.get_start_addr() + offset, 1);
    }
}

uint64 FuncMemory::read( Addr addr, uint32 num_of_bytes) const
{
    if ( num_of_bytes == 0 || num_of_bytes > 8) {
        std::cerr << "ERROR. Reading " << num_of_bytes << " bytes)\n";
        std::exit( EXIT_FAILURE);
    }
    assert( addr <= addr_mask);
    if ( !check( addr) || !check( addr + num_of_bytes - 1))
         return NO_VAL64;

    uint64_8 value(0ull);

    for ( size_t i = 0; i < num_of_bytes; ++i)
        value.bytes[i] = read_byte( addr + i);

    return value.val;
}

void FuncMemory::write( uint64 value, Addr addr, uint32 num_of_bytes)
{
    assert( addr != 0);
    assert( addr <= addr_mask);
    if ( num_of_bytes == 0 || num_of_bytes > 8)
    {
        std::cerr << "ERROR. Writing " << num_of_bytes << " bytes)\n";
        std::exit( EXIT_FAILURE);
    }

    alloc( addr);
    alloc( addr + num_of_bytes - 1);

    const uint64_8 value_( value);

    for ( size_t i = 0; i < num_of_bytes; ++i)
        write_byte( addr + i, value_.bytes[i]);
}

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
