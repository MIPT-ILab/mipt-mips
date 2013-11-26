/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Anton Mitrokhin <anton.v.mitrokhin@gmail.com>
 * Copyright 2013 uArchSim iLab project
 */

// Generic C

// Generic C++

// uArchSim modules
#include <elf_parser.h>
#include <func_memory.h>

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size_bits,
                        uint64 page_bits,
                        uint64 offset_bits)
{
	addr_size		= addr_size;
    page_num_size	= page_bits;
    offset_size		= offset_bits;

    // extract all ELF sections into the section_array variable
    vector<ElfSection> sections_array;
    ElfSection::getAllElfSections( executable_file_name, sections_array);
        
    // print the information about each section
    for ( int i = 0; i < sections_array.size(); ++i)
		cout << sections_array[ i].dump() << endl;

}

FuncMemory::~FuncMemory()
{
    delete [] this->name;
    delete [] this->content;
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
    ostringstream oss;

    oss << indent << "Dump memory section \"" << this->name << "\"" << endl
        << indent << "  size = " << this->size << " Bytes" << endl
        << indent << "  start_addr = 0x" << hex << this->start_addr << dec << endl
        << indent << "  Content:" << endl;
     
    string str = this->strByBytes();

    // split the contents into words of 4 bytes
    bool skip_was_printed = false;
    for ( size_t offset = 0; offset < this->size; offset += sizeof( uint32))
    {
        string substr =  str.substr( 2 * offset, // 2 hex digits is need per byte
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

uint64 FuncMemory::mirror( uint64 value, unsigned short num_of_bytes = 4) const
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

	cout << value << " " << output << endl; // test
	return output;
}

