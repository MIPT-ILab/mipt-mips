/**
 * page.cpp - Page class realisation
 * programer-visible memory space accesing via memory address.
 * @author Anton Mitrokhin <anton.v.mitrokhin@gmail.com>
 * Copyright 2013 uArchSim iLab project
 */

// Generic C
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <cassert>

// Generic C++
#include <iostream>
#include <string>
#include <sstream>

// uArchSim modules
#include <func_memory.h>



Page::Page ( uint64 start_addr, 
             uint64 size)
{
    this->start_addr = start_addr;
    this->size = size;

    this->content = new uint8[ this->size + sizeof( uint64)];
}

Page::~Page()
{
    delete [] this->content;
}
    
uint64 Page::read( uint64 addr, unsigned short num_of_bytes = 4) const
{
    if( addr >= size)
    {
        cerr << "ERROR: unable to read "
             << "- the address does not exist" << endl;
        exit( EXIT_FAILURE);
    }

    if( num_of_bytes > 8)
    {
        cerr << "ERROR: unable to read "
             << "the maximum number of bytes to read is 8" << endl;
        exit( EXIT_FAILURE);
    }

    uint64 read_data = (uint64) *( this->content + addr);
    read_data = this->mirror( read_data, num_of_bytes);

    return read_data;
}

void   Page::write( uint64 value, uint64 addr, unsigned short num_of_bytes = 4)
{
    if( addr >= size)
    {
        cerr << "ERROR: unable to read "
             << "- the address does not exist" << endl;
        exit( EXIT_FAILURE);
    }

    if( num_of_bytes > 8)
    {
        cerr << "ERROR: unable to write "
             << "the maximum number of bytes to write is 8" << endl;
        exit( EXIT_FAILURE);
    }

    content[ addr] = value;
}

string Page::dump( string indent) const
{
    ostringstream oss;

    oss << indent << "  size = " << this->size << " Bytes" << endl
        << indent << "  start_addr = 0x" << hex << this->start_addr << dec << endl
        << indent << "  Content:" << endl;
     
    string str = this->strByBytes();

    // split the contents into words of 4 bytes
    bool skip_was_printed = false;
    for ( size_t offset = 0; offset < this->size; offset += sizeof( uint32))
    {
        string substr =  str.substr( 2 * offset, // 2 hex digits are needed per byte
                                     sizeof( uint64));

        if ( substr.compare( "00000000") == 0)
        {   
            if ( !skip_was_printed)
            {
                oss << indent << "  ....  " << endl;
                skip_was_printed = true;
            }
        }
        else
        { 
            oss << indent << "    0x" << hex << ( this->start_addr + offset) 
                << indent << ":    " << substr << endl;
            skip_was_printed = false;
        }
    }

    return oss.str();
}

uint64 Page::mirror( uint64 value, unsigned short num_of_bytes = 4) const
{
	// this function will convert data from big endian to little endian and back
	// in case num_of_bytes equals zero, the function will return zero
	uint64 output = 0;

	const unsigned int max_bit_num = 8 * num_of_bytes - 1;
	for( uint32 iterator = 0; iterator <= max_bit_num; ++iterator)
	{
		unsigned short bit_value = ( ( value & ( 1 << iterator)) >> iterator);
		uint64 mask = (bit_value << ( max_bit_num - iterator));
		output |= mask;
	}

	cout << value << " " << output << endl; // debug
	return output;
}

string Page::strByBytes() const
{
    // temp stream is used to convert numbers into the output string
    ostringstream oss;
    oss << hex;
	
    // convert each byte into 2 hex digits 
    for( size_t i = 0; i < this->size; ++i)
    {
        oss.width( 2); // because we need two hex symbols to print a byte (e.g. "ff")
        oss.fill( '0'); // thus, number 8 will be printed as "08"
        
        // print a value of 
        oss << (uint16) *( this->content + i); // need converting to uint16
                                               // to be not preinted as an alphabet symbol	
    }
    
    return oss.str();
}

string Page::strByWords() const
{
    // temp stream is used to convert numbers into the output string
    ostringstream oss;
    oss << hex;

    // convert each words of 4 bytes into 8 hex digits
    for( size_t i = 0; i < this->size/sizeof( uint32); ++i)
    {
        oss.width( 8); // because we need 8 hex symbols to print a word (e.g. "ffffffff")
        oss.fill( '0'); // thus, number a44f will be printed as "0000a44f"
        
        oss << *( ( uint32*)this->content + i);
    }
    
    return oss.str();
}

