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
    // You could not create the object
    // using this default constructor
    FuncMemory();

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
    string page_dump(uint64 set_number, uint64 page_number) const;

private:

    void destroy_mem();
    void pars_addr(uint64 addr, uint64 &set_number, uint64 &page_number, uint64 &offset) const;
    void Write_AllElfSections();
    void Write_Section(uint64 sect_number);

    uint64 set_num_size;              // sizes of addresses in bits
    uint64 page_num_size;
    uint64 offset_size;

    uint64 MAX_ADDR;
    uint64 page_size;               // sizes of page, set in bytes
    uint64 pages_array_size;
    uint64 sets_array_size;

    vector<ElfSection> sections_array;  // array for saving sections of elf_file

    uint8 ***mem;   // our memory
};

#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H

