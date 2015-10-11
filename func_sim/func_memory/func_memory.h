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

// uArchSim modules
#include <types.h>
#include <elf_parser.h>

using namespace std;

class FuncMemory
{
    // You could not create the object
    // using this default constructor
    FuncMemory(){}

    void increase_offset_priv( unsigned& offset, unsigned& num_of_page, unsigned& num_of_set, unsigned inc_by) const;
    void decrease_offset_priv( unsigned& offset, unsigned& num_of_page, unsigned& num_of_set, unsigned dec_by) const;
    
    uint64 num_of_sets_priv;
    uint64 num_of_pages_priv;
    uint64 max_offset_priv;

    char* exe_file_name_priv;
    uint64 addr_size_priv;
    uint64 page_num_size_priv;
    uint64 offset_size_priv;

    uint8*** sets_array_priv;

    uint64 start_pc_adress_priv;
    const static uint64 begin_addr_priv = 0x400094;

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

#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H
