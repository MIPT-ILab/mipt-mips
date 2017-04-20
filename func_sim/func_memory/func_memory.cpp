/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C++
#include <sstream>
#include <iomanip>

// uArchSim modules
#include "func_memory.h"

union uint64_8
{
    uint8 bytes[sizeof(uint64) / sizeof(uint8)];
    uint64 val;
};

FuncMemory::FuncMemory( const std::string& executable_file_name,
                        uint64 addr_bits,
                        uint64 page_bits,
                        uint64 offset_bits) :
    page_bits( page_bits),
    offset_bits( offset_bits),
    set_bits( addr_bits - offset_bits - page_bits),
    addr_mask( addr_bits >= 64 ? ~(0ull) : ( 1ull << addr_bits) - 1),
    offset_mask( ( 1ull << offset_bits) - 1),
    page_mask ( ( ( 1ull << page_bits) - 1) << offset_bits),
    set_mask ( (( 1ull << set_bits) - 1) << ( page_bits + offset_bits)),
    page_cnt ( 1ull << page_bits ),
    set_cnt ( 1ull << set_bits ),
    page_size ( 1ull << offset_bits)
{
    memory = new uint8** [set_cnt]();

    std::vector<ElfSection> sections_array;
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

FuncMemory::~FuncMemory()
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

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    assert( num_of_bytes <= 8);
    assert( num_of_bytes != 0);
    assert( check( addr));
    assert( check( addr + num_of_bytes - 1));
    assert( addr <= addr_mask);

    uint64_8 value;
    value.val = 0ull;

    for ( size_t i = 0; i < num_of_bytes; ++i)
    {
        value.bytes[i] = read_byte( addr + i);
    }

    return value.val;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
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

void FuncMemory::alloc( uint64 addr)
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

bool FuncMemory::check( uint64 addr) const
{
    uint8** set = memory[get_set(addr)];
    return set != nullptr && set[get_page(addr)] != nullptr;
}

std::string FuncMemory::dump( std::string indent) const
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
