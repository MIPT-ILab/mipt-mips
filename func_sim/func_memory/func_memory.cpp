/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C

// Generic C++

// uArchSim modules
#include <func_memory.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <sstream>
#include <iomanip>  

void FuncMemory::memoryCalculator( uint64* first, uint64* second, 
                                   uint64* third, uint64 address) const
{
    *first = address >> ( page_num_size_global + offset_size_global);
    *second = ( address - ( *first << ( page_num_size_global + offset_size_global))) 
              >> offset_size_global;
    *third = address 
           - ( *first << ( page_num_size_global + offset_size_global)) 
           - ( *second << offset_size_global);
}

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
{
    ElfSection::getAllElfSections(executable_file_name, sections_array );
    Memory = NULL;
    addr_size_global = addr_size;
    page_num_size_global = page_bits;
    offset_size_global = offset_bits;
    addr_count = pow( 2, addr_size_global - page_num_size_global - offset_size_global);
    page_num_count = pow( 2, page_num_size_global);
    offset_count = pow( 2, offset_size_global);
    Memory = new uint8**[ addr_count];
    memset( Memory, 0, addr_count*sizeof(uint8**));
    assert( Memory);
    int i = 0;
    uint64 first, second, third;
    while ( sections_array.size() > i)
    {
        for ( int j = 0; j < sections_array[i].size; j++)
        {

            memoryCalculator( &first, &second, &third,
                              sections_array[i].start_addr + j * sizeof( uint8));

            if ( Memory[first] == NULL)
            {
                Memory[ first] = new uint8*[ page_num_count];
                memset( Memory[first], 0, page_num_count * sizeof( uint8*));

            }
            if ( Memory[ first][ second] == NULL)
            {
                if (offset_size_global < 32)
                {
                    Memory[ first][ second] = new uint8[ offset_count];
                    memset( Memory[ first][ second], 0, offset_count * sizeof( uint8));
                }
                else 
                {
                    Memory[ first][ second] = new uint8[ 1000];
                    memset( Memory[ first][ second], 0, 1000 * sizeof( uint8));
                }
            }
        if ( offset_size_global < 32)	
            Memory[ first][ second][ third] = ( uint8) * ( sections_array[ i].content + j);
        }
        i++;
    }
}

FuncMemory::~FuncMemory()
{
    for ( int i = 0; i < addr_count; i++)
        for ( int j = 0; j < page_num_count; j++)
            if ( Memory[ i] != NULL)
                if ( Memory[ i][ j] != NULL)
                    delete[] Memory[i][j];
    for ( int i = 0; i < addr_count; i++)
        if ( Memory[ i] != NULL)
            delete[] Memory[ i];
    if ( Memory != NULL)
        delete[] Memory;
}

uint64 FuncMemory::startPC() const
{
    int i = 0;
    while ( strcmp( ".text", sections_array[ i].name) != 0)
        i++;
    return sections_array[ i].start_addr;
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    if ( !num_of_bytes)
        abort();
    uint64 first, second, third;
    uint64 returnvalue = 0;
    for ( int j = num_of_bytes - 1; j >= 0; j--)
    {
        FuncMemory::memoryCalculator( &first, &second, &third, addr + j * sizeof( uint8));
        if ( first >= addr_count 
          || second >= page_num_count
          || third >= offset_count)
            abort();
        if ( Memory[ first] == NULL) 
            abort();
        if ( Memory[ first][ second] == NULL) 
            abort();
        returnvalue = 256 * returnvalue + ( uint32) Memory[ first][ second][ third];
    }
    return returnvalue;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    if ( !num_of_bytes)
        abort();
    uint64 first, second, third;
    for ( int j = 0; j < num_of_bytes; j++)
    {
        memoryCalculator(&first, &second, &third, addr + j * sizeof(uint8));
        if (first >= addr_count || 
            second >= page_num_count || 
            third >= offset_count || 
            first < 0 || 
            second < 0|| 
            third < 0) 
                abort();
        
        if ( Memory[ first] == NULL)
        {
            Memory[ first] = new uint8*[ page_num_count];
            memset( Memory[ first], 0, page_num_count * sizeof( uint8*));
        }
        if ( Memory[ first][ second] == NULL) 
            {
                Memory[ first][ second] = new uint8[ offset_count];
                memset( Memory[ first][ second], 0, offset_count * sizeof( uint8));
            }
        if ( offset_size_global < 32)
            Memory[ first][ second][ third] = ( uint32)( value % 256);
            value /= 256;
    }
}

string FuncMemory::dump( string indent) const
{
    ostringstream oss;
    for ( uint64 i = 0; i < addr_count; i++)
        for ( uint64 j = 0; j < page_num_count; j++)
            if ( Memory[ i] != NULL)
                if ( Memory[ i][ j] != NULL)
                {
                    oss << endl;
                    uint64 addr;
                    uint64 value;
                    for ( uint64 k = 0; k < offset_count; k += 4)
                    {
                        addr = ( i << ( page_num_size_global + offset_size_global)) + 
                               ( j << ( offset_size_global)) + k;
                        value = ( Memory[ i][ j][ k] << 24) + 
                                ( Memory[ i][ j][ k + 1] << 16) + 
                                ( Memory[ i][ j][ k +2 ] << 8) + 
                                  Memory[ i][ j][ k + 3];
                        if (value != (uint32)0)
                        {
                            oss << showbase << internal << setfill('0')
                                << hex << (uint64)addr << ":    " << noshowbase
                                << setw(8) << (uint32)(value) << endl;
                        }
                    }

                }
    return oss.str();
}
