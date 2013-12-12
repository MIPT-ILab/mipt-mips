/**
 * func_memory.h - Header of module implementing the concept of 
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// protection from multi-include
#ifndef FUNC_MEMORY__FUNC_MEMORY_H
#define FUNC_MEMORY__FUNC_MEMORY_H

// Generic C

// Generic C++
#include <string>
#include <iostream>
#include <cmath>
#include <sstream>
#include <cstring>

// uArchSim modules
#include <types.h>
#include <elf_parser.h>

using namespace std;

union intdata
{
	uint64 data64;
	uint8 data8[8];
};

class Memory
{
	uint8 ***data;
	uint64 MemSize;
	uint64 SegSize;
	uint64 PageSize;

	Memory(){};
public:
	Memory(uint64 memsize, uint64 segsize, uint64 pagesize);
	
	~Memory();

	bool CreatePage(uint64 segnum, uint64 pagenum);
	bool ExistPage(uint64 segnum, uint64 pagenum);
	bool WriteData(uint64 segnum, uint64 pagenum, uint64 bytenum, uint8 value);
	void WriteDataHard(uint64 segnum, uint64 pagenum, uint64 bytenum, uint8 value);
	uint8 ReadData(uint64 segnum, uint64 pagenum, uint64 bytenum);
	uint64 _MemSize(){ return this->MemSize; };
	uint64 _SegSize(){ return this->SegSize; };
	uint64 _PageSize(){ return this->PageSize; };
};

class FuncMemory
{

	
	uint64 AddrSize;
	uint64 PageBits;
	uint64 OffsetBits;
	uint64 StartAdrr;
		
	FuncMemory();
	
public:
	Memory *memory;

    FuncMemory ( const char* executable_file_name,
                 uint64 addr_size = 32,
                 uint64 page_num_size = 10,
                 uint64 offset_size = 12);
    
    virtual ~FuncMemory();
    
    uint64 read( uint64 addr, unsigned short num_of_bytes = 4) const;
    void   write( uint64 value, uint64 addr, unsigned short num_of_bytes = 4);

	uint64 getAddr(uint64 segnum, uint64 pagenum, uint64 bytenum) const;
    
    uint64 startPC() const;
    
    string dump( string indent = "") const;
};

#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H
