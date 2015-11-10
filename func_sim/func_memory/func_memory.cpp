/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <string.h>

// Generic C++
#include <sstream>
#include <iomanip>

// uArchSim modules
#include <func_memory.h>

union uint64_8
{
    uint8 bytes[sizeof(uint64) / sizeof(uint8)];
    uint64 val;
};

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_bits,
                        uint64 page_bits,
                        uint64 offset_bits) :
    addr_bits( addr_bits),
    page_bits( page_bits),
    offset_bits( offset_bits),
    set_bits( addr_bits - offset_bits - page_bits),
    offset_mask( ( 1 << offset_bits) - 1),
    page_mask ( ( ( 1 << page_bits) - 1) << offset_bits),
    set_mask ( (( 1 << set_bits) - 1) << ( page_bits + offset_bits))
{
    assert( executable_file_name);

    memory = new uint8** [1 << set_bits];
    memset(memory, 0, sizeof(uint8**) * (1 << set_bits));
    
    std::vector<ElfSection> sections_array;
    ElfSection::getAllElfSections( executable_file_name, sections_array);

    for ( vector<ElfSection>::iterator it = sections_array.begin(); it != sections_array.end(); ++it)
    {
        if ( !strcmp( ".text", it->name))
        {
            startPC_addr = it->start_addr;
        }
        for ( size_t offset = 0; offset < it->size; ++offset)
        {
            write( it->content[offset], it->start_addr + offset, 1);
        }
    }
}

FuncMemory::~FuncMemory()
{
    uint64 set_cnt = 1 << set_bits;
    uint64 page_cnt = 1 << page_bits;

    for ( size_t set = 0; set < set_cnt; ++set)
    {
        if (memory[set] != NULL)
        {
            for ( size_t page = 0; page < page_cnt; ++page)
            {
                if (memory[set][page] != NULL)
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
    assert( num_of_bytes != 0 );
    alloc( addr);
    alloc( addr + num_of_bytes - 1);

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
    if ( *set == NULL)
    {
        *set = new uint8* [1 << page_bits];
    	memset(*set, 0, sizeof(uint8*) * (1 << page_bits));
    }
    uint8** page = &memory[get_set(addr)][get_page(addr)];
    if ( *page == NULL)
    {
        *page = new uint8 [1 << offset_bits];
    	memset(*page, 0, sizeof(uint8) * (1 << offset_bits));
    }
}

bool FuncMemory::check( uint64 addr) const
{
    uint8** set = memory[get_set(addr)];
    return set != NULL && set[get_page(addr)] != NULL;
}

string FuncMemory::dump( string indent) const
{
    std::ostringstream oss;
    oss << std::setfill( '0') << hex;
    
    uint64 set_cnt = 1 << set_bits;
    uint64 page_cnt = 1 << page_bits;
    uint64 offset_cnt = 1 << offset_bits;
    
    for ( size_t set = 0; set < set_cnt; ++set)
    {
        if (memory[set] != NULL)
        {
            for ( size_t page = 0; page < page_cnt; ++page)
            {
                if (memory[set][page] != NULL)
                {
                    for ( size_t offset = 0; offset < offset_cnt; ++offset)
                    {
                        if (memory[set][page][offset])
                        {
                            oss << "addr 0x" << get_addr( set, page, offset) 
                                << ": data 0x" << memory[set][page][offset] << std::endl;
                        }
                    }
                }
            }
        }
    }

    return oss.str();
}
