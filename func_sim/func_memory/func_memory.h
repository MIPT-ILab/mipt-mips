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

//--------------------------------------------------------------
#include "virtual_memory.h" //while no changes in the makefile
//--------------------------------------------------------------

using namespace std;

class FuncMemory
{
    uint8** sets;   // this is a main array of data ( 1st level)
    uint64 addr_size;       //--------
    uint64 sets_num_size;   //Sizes in bits. Class is working with
    uint64 page_num_size;   //any sizes, from 3 to 64. 
    uint64 offset_size;     //--------
    uint64 sets_num_mask;   //--Constants wich is defined in the constructor.
    uint64 page_num_mask;   // It is uint64 with 1 bits in same positions.
    uint64 offset_mask;     //--It is using to make code simpler.
    uint64 startPC; // It is pointer to start of executable code;
  
  // You could not create the object
    // using this default constructor
    FuncMemory(){}

public:
    VirtualMemory* memory;
    FuncMemory ( const char* executable_file_name,
                 uint64 addr_size = 32,
                 uint64 page_num_size = 10,
                 uint64 offset_size = 12);
    
    virtual ~FuncMemory();
    
    uint64 read( uint64 addr, unsigned short num_of_bytes = 4); //const;
    void   write( uint64 value, uint64 addr, unsigned short num_of_bytes = 4);
    
    uint64 startPC(); //const;
    
    string dump( string indent = ""); //const;
    
    
        //---Functions for work with data
    //I try to name it like it works.
    uint64 getSetsNum( uint64 addr); //It takes addres
    uint64 getPageNum( uint64 addr); //and returned
    uint64 getOffset( uint64 addr);  //offset in set of elements
    uint8 getByteFromAddr( uint64 addr);
    uint8 getByteFromPages( uint8** pages, uint64 page,//used to make
                            uint64 offset);             //code linel shorter
    void setByteToAddr( uint64 addr, uint8 byte);
    void setByteToPages( uint8** pages, uint64 page,  //used to make
                          uint64 offset, uint8 byte);   //code lines shorter
    uint64 setStartPC( uint64 startPC ); //const;  
    uint64 getStartPC( ); //const;  
    //-----

    uint8* allocatePage();  //Functions for
    uint8* allocateSet();   //memory allocating

    ~VirtualMemory();   // Recursive memory freeng
    
};

#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H
