/**
 * func_memory.cpp - the module implementing the concept of
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
#include <cmath>

// Generic C++
#include <iostream>
#include <string>
#include <sstream>

// uArchSim modules
#include <elf_parser.h>
#include <func_memory.h>


using namespace std;

union Mirrorer
{
    uint64 val;
    uint8 byte[ 8];
};

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size_bits,
                        uint64 page_bits,
                        uint64 offset_bits)
{
    this->addr_size     = addr_size_bits;
    this->page_num_size    = page_bits;
    this->offset_size   = offset_bits;

    this->max_set_number = pow( 2, addr_size_bits - page_bits - offset_bits);
    this->max_pages_per_set = pow( 2, page_bits);
    this->max_page_size = pow( 2, offset_bits);

    this->content = new Set*[ this->max_set_number];
    for( size_t iterator = 0; iterator < ( this->max_set_number); iterator++)
        ( this->content)[ iterator] = NULL;


    // extract all ELF sections into the section_array variable
    vector<ElfSection> sections_array;
    ElfSection::getAllElfSections( executable_file_name, sections_array); //*********

    // print the information about each section
    for( int i = 0; i < sections_array.size(); ++i)
    {
        if( strcmp( sections_array[ i].name, ".text") == 0)
            this->startPC_addr = sections_array[ i].start_addr;

        for( int offset = 0; offset < sections_array[ i].size; offset++)
            this->write( sections_array[ i].content[ offset], sections_array[ i].start_addr + offset, 1);

    }

}

FuncMemory::~FuncMemory()
{
    for( size_t iterator = 0; iterator < ( this->max_set_number); iterator++)
        delete content[ iterator];
    delete [] this->content;
}

uint64 FuncMemory::startPC() const
{
    return startPC_addr;
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    assert( num_of_bytes != 0);
    Mirrorer byte_to_int;
    byte_to_int.val = 0;

    for ( uint64 byte_addr = addr, i = 0; byte_addr < addr + num_of_bytes; byte_addr++, i++)
    {
        MemLocation memlock( byte_addr,
                             this->addr_size,
                             this->page_num_size,
                             this->offset_size);

        assert( content[ memlock.set_num] != NULL);

        Set* set = content[ memlock.set_num];
        byte_to_int.byte[ i] = (uint8)set->read( memlock.page_num, memlock.byte_num, 1);
        //cout << i << ":" << hex << (int)byte_to_int.byte[ i] << dec << endl;
    }

    //cout << ":" << hex << (int)byte_to_int.val << endl << endl;
    return byte_to_int.val;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    assert( num_of_bytes != 0);
    Mirrorer int_to_byte;
    int_to_byte.val = value;

    for ( uint64 byte_addr = addr, i = 0; byte_addr < addr + num_of_bytes; byte_addr++, i++)
    {
        MemLocation memlock( byte_addr,
                             this->addr_size,
                             this->page_num_size,
                             this->offset_size);

        if( content[ memlock.set_num] == NULL)
            content[ memlock.set_num] = new Set( memlock.set_num, this->max_pages_per_set, this->max_page_size);

        /* This function will write bytes one by one */
        content[ memlock.set_num]->write( int_to_byte.byte[ i], memlock.page_num, memlock.byte_num, 1);
    }
}

string FuncMemory::dump( string indent) const
{
    ostringstream oss;

    oss << indent << "Dump memory section..." << endl;

    for( size_t iterator = 0; iterator < ( this->max_set_number); iterator++)
    {
        if( this->content[ iterator] != NULL)
        {
            oss << (this->content)[ iterator]->dump( "  ");
        }
    }

    return oss.str();
}

