/**
 * func_memory.h - Header of module implementing the concept of 
 * programer-visible memory space accesing via memory address.
 * @author Anton Mitrokhin <anton.v.mitrokhin@gmail.com>
 * Copyright 2013 uArchSim iLab project
 */

// protection from multi-include
#ifndef FUNC_MEMORY__FUNC_MEMORY_H
#define FUNC_MEMORY__FUNC_MEMORY_H

// Generic C++
#include <vector>
#include <string>
#include <cassert>

// uArchSim modules
#include <types.h>
#include <elf_parser.h>

using namespace std;


class MemLocation
{
public:
	uint64 set_num;
    uint64 page_num;
    uint64 byte_num;

	MemLocation()
		: set_num( 0), page_num( 0), byte_num( 0) {}

	MemLocation( uint64 addr,               
				 uint64 addr_size = 32,
                 uint64 page_num_size = 10,
                 uint64 offset_size = 12);
};


class Set
{

};


class Page
{

};


class FuncMemory
{
    uint64 addr_size;
    uint64 page_num_size;
    uint64 offset_size;

    uint64 size; // memory size in bytes
    uint8* content; // the raw memory data

    // You could not create the object
    // using this default constructor
    FuncMemory(){}

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

private:
	uint64 mirror( uint64 value, unsigned short num_of_bytes = 4) const;
};

#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H
