/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <cstring>
#include <cassert>

// Generic C++

// uArchSim modules
#include <func_memory.h>

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
{
    page_size = 2 << offset_bits;
    set_size = 2 << page_bits;
    this->addr_size = addr_size;
    this->page_bits = page_bits;
    this->offset_bits = offset_bits;
    
    assert( executable_file_name);
    vector<ElfSection> sections_array;
    ElfSection::getAllElfSections( executable_file_name, sections_array);
    assert( sections_array.empty());
    for ( int i = 0; i < sections_array.size(); i++)
    {
        if ( !strcmp( sections_array[i].name, ".text"))
        {
            text_start = sections_array[i].start_addr;
        }
        
        uint64 start_addr = sections_array[i].start_addr;
        
        for ( uint64 cur_addr = start_addr; cur_addr - start_addr < sections_array[i].size; cur_addr += 1)
        {
            Address parsed_addr( cur_addr, addr_size, page_bits, offset_bits);
            memory[parsed_addr.set]
                  [parsed_addr.page]
                  [parsed_addr.offset] = sections_array[i].content[cur_addr - start_addr];
        }
    }
}

Address::Address ( uint64 addr,
                   uint64 addr_size,
                   uint64 page_bits,
                   uint64 offset_bits)
{
    offset = addr & ( ( 2 << offset_bits) - 1);
    page   = addr & ( ( 2 << ( offset_bits + page_bits)) - 1) - offset;
    set    = addr & - offset - page;
}


uint64 FuncMemory::startPC() const
{
    return text_start;
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes)
{
    uint64 cur_addr = addr;
    uint64 res = 0;
    uint64 counter = 0;
    while ( cur_addr - addr < num_of_bytes)
    {
        Address parsed_addr( cur_addr, addr_size, page_bits, offset_bits);
        res += ( memory[parsed_addr.set][parsed_addr.page][parsed_addr.offset] << counter);
        counter += 8;
        cur_addr++;
    }
    return res;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    uint64 cur_addr = addr;
    uint64 counter = 0;
    while ( cur_addr - addr < num_of_bytes)
    {
        Address parsed_addr( cur_addr, addr_size, page_bits, offset_bits);
        memory[parsed_addr.set][parsed_addr.page][parsed_addr.offset] = value & (0xFF << counter);
        counter += 1;
        cur_addr++;
    }
}

string FuncMemory::dump( string indent) const
{
    return string("ERROR: You need to implement FuncMemory!");
}
