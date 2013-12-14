/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <stdlib.h>
#include <string.h>

// Generic C++
#include <iostream>
#include <iomanip>
#include <sstream>

// uArchSim modules
#include <func_memory.h>

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_size,
                        uint64 offset_size) :
                    addr_bits( addr_size),
                    page_bits( page_size),
                    offset_bits( offset_size),
                    set_bits( addr_bits - offset_bits - page_bits),
                    offset_mask( ( 1 << offset_bits) - 1),
                    page_mask( ( ( 1 << page_bits) - 1) << offset_bits),
                    set_mask( ( ( 1 << set_bits) - 1) << ( page_bits + offset_bits))
{
    assert( addr_bits <= 64);
    assert( set_bits > 0);
    assert( page_bits > 0);
    assert( offset_bits >= 4);

    // Creating array of sets and its initialization
    uint64 set_cnt = 1 << set_bits;
    mem_set = new uint8** [set_cnt]();

    // Parsing ELF file and first allocation
    vector<ElfSection> sections_array;
    ElfSection::getAllElfSections( executable_file_name, sections_array);
    for ( int i = 0; i < sections_array.size(); i++)
    {
        uint64 start_addr = sections_array[i].start_addr;
        uint64 last_addr = start_addr + sections_array[i].size;
        uint64 addr = start_addr;
        if ( !strcmp( sections_array[i].name, ".text"))
            start_pc = start_addr;
        // Loop for many real pages
        while ( addr != last_addr)
        {
            memAlloc( addr);     // Allocate this particular page in host memory
            uint64 next_page_addr = ( ( addr >> page_bits) + 1) << page_bits;
            int count = ( next_page_addr < last_addr) ? 
                        ( next_page_addr - addr) :
                        ( last_addr - addr);
            // Copy content into memory ( less or equal than 1 page)
            memcpy( getRealAddress( addr),
                   sections_array[i].content + addr - start_addr, 
                   count);
            addr += count;
        }
    }
}


inline uint8** FuncMemory::getSet( uint64 num) const
{
    return mem_set[num];
}


inline uint8* FuncMemory::getPage( uint8** set, uint64 page_num) const
{
    return set[page_num];
}


// Gets host machine adress from virtual address
uint8* FuncMemory::getRealAddress( uint64 addr) const
{
    return getPage( getSet( getSetNum( addr)), getPageNum( addr)) + getOffset( addr);
}


inline uint64 FuncMemory::getGuestPage( uint64 set, uint64 page) const
{
    return ( set << ( page_bits + offset_bits)) + ( page << offset_bits);
}

inline uint64 FuncMemory::getSetNum( uint64 addr) const
{
    return ( addr & set_mask) >> ( page_bits + offset_bits);
}


inline uint64 FuncMemory::getPageNum( uint64 addr) const
{
    return ( addr & page_mask) >> offset_bits;
}


inline uint64 FuncMemory::getOffset( uint64 addr) const
{
    return addr & offset_mask;
}


FuncMemory::~FuncMemory()
{
    uint64 set_cnt = ( 1 << set_bits);
    uint64 page_cnt = ( 1 << page_bits);
    uint8** set;
    uint8* page;
    for ( uint64 i = 0; i < set_cnt; i++)
        if ( ( set = getSet( i)) != NULL)
        {
            for ( uint64 j = 0; j < page_cnt; j++)
                if ( ( page = getPage( set, j)) != NULL)
                    delete [] page;
            delete [] set;
        }
}

uint64 FuncMemory::startPC() const
{
    return start_pc;
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    assert( !memAlloc( addr));
    assert( !memAlloc( addr + num_of_bytes - 1));

    uint64 value( 0);
    for ( int i = 0; i < num_of_bytes; i++)
        ( ( uint8*) &value)[i] = *getRealAddress( addr + i);
    return value;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    memAlloc( addr);
    memAlloc( addr + num_of_bytes - 1);
    for ( int i = 0; i < num_of_bytes; i++)
        *getRealAddress( addr + i) = ( ( uint8*) &value)[i];
}

string FuncMemory::dump( string indent) const
{
    uint64 set_cnt = ( 1 << set_bits);
    uint64 page_cnt = ( 1 << page_bits);
    uint8** set;
    uint8* page;
    string str;
    for ( uint64 i = 0; i < set_cnt; i++)
        if ( ( set = getSet( i)) != NULL)
        {
            for ( uint64 j = 0; j < page_cnt; j++)
                if ( ( page = getPage( set, j)) != NULL)
                    str += dumpPage( page, getGuestPage( i, j));
        }
    // We need to return string and NOT printf in function!
    return str;
}


string FuncMemory::dumpPage( uint8* host_page, uint64 guest_page) const 
{
    uint64 offset_cnt = ( 1 << offset_bits);
    uint64 i( 0);
    ostringstream sstream;
    sstream << setfill( '0') << hex;
    while ( i < offset_cnt)
    {
        if ( host_page[i] == 0)
        {
            i++;
            continue;
        }
        i = ( i >> 4) << 4;      // Align to 4 uint32
        sstream << "0x" << setw( 16) << ( guest_page + i) << ":\t";
        for ( uint64 j = 0; j < 4; j++, i += 4)
        {
            sstream << setw( 2) << ( uint32) host_page[i];
            sstream << setw( 2) << ( uint32) host_page[i + 1];
            sstream << setw( 2) << ( uint32) host_page[i + 2];
            sstream << setw( 2) << ( uint32) host_page[i + 3] << "  ";
        }
        sstream << endl;
    }
    return sstream.str();
}


/* This function will allocate some memory if need
 * We will use it in most of write actions
 */
int FuncMemory::memAlloc( uint64 addr)
{
    uint8*** set = mem_set + getSetNum( addr);
    if ( *set == NULL)
        *set = new uint8* [1 << page_bits]();
    uint8** page = *set + getPageNum( addr);
    if ( *page == NULL){
        *page = new uint8 [1 << offset_bits]();
        return 1;
    }
    return 0;
}


int FuncMemory::memAlloc( uint64 addr) const
{
    uint8** set = getSet( getSetNum( addr));
    if ( set == NULL)
        return 1;
    uint8* page = getPage( set, getPageNum( addr));
    if ( page == NULL){
        return 1;
    }
    return 0;
}

