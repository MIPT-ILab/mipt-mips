/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C

// Generic C++
#include <string>
#include <iostream>

// uArchSim modules
#include <func_memory.h>
#include <elf_parser.h>

//My_Lib
#include <m_memory.h>

using namespace std;

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
{
	//Use ElfParser 
	vector<ElfSection> section_array;
	ElfSection :: getAllElfSections ( executable_file_name, 
					  section_array );
	//Create memory
	Memory mem ( addr_size, page_bits, offset_bits ); 
	
	for ( int i = 0; i < section_array.size(); ++i )
	{
		int start_addr = section_array[i].start_addr;	
		for ( size_t offset = 0; offset < section_array[i].size; ++offset )
		{		
			mem.filling ( start_addr + offset, section_array[i].content[offset] );
		} 
	}

}

FuncMemory::~FuncMemory()
{
    // put your code here
}

uint64 FuncMemory::startPC() const
{
    // put your code here
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    // put your code here

    return 0;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    // put your code here
}

string FuncMemory::dump( string indent) const
{
    // put your code here
    return string("ERROR: You need to implement FuncMemory!");
}
