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
#include <sstream>
#include <cstdlib>
#include <cassert>
#include <cmath>

// uArchSim modules
#include <func_memory.h>
#include <elf_parser.h>

//My_Lib
#include <m_memory.h>

using namespace std;

///////////////////////////////////////////////////////////////////////////

void M_EXIT_FAIL ( const char *reason )
{
	cout << reason << endl;
	assert ( 0 );
} 

///////////////////////////////////////////////////////////////////////////

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
{	
	//Start check
	if ( ( addr_size > 64 ) || ( page_bits > 64 ) || ( offset_bits > 64 ) )
	{
		cout << "ERROR : Size of memomy, page and offset mustn't be more than 64!\n" 
		<< "addr_size = " << addr_size
		<< "\npage_bits = " << page_bits
		<< "\noffset_bits = " << offset_bits 
		<< endl;
		exit ( EXIT_FAILURE );
	}
	
	//Use ElfParser to get data from ELF file
	vector<ElfSection> section_array;
	ElfSection :: getAllElfSections ( executable_file_name, 
					  section_array );
	
	//Create memory and check
	mem = new Memory( addr_size, page_bits, offset_bits );
	if ( mem <= 0 )
	{
		cout << "ERROR : Don't allocate memory\n";
		exit ( EXIT_FAILURE );
	}

	//Filling our virtual memory	
	for ( unsigned int i = 0; i < section_array.size(); ++i )
	{
		int start_addr = section_array[i].start_addr;
		if ( i == 1 )
		{
			start_PC = start_addr;
		}	
		for ( size_t offset = 0; offset < section_array[i].size; ++offset )
		{		
			if ( mem -> filling ( start_addr + offset, section_array[i].content[offset] ) )
			{
				cout << "ERROR: problem with filling memory\n";
				exit ( EXIT_FAILURE );
			}
		}	
	}
}

///////////////////////////////////////////////////////////////////////////

FuncMemory::~FuncMemory()
{
	delete mem;
}

///////////////////////////////////////////////////////////////////////////

uint64 FuncMemory::startPC() const
{
    	return start_PC;
}

///////////////////////////////////////////////////////////////////////////

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{	
	if ( ( MAX_VAL64 - addr ) < num_of_bytes )
	{
		cout << "ERROR : This addr 0x" << hex << addr 
		<< dec << " and num_of_bytes " << num_of_bytes 
		<< " are too big\n";
		M_EXIT_FAIL ( "READ_FAIL" ); 
	}
	if ( !num_of_bytes )
	{
		cout << "ERROR : num_of_bytes = " << num_of_bytes << endl;
		M_EXIT_FAIL ( "READ_FAIL" );
	}
	uint64 offset = 0;
	uint64 num_page = 0;
	uint64 num_seg = 0;
	uint64 result = 0;
	
	for ( unsigned short i = 0; i < num_of_bytes; ++i )
	{   
		//get offset 
		offset = ( (addr+i) << ( ( sizeof ( addr ) * 8 ) - ( mem -> offset_bits ) ) ) 
				>>  ( (sizeof(addr)*8) - ( mem -> offset_bits ) );	  
		
		//get num_page
		num_page = ( (addr+i) << ( ( sizeof ( addr ) * 8 ) - ( mem -> offset_bits ) - ( mem -> page_bits )  ) ) 
				>> ( (sizeof(addr)*8) - ( mem -> page_bits ) );
		
		//get num_seg
		num_seg = ( (addr+i) >> ( ( mem -> offset_bits ) + ( mem -> page_bits ) ) );
		if ( num_seg > mem -> num_seg )
		{
			cout << "ERROR : This addr 0x" << hex << addr << dec << " is too big\n";
			M_EXIT_FAIL ( "READ_FAIL" ); 
		}
		
		if ( ( mem -> mem[num_seg] ) &&
			( mem -> mem[num_seg] -> Seg[num_page] ) )
		{
			result += ( mem -> mem[num_seg] -> Seg[num_page] -> data_page[offset] ) << i*8;  
		}
		else
		{
			cout << "ERROR : Memory isn't allocated, addr 0x" << hex << addr << dec  << " isn't valid\n";
			M_EXIT_FAIL ( "READ_FAIL" ); 
		}
	}
	return result;
}

///////////////////////////////////////////////////////////////////////////

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    	if ( ( MAX_VAL64 - addr ) < num_of_bytes )
	{
		cout << "ERROR : This addr 0x" << hex << addr 
		<< dec << " and num_of_bytes " << num_of_bytes 
		<< " are too big\n";
		M_EXIT_FAIL ( "WRITE_FAIL" ); 
	}
	if ( value > pow ( 256, num_of_bytes) - 1 )
	{
		cout << "ERROR : value 0x" << hex << value
		<< dec <<  " isn't valid\n";
		M_EXIT_FAIL ( "WRITE_FAIL" );
	}
 	if ( !num_of_bytes )
	{
		cout << "ERROR : num_of_bytes = " << num_of_bytes << endl;
		M_EXIT_FAIL ( "WRITE_FAIL" );
	}
	

	for ( int i = 0; i < num_of_bytes; ++i )
	{
		uint8 data =  ( value & ( 0xFF << i*8 ) ) >> i*8;
		 
		if ( mem -> filling ( addr + i, data ) )
		{
			cout << "ERROR: problem with filling memory\n";
			M_EXIT_FAIL ( "WRITE_FAIL" );
		}
	}
	
}

///////////////////////////////////////////////////////////////////////////

string FuncMemory::dump( string indent ) const
{
	ostringstream oss;

	for ( unsigned int i = 0; i < mem -> num_seg; ++i )
	{
		if ( mem -> mem[i] )
		{
			for ( int j = 0; j < ( mem -> mem[i] -> seg_size ); ++j )
			{
				if ( mem -> mem[i] -> Seg[j] )
				{
					for ( int k = 0; k < ( mem -> mem[i] -> Seg[j] -> size ); ++k )
					{
						if ( mem -> mem[i] -> Seg[j] -> data_page[k] )
						{
							char nul_str = '\0';
							if ( mem -> mem[i] -> Seg[j] -> data_page[k]  < 16 )
								nul_str = '0';

							oss << indent << "    0x" << hex 
							<< ( ( mem -> mem[i] -> Seg[j] -> start_addr ) + k ) 
							<< indent << ":    " << hex  << nul_str
							<< int( mem -> mem[i] -> Seg[j] -> data_page[k] ) <<  endl;
						}
					} 
				}
			}
		}
	}		 
	return oss.str();
}

///////////////////////////////////////////////////////////////////////////










