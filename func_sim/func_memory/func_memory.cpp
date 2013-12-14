/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Aleksandr Kayda <aleksandr.kajda@frtk.ru>
 * Copyright 2013 uArchSim iLab project
 */

// Generic C
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
    assert( offset_bits >= 4);  // Implementation limiting (dump)

    // Creating array of sets
    guest_mem = new uint8** [1 << set_bits]();

    // Parsing ELF file and first allocation
    vector<ElfSection> section;
    ElfSection::getAllElfSections( executable_file_name, section);
    for ( int i = 0; i < section.size(); i++)
    {
        if ( !strcmp( section[i].name, ".text"))
            start_pc = section[i].start_addr;
        // This is slow and simple way to copy content. 
        // But init dont need to be very fast, I think.
        for ( uint64 cnt = 0 ; cnt < section[i].size; cnt++)
            write( section[i].content[cnt], section[i].start_addr + cnt, 1);
    }
}


// Gets host machine adress from virtual address
uint8* FuncMemory::getRealAddress( uint64 addr) const
{
    return guest_mem[getSetNum(addr)][getPageNum(addr)] + getOffset(addr);
}

// Gets guest address of specified page
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
    // Deallocate all guest memory
    for ( uint64 i = 0; i < set_cnt; i++)
        if ( ( set = guest_mem[i]) != NULL)
        {
            for ( uint64 j = 0; j < page_cnt; j++)
                if ( ( page = set[j]) != NULL)
                    delete [] page;
            delete [] set;
        }
    delete [] guest_mem;
}


uint64 FuncMemory::startPC() const
{
    return start_pc;
}


uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    assert( num_of_bytes != 0);
    // Guest can't read from address, that is not initialized by writing to it
    assert( checkAlloc( addr) == 0);
    assert( checkAlloc( addr + num_of_bytes - 1) == 0);

    uint64 value( 0);
    for ( int i = 0; i < num_of_bytes; i++)
        ( ( uint8*) &value)[i] = *getRealAddress( addr + i);
    return value;
}


void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    assert( num_of_bytes != 0);
    memAlloc( addr);
    memAlloc( addr + num_of_bytes - 1);
    for ( int i = 0; i < num_of_bytes; i++)
        *getRealAddress( addr + i) = ( ( uint8*) &value)[i];
}


/* Dump guest memory and print all non-zero allocated values */
string FuncMemory::dump( string indent) const
{
    uint64 set_cnt = ( 1 << set_bits);
    uint64 page_cnt = ( 1 << page_bits);
    uint8** set;
    uint8* page;
    string str;
    for ( uint64 i = 0; i < set_cnt; i++)
        if ( ( set = guest_mem[i]) != NULL)
            for ( uint64 j = 0; j < page_cnt; j++)
                if ( ( page = set[j]) != NULL)
                    str += dumpPage( page, getGuestPage( i, j));
    return str;
}


/* Receive address of the page in host memory and
 * address of the same page in guest memory.
 * Output consists of guest adresses and its values
 */
string FuncMemory::dumpPage( uint8* host_page, uint64 guest_page_addr) const 
{
    uint64 offset_cnt = ( 1 << offset_bits);
    uint64 i( 0);
    ostringstream sstream;
    sstream << setfill( '0') << hex;
    while ( i < offset_cnt)
    {
        if ( host_page[i] == 0)     // Skip zeroes
        {
            i++;
            continue;
        }
        // Then print guest address and 16 aligned bytes
        i = ( i >> 4) << 4;
        sstream << "0x" << setw( 16) << ( guest_page_addr + i) << ":\t";
        for ( int j = 0; j < 4; j++, i += 4)    // Print 4 bytes, 4 times
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


/* This method allocate some memory if it is necessary
 * We will use it in most of write actions
 */
int FuncMemory::memAlloc( uint64 addr)
{
    uint8*** set = guest_mem + getSetNum( addr);
    if ( *set == NULL)
        *set = new uint8* [1 << page_bits]();
    uint8** page = *set + getPageNum( addr);
    if ( *page == NULL){
        *page = new uint8 [1 << offset_bits]();
        return 1;
    }
    return 0;
}


/* Const method, only check for allocation of this address.
 * If allocated, return 0
 */
int FuncMemory::checkAlloc( uint64 addr) const
{
    uint8** set = guest_mem[getSetNum( addr)];
    if ( set == NULL)
        return 1;
    if ( set[getPageNum( addr)] == NULL)
        return 1;
    return 0;
}

