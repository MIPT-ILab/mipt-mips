/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <cstdio>
#include <cstring>
#include <cassert>

// Generic C++
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

// uArchSim modules
#include <func_memory.h>


FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
{
    assert( addr_size);
    assert( page_bits);
    assert( offset_bits);
    assert( addr_size > ( page_bits + offset_bits));

    // Downloading ELF
    ElfSection::getAllElfSections( executable_file_name, sections_array);

    // Initialization of class variables
    uint64 ui1 = 1; // 64-bit version of 1 (used for shifts)
    this->addr_size = addr_size;
    this->page_bits = page_bits;
    this->offset_bits = offset_bits;
    set_bits = addr_size - page_bits - offset_bits;
    if ( addr_size != 64 )
    {
        max_addr = ui1 << addr_size;
    }
    else
    {
        max_addr = MAX_VAL64; // defined in types.h
    }
    max_page = ui1 << page_bits;
    max_offset = ui1 << offset_bits;
    max_set = ui1 << set_bits;

    // Allocating array of sets
    memory = new uint8** [ max_set];
    for ( uint64 i = 0; i < ( max_set); i++)
    {
        memory[ i] = NULL;
    }
    // Downloading Elf Sections into Functional Memory
    uint64 section_start;
    uint64 section_size;
    uint8* section_content;

    for ( uint8 i = 0; i < sections_array.size(); i++)
    {
        section_start = sections_array[ i].start_addr;
        section_size = sections_array[ i].size;
        section_content = sections_array[ i].content;
        for ( uint64 j = 0; j < section_size; j++)
        {
            FuncMemory::write( section_content[ j], section_start + j, 1);
        }
    }
}

FuncMemory::~FuncMemory()
{
    // Disposing of all allocated memory
    for( uint64 i = 0; i < ( max_set); i++)
    {
        if ( memory[ i] != NULL)
        {
            for( uint64 j = 0; j < ( max_page); j++)
            {
                if ( memory[ i][ j] != NULL)
                {
                    delete [] memory[ i][ j];
                }
            }
            delete [] memory[ i];
        }
    }
    delete [] memory;
}

uint64 FuncMemory::startPC() const
{
    return this->sections_array[ 1].start_addr;
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    assert( num_of_bytes != 0);
    assert( num_of_bytes <= 8);
    assert( ( addr + num_of_bytes) <= max_addr); // Else = out of memory range.

    uint64 set = addr >> ( page_bits + offset_bits);
    uint64 page = ( addr << ( 64 - page_bits - offset_bits)) >> ( 64 - page_bits);
    uint64 offset = ( addr << ( 64 - offset_bits)) >> ( 64 - offset_bits);
    uint8* data;
    uint8* data2;
    uint64 data64;
    uint64 le = 0; // little endian of the result
    uint64 set2, page2; // Will be needed, in case of reading from 2 pages

    // Asserting address points to the allocated part of memory
    assert(this->memory[ set]);
    assert(this->memory[ set][ page]);
    data = this->memory[ set][ page] + offset;

    if ( ( offset + num_of_bytes) > max_offset)
    {
        // part of data will be read from the next page
        page2 = page + 1;
        set2 = set;
        if ( page2 == ( max_page))
        {
            page2 = 0;
            set2 += 1;
        }

        // Asserting next page is allocated
        assert(this->memory[ set2]);
        data2 = this->memory[ set2][ page2];
        assert(data2);
        uint8 max = max_offset - offset;

        // Reading byte by byte into "le"
        for ( uint8 i = 0; i < max; i++)
        {
            data64 = data[i];
            le = le | ( data64 << ( 56 - ( i << 3)));
        }
        for ( uint8 i = max; i < num_of_bytes; i++)
        {
            data64 = data2[ i - max ];
            le = le | ( data64 << ( 56 - ( i << 3)));
        }
    }
    else
    {
        // Reading byte by byte into "le":
        // in case page contains all requested data
        for ( uint8 i = 0; i < num_of_bytes; i++)
        {
            data64 = data[i];
            le = le | ( data64 << ( 56 - ( i << 3)));
        }
    }
    return reverse( le);
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    assert( num_of_bytes != 0);
    assert( num_of_bytes <= 8);
    assert( ( addr + num_of_bytes) <= max_addr); // Else = out of memory range.

    uint64 set = addr >> ( page_bits + offset_bits);
    uint64 page = ( addr << ( 64 - page_bits - offset_bits)) >> ( 64 - page_bits);
    uint64 offset = ( addr << ( 64 - offset_bits)) >> ( 64 - offset_bits);
    uint8* data;
    uint64 le = reverse( value); // little endian of "value"
    uint8* data2;
    uint64 page2,set2; // Will be needed, in case of writing into 2 pages

    // Memory allocation, if required
    if ( this->memory[ set] == NULL)
    {
        memory[ set] = new uint8* [ max_page];
        for ( uint64 i = 0; i < max_page; i++)
        {
            memory[ set][ i] = NULL;
        }
    }
    if ( this->memory[ set][ page] == NULL)
    {
        memory[ set][ page] = new uint8 [ max_offset];
        for ( uint64 i = 0; i < max_offset; i++)
        {
            memory[ set][ page][ i] = 0;
        }
    }
    data = this->memory[ set][ page] + offset;

    if ( ( offset + num_of_bytes) > max_offset)
    {
        // Data needs to be written into 2 adjoining pages
        page2 = page + 1;
        set2 = set;
        if ( page2 == ( max_page))
        {
            page2 = 0;
            set2 += 1; // Already asserted, that set2<=max_set, since in memory range
        }

        //  Memory allocation for 2nd page, if required
        if ( this->memory[ set2] == NULL)
        {
            memory[ set2] = new uint8* [ max_page];
            for ( uint64 i = 0; i < ( max_page); i++)
            {
                this->memory[ set2][ i] = NULL;
            }
        }
        if ( this->memory[ set2][ page2] == NULL)
        {
            memory[ set2][ page2] = new uint8 [ max_offset];
            for ( uint64 i = 0; i < max_offset; i++)
            {
                this->memory[ set2][ page2][ i] = 0;
            }
        }

        data2 = this->memory[ set2][ page2];
        uint8 max = max_offset - offset; // Number of bytes on 1st page
        for ( uint8 i = 0; i < max; i++)
        {
            data[ i] = ( le << ( i << 3)) >> 56;
        }
        for ( uint8 i = max; i < num_of_bytes; i++)
        {
            data2[ i - max ] = ( le << ( i << 3)) >> 56;
        }
    }
    else
    {
        for ( uint8 i = 0; i < num_of_bytes; i++)
        {
           data[ i] = ( le << ( i << 3)) >> 56;
        }
    }
    return;
}

string FuncMemory::dump( string indent) const
{
    ostringstream oss;

    oss << indent << "Dump FuncMemory" << endl
        << indent << "  Content:" << endl;
   oss.fill('0'); // used to add insignificant zeros in front of data

    bool skip_was_printed = false;
    uint64 addr = 0;

    // split the contents into words of 4 bytes
    for ( uint64 set = 0; set < max_set; set++)
    {
        if ( memory[ set] != NULL) // If set is not allocated -> skip it
        {
            for ( uint64 page = 0; page < max_page; page++)
            {
                if ( memory[ set][ page] != NULL) // If page is not allocated -> skip it
                {
                    for ( uint64 offset = 0; offset < max_offset; offset += 4)
                    {
                        // Sequentially reading 4 bytes and uniting them into uint32 data
                        uint64 data = 0;
                        for ( uint8 i = 0; i < 4; i++)
                        {
                            uint64 temp = FuncMemory::read( addr + i, 1);
                            data = ( data << 8 ) | temp;
                        }

                        if ( data == 0)
                        {
                            if ( !skip_was_printed)
                            {
                                oss << indent << "  ....  " << endl;
                                skip_was_printed = true;
                            }
                        }
                        else
                        {
                            oss << indent << "    0x" << hex << addr;
                            oss << indent << ":    ";
                            oss.width( 8); // used to add insignificant zeros
                            oss << data << endl;
                            skip_was_printed = false;
                        }
                        addr += 4;
                    }
                }
                else
                {
                    addr += max_offset; // add skipped number of bytes to addr
                }
            }
        }
        else
        {
            addr += max_page << offset_bits; // add skipped number of bytes to addr
        }
    }

    return oss.str();
}

// Returns reverse endian of 64-bit input
// (Little->Big; Big->Little)
uint64 reverse( uint64 input)
{
    uint64 output = input & 1;
    for ( uint8 i = 1; i < 64 ; i++)
    {
        input = input >> 1;
        output = ( output << 1) | input & 1;
    }
    return output;
}
