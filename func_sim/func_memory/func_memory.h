/**
 * func_memory.h - Header of module implementing the concept of 
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// protection from multi-include
#ifndef FUNC_MEMORY__FUNC_MEMORY_H
#define FUNC_MEMORY__FUNC_MEMORY_H

// Generic C++
#include <string>
#include <cassert>

// uArchSim modules
#include <types.h>
#include <elf_parser.h>

using namespace std;

class FuncMemory
{

    /* Main memory pointer
     * This is an array of pointers to sets, 
     * any set is an array of pages,
     * that are pointers to host memory, just to allocated page.
     * guest_mem[setNum] - pointer to set;
     * guest_mem[setNum][pageNum] - pointer to page;
     * guest_mem[setNum][pageNum][offset] - guest data.
     */
    uint8 ***guest_mem;

    // Check if this address allocated, otherwise allocate it
    int memAlloc(uint64 address);
    int checkAlloc(uint64 address) const;

    // Gets host adress from guest address
    uint8* getRealAddress(uint64 addr) const;

    // Small and simple address computations
    inline uint64 getGuestPage(uint64 set, uint64 page) const;
    inline uint64 getSetNum(uint64 addr) const;
    inline uint64 getPageNum(uint64 addr) const;
    inline uint64 getOffset(uint64 addr) const;

    // Dump one page
    string dumpPage(uint8* host_page, uint64 guest_page) const;

    // PC start position taked from ELF file
    uint64 start_pc;

    // Some basic constants. Order is significant
    const uint64 addr_bits;
    const uint64 offset_bits;
    const uint64 page_bits;
    const uint64 set_bits;

    const uint64 offset_mask;
    const uint64 page_mask;
    const uint64 set_mask;
public:

    FuncMemory ( const char* executable_file_name,
                 uint64 addr_size = 32,
                 uint64 page_size = 10,
                 uint64 offset_size = 12);
    
    virtual ~FuncMemory();
    
    uint64 read( uint64 addr, unsigned short num_of_bytes = 4) const;
    void   write( uint64 value, uint64 addr, unsigned short num_of_bytes = 4);
    
    uint64 startPC() const;
    
    string dump( string indent = "") const;
};

#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H

