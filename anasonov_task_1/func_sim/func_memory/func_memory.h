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
#include <iostream>
#include <sstream>

// uArchSim modules
#include <types.h>
#include <elf_parser.h>

#include <vector>

using namespace std;

class FuncMemory
{
    // You could not create the object
    // using this default constructor
    FuncMemory(){}
    uint64 addr_size_tmp;
    uint64 page_bits_tmp;
    uint64 offset_bits_tmp;
    vector<vector<vector<uint8> > > memory;
    uint64 start_addres;
        

public:

    FuncMemory ( const char* executable_file_name,
                 uint64 addr_size = 32,
                 uint64 page_num_size = 10,
                 uint64 offset_size = 12);
    
    virtual ~FuncMemory();
    
    uint64 read( uint64 addr, unsigned short num_of_bytes = 4) const;
    void   write( uint64 value, uint64 addr, unsigned short num_of_bytes = 4);
    
    uint64 startPC() const;
    
    string dump( string indent = "") const;
};

uint64 maskfunc(uint64 addr_size_len, uint64 size_right, uint64 size);
uint8 invert(uint8 var);
#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H
