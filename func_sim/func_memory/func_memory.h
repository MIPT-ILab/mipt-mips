/**
 * func_memory.h - Header of module implementing the concept of 
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// protection from multi-include
#ifndef FUNC_MEMORY__FUNC_MEMORY_H
#define FUNC_MEMORY__FUNC_MEMORY_H

// Genecic C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Generic C++
#include <string>
#include <cassert>

// uArchSim modules
#include <types.h>
#include <elf_parser.h>


using namespace std;

class FuncMemory
{
    uint8** sets;   // this is the main array of data ( 1st level)
    
    const uint64 addr_size;      //+
    const uint64 sets_num_size;  //+ Sizes in bits. Class is working with
    const uint64 page_num_size;  //+ any sizes, from 1 to 63. 
    const uint64 offset_size;    //+ 

    const uint64 sets_num_mask; //* Constants wich is defined before construct
    const uint64 page_num_mask; //* It is uint64 with 1 bits in same positions
    const uint64 offset_mask;   //* It is using to make code simpler
  
    uint64 startPC_var;     // It is pointer to start of executable code,
                            // I propogate, it can be changed.
    //FuncMemory(){}

public:
    FuncMemory ( const char* executable_file_name,
                 uint64 addr_size = 32,
                 uint64 page_num_size = 10,
                 uint64 offset_size = 12); // Consts initialisation
    
    // Functions for work with data
    // I try to name it like it works.
    // Read functions -------------------------------------------------
    uint64 read( uint64 addr, unsigned short num_of_bytes = 4) const;
private:
    uint8 getByteFromAddr( uint64 addr) const;
    uint8 getByteFromPages( uint8** pages, uint64 page,// used to make
                            uint64 offset) const;      // code linel shorter
    // Write functions ------------------------------------------------
public:
    void write( uint64 value, uint64 addr, unsigned short num_of_bytes = 4);
private:
    void setByteToAddr( uint64 addr, uint8 byte);
    void setByteToPages( uint8** pages, uint64 page,    // used to make
                          uint64 offset, uint8 byte);   // code lines shorter
    // Service functions ----------------------------------------------
private:
    uint64 getSetsNum( uint64 addr) const; // It takes addres
    uint64 getPageNum( uint64 addr) const; // and returned
    uint64 getOffset( uint64 addr) const;  // offset in set of elements
    uint8* allocatePage();  // Functions for
    uint8* allocateSet();   // memory allocating
    string dumpPage( uint8* page ) const; 
    void setStartPC( uint64 startPC); 
public:
    uint64 getStartPC() const;
    uint64 startPC() const; 
  
    string dump( string indent = "") const;


    virtual ~FuncMemory();
};

#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H

