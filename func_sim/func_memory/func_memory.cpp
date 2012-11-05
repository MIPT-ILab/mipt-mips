/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <cassert>
#include <cstdlib>

// Generic C++
#include <string> 

// uArchSim modules
#include <func_memory.h>

FuncMemory::FuncMemory( const char* executable_file_name,
                        const char* const elf_sections_names[],
                        short num_of_elf_sections)
{
    // Change it with your implementation.
    assert(0);
}

FuncMemory::~FuncMemory()
{
    // Change it with your implementation.
    assert(0);
}

uint64 FuncMemory::read( uint64 addr, short num_of_bytes) const
{
    // Change it with your implementation.
    assert(0);
    return NO_VAL64; 
}

string FuncMemory::dump( string indent) const
{
    // Change it with your implementation
    assert(0);
    return " ";
}

