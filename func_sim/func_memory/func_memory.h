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

class page
{
    public:
    uint8* bytes_array;

    page ();
    ~page ();
};

class Set
{
    public:
    page** pages_array;

    Set  ();
    ~Set ();
};

using namespace std;

class FuncMemory
{
    private :

    Set** sets_array;

    vector <ElfSection> sections_array;
    
    uint64 _addr_size  ;
    uint64 _page_bits  ;
    uint64 _offset_bits;
    
    FuncMemory(){}

    void allocate_new_memory (uint64 addr);
    
    uint64 byteAddress (uint64 address) const;
    uint64 setAddress  (uint64 address) const;
    uint64 pageAddress (uint64 address) const;
    
    void uint64ToCharsArray (uint64 source       , 
                             uint8  dest_array []) const;
                                
    void charsArrayToUint64 (uint8   source_array [], 
                             uint64* dest           ) const;
    

public:

    FuncMemory ( const char* executable_file_name,
                 uint64 addr_size = 32,
                 uint64 page_num_size = 10,
                 uint64 offset_size = 12);

    virtual ~FuncMemory();

    uint64 read( uint64 addr, unsigned short num_of_bytes = 4) const;
    void   write( uint64 value, uint64 addr, unsigned short num_of_bytes = 4);

    uint64 startPC() const;
    
    string strByWord (uint64 addr) const;
    string dump( string indent = "") const;
};

#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H
