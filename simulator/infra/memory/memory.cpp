/**
 * memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C++
#include <sstream>
#include <iomanip>

// uArchSim modules
#include "memory.h"

union uint64_8
{
    uint8 bytes[sizeof(uint64) / sizeof(uint8)];
    uint64 val;
};

Memory::Memory( const std::string& executable_file_name,
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
    if ( set_cnt > MAX_VAL32) {
        std::cerr << "ERROR. Memory is divided to too many (" << set_cnt << ") sets\n";
        std::exit( EXIT_FAILURE);
    }
    if ( page_cnt > MAX_VAL32) {
        std::cerr << "ERROR. Each set is divided to too many (" << page_cnt << ") pages\n";
        std::exit( EXIT_FAILURE);
    }
    if ( page_size > MAX_VAL32) {
        std::cerr << "ERROR. Each page is too large (" << page_cnt << " bytes)\n";
        std::exit( EXIT_FAILURE);
    }


    memory = new uint8** [set_cnt]();

    std::list<ElfSection> sections_array;
    ElfSection::getAllElfSections( executable_file_name.c_str(), sections_array);

    for ( const auto& section : sections_array)
    {
        if ( section.name == ".text")
        {
            startPC_addr = section.start_addr;
        }
        for ( size_t offset = 0; offset < section.size; ++offset)
        {
            write( section.content[offset], section.start_addr + offset, 1);
        }
    }
}

Memory::~Memory()
{
    for ( size_t set = 0; set < set_cnt; ++set)
    {
        if (memory[set] != nullptr)
        {
            for ( size_t page = 0; page < page_cnt; ++page)
            {
                if (memory[set][page] != nullptr)
                {
                    delete [] memory[set][page];
                }
            }
            delete [] memory[set];
        }
    }
    delete [] memory;
}

uint64 Memory::read( Addr addr, uint32 num_of_bytes) const
{
    assert( num_of_bytes <= 8);
    assert( num_of_bytes != 0);
    assert( addr <= addr_mask);
    if ( !check( addr) || !check( addr + num_of_bytes - 1))
         return NO_VAL64;

    uint64_8 value;
    value.val = 0ull;

    for ( size_t i = 0; i < num_of_bytes; ++i)
    {
        value.bytes[i] = read_byte( addr + i);
    }

    return value.val;
}

void Memory::write( uint64 value, Addr addr, uint32 num_of_bytes)
{
    assert( addr != 0);
    assert( addr <= addr_mask);
    assert( num_of_bytes != 0 );
    alloc( addr);
    alloc( addr + num_of_bytes - 1);

    if ( addr > addr_mask)
    {
        assert(0);
    }

    uint64_8 value_;
    value_.val = value;

    for ( size_t i = 0; i < num_of_bytes; ++i)
    {
        write_byte( addr + i, value_.bytes[i]);
    }
}

void Memory::alloc( Addr addr)
{
    uint8*** set = &memory[get_set(addr)];
    if ( *set == nullptr)
    {
        *set = new uint8* [page_cnt]();
    }
    uint8** page = &memory[get_set(addr)][get_page(addr)];
    if ( *page == nullptr)
    {
        *page = new uint8 [page_size]();
    }
}

bool Memory::check( Addr addr) const
{
    uint8** set = memory[get_set(addr)];
    return set != nullptr && set[get_page(addr)] != nullptr;
}

std::string Memory::dump( std::string indent) const
{
    std::ostringstream oss;
    oss << indent << std::setfill( '0') << std::hex;

    for ( size_t set = 0; set < set_cnt; ++set)
    {
        if (memory[set] == nullptr)
            continue;

        for ( size_t page = 0; page < page_cnt; ++page)
        {
            if (memory[set][page] == nullptr)
                continue;

            for ( size_t offset = 0; offset < page_size; ++offset)
            {
                if (memory[set][page][offset] == 0)
                    continue;
                oss << "addr 0x" << get_addr( set, page, offset)
                    << ": data 0x" << memory[set][page][offset] << std::endl;
            }
        }
    }

    return oss.str();
}
