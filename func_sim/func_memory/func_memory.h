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
#include <vector>
#include <map>

// uArchSim modules
#include <types.h>
#include <elf_parser.h>

using namespace std;

class FuncMemory
{
    // You could not create the object
    // using this default constructor
    FuncMemory(){}
    map< uint64, map< uint64, map< uint64, uint8 > > > memory;
    uint64 text_start;
    uint64 set_size;
    uint64 page_size;
    uint64 page_bits;
    uint64 offset_bits;
    uint64 addr_size;
    uint64 max_addr;
    uint64 set_count;
    uint64 page_in_set_count;
    
public:

    FuncMemory ( const char* executable_file_name,
                 uint64 addr_size = 32,
                 uint64 page_num_size = 10,
                 uint64 offset_size = 12);
    
    virtual ~FuncMemory(){};
    
    uint64 read( uint64 addr, unsigned short num_of_bytes = 4) const;
    void   write( uint64 value, uint64 addr, unsigned short num_of_bytes = 4);
    
    uint64 startPC() const;
    
    string dump( string indent = "") const;
};


#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H
