/**
 * This class implements memory functionality
 * (saving, loading, chenging data) with unuseful memory optimisation.
 * It is used in "func_memory" for making
 * orders siplier.
 * It offers itterface for calls and data save.
 *
 *
**/

#ifndef FUNC_MEMORY__VIRTUAL_MEMORY_H
#define FUNC_MEMORY__VIRTUAL_MEMORY_H

// Generic C
#include <stdlib.h>
#include <assert.h>

// Generic C++
#include "../../common/types.h"
#include <iostream>

class VirtualMemory
{
    uint8** sets;   // this is a main array of data ( 1st level)
    uint64 addr_size;       //--------
    uint64 sets_num_size;   //Sizes in bits. Class is working with
    uint64 page_num_size;   //any sizes, from 1 to 63. (0 is included) 
    uint64 offset_size;     //--------
    uint64 sets_num_mask;   //--Constants wich is defined in the constructor.
    uint64 page_num_mask;   // It is uint64 with 1 bits in same positions.
    uint64 offset_mask;     //--It is using to make code simpler.
    uint64 startPC; // It is pointer to start of executable code;
public:
    VirtualMemory (uint64 addr_size = 32,
                   uint64 page_num_size = 10,
                   uint64 offset_size = 12);
    
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
    uint64 read( uint64 addr, unsigned short num_of_bytes = 4); //const;
    void write( uint64 value, uint64 addr, unsigned short num_of_bytes = 4);
    uint64 setStartPC( uint64 startPC ); //const;  
    uint64 getStartPC( ); //const;  
    //-----

    uint8* allocatePage();  //Functions for
    uint8* allocateSet();   //memory allocating

    ~VirtualMemory();   // Recursive memory freeng

};




#endif //ifndef FUNC_MEMORY__VIRTUAL_MEMORY_H
