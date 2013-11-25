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

// uArchSim modules
#include <func_memory.h>

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_size,
                        uint64 offset_size)
{
    addr_bits = addr_size;
    offset_bits = offset_size;
    page_bits = page_size;
    set_bits = addr_bits - offset_bits - page_bits;

    int set_cnt = 1 << set_bits;
    int page_cnt = 1 << page_bits;
    int offset_cnt = 1 << offset_bits;

    // Creating array of sets and its initialization
    mem_set = ( uint8***) malloc( set_cnt * sizeof( uint8**));
    for ( int i = 0; i < set_cnt; i++)
        mem_set[i] = NULL;

    // Parsing ELF file and first allocation
    vector<ElfSection> sections_array;
    ElfSection::getAllElfSections( executable_file_name, sections_array);
    for (int i = 0; i < sections_array.size(); i++)
    {
        uint64 start_addr = sections_array[i].start_addr;
        uint64 last_addr = start_addr + sections_array[i].size;
        uint64 addr = start_addr;
        // Loop for many real pages
        while ( addr != last_addr)
        {
            memAlloc(addr);     // Allocate this particular page in host memory
            uint64 next_page_addr = ((addr >> page_bits) + 1) << page_bits;
            int count = (next_page_addr < last_addr) ? 
                        (next_page_addr - addr) :
                        (last_addr - addr);
            // Debugging
            uint64 page_num = (addr << (64 - page_bits - offset_bits)) >> (64 - page_bits);
            cout << "Init " << count << "b for " << sections_array[i].name 
                 << "\tPage #" << page_num << endl;
            // End debugging

            // Copy content into memory (less or equal than 1 page)
            memcpy(getRealAddress(addr),
                   sections_array[i].content + addr - start_addr, 
                   count);

            addr += count;
        }

    }
}


// Gets host machine adress from virtual address
uint8* FuncMemory::getRealAddress(uint64 addr)
{
    uint64 set_num = addr >> (offset_bits + page_bits);
    uint64 page_num = (addr << (64 - page_bits - offset_bits)) >> (64 - page_bits);
    uint64 offset = (addr << (64 - offset_bits)) >> (64 - offset_bits);

    return *(*(mem_set + set_num) + page_num) + offset;
}


FuncMemory::~FuncMemory()
{
    // put your code here
}

uint64 FuncMemory::startPC() const
{
    // put your code here
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    // put your code here

    return 0;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    // put your code here
}

string FuncMemory::dump( string indent) const
{
    // put your code here
    return string("ERROR: You need to implement FuncMemory!");
}


/* This function will allocate some memory if need
 * We will use it in most of write actions
 * Note that we use fact that all load operations use ONLY aligned addresses
 */
void FuncMemory::memAlloc(uint64 address)
{
    uint64 set_num = address >> (offset_bits + page_bits);
    uint64 page_num = (address << (64 - page_bits - offset_bits)) >> (64 - page_bits);
    
    uint8*** set = mem_set + set_num;
    if ( *set == NULL)
        *set = (uint8**) malloc((1 << page_bits) * sizeof(uint8*));
    if ( *(*set + page_num) == NULL)
        *(*set + page_num) = (uint8*) malloc(1 << offset_bits);
}


