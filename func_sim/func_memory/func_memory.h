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
#include <map>

// uArchSim modules
#include <types.h>
#include <elf_parser.h>

using namespace std;

class FuncMemory
{
    map<uint64 /*start addr*/, ElfSection *> sections;
    typedef map<uint64, ElfSection *>::iterator Iter;
    typedef map<uint64, ElfSection *>::const_iterator ConstIter;
    
    uint64 start_PC;

    // You could not create the object
    // using this default constructor
    FuncMemory(){}

public:
    
    FuncMemory( const char* executable_file_name,
                const char* const elf_sections_names[],
                unsigned short num_of_elf_sections);

    virtual ~FuncMemory();
    
    uint64 read( uint64 addr, unsigned short num_of_bytes = 4) const;
    void   write( uint64 value, uint64 addr, unsigned short num_of_bytes = 4);
    
    uint64 startPC() const;
    
    string dump( string indent = "") const;
};

#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H
