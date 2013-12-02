/**
 * func_memory.h - H./func_memory.h:78:26: error: expected ‘;’ at end of member declaration
eader of module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Anton Mitrokhin <anton.v.mitrokhin@gmail.com>
 * Copyright 2013 uArchSim iLab project
 */

// protection from multi-include
#ifndef FUNC_MEMORY__FUNC_MEMORY_H
#define FUNC_MEMORY__FUNC_MEMORY_H

// Generic C++
#include <vector>
#include <string>
#include <cassert>

// uArchSim modules
#include <types.h>
#include <elf_parser.h>

using namespace std;


class MemLocation
{
public:
    uint64 set_num;
    uint64 page_num;
    uint64 byte_num;

    MemLocation()
        : set_num( 0), page_num( 0), byte_num( 0) {}

    MemLocation( uint64 addr,
                 uint64 addr_size = 32,
                 uint64 page_num_size = 10,
                 uint64 offset_size = 12);

    MemLocation& operator=( const MemLocation& that);
};


class Page
{
    uint8* content; // the raw data of the section

public:
    uint64 size; // size of the page in bytes
    uint64 start_addr; // the start address of the page in set

    Page ( uint64 start_addr,
           uint64 size);

    virtual ~Page();

    uint64 read( uint64 addr, unsigned short num_of_bytes = 4) const;
    void   write( uint64 value, uint64 addr, unsigned short num_of_bytes = 4);
    string dump( string indent = "") const;

private:
    string strByBytes() const;
    string strByWords() const;
};


class Set
{
    Page** content;

public:
    uint64 size; // size of the set in pages
    uint64 page_size; // page size in bytes
    uint64 start_addr; // the start address of the set in memory

    Set( uint64 start_addr,
         uint64 size,
         uint64 page_size);

    virtual ~Set();

    uint64 read( uint64 page_num, uint64 page_offset, unsigned short num_of_bytes = 4) const;
    void   write( uint64 value, uint64 page_num, uint64 page_offset, unsigned short num_of_bytes = 4);
    string dump( string indent = "") const;
};


class FuncMemory
{
    uint64 addr_size;
    uint64 page_num_size;
    uint64 offset_size;
    uint64 startPC_addr;

    Set** content;
    uint64 max_set_number;
    uint64 max_pages_per_set;
    uint64 max_page_size;

    FuncMemory();

public:
    FuncMemory( const char* executable_file_name,
                uint64 addr_size = 32,
                uint64 page_num_size = 10,
                uint64 offset_size = 12);

    virtual ~FuncMemory();

    uint64 read( uint64 addr, unsigned short num_of_bytes = 4) const;
    void   write( uint64 value, uint64 addr, unsigned short num_of_bytes = 4);

    uint64 startPC() const;

    string dump( string indent = "") const;
};

#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H
