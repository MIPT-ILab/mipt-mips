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
#include <string.h>

page::page ()
{
    for (int i = 0; i < SIZE_OF_BYTES_ARRAY; i++)
        bytes_array [i] = '\0';
}


set::set ()
{
    for (int i = 0; i < SIZE_OF_PAGES_ARRAY; i++)
        pages_array [i] = new page;
}


set::~set ()
{
    delete [] pages_array;
}


int FuncMemory::byte_addres (uint64 adress) const
{
    int byte_addres = adress;
    byte_addres &= 0b00000000000000000000111111111111;
    return (int)byte_addres;
}


int FuncMemory::page_addres (uint64 adress) const
{
    int page_addres = adress;
    page_addres &= 0b00000000001111111111000000000000;
    page_addres >>= 12;
    return (int)page_addres;
}


int FuncMemory::set_addres  (uint64 adress) const
{
    int set_addres = adress;
    set_addres &= 0b11111111110000000000000000000000;
    set_addres >>= 20;
    return (int)set_addres;
}


FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
{
    for (int i = 0; i < SIZE_OF_SETS_ARRAY; i++)
        sets_array [i] = new set;

    ElfSection::getAllElfSections(executable_file_name, sections_array);



    // put your code here
}

FuncMemory::~FuncMemory()
{
    delete [] sets_array;

    // put your code here
}

uint64 FuncMemory::startPC() const
{
    // put your code here
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    uint64_type data;

    int byte_pos = byte_addres (addr);
    int page_pos = page_addres (addr);
    int set_pos  = set_addres  (addr);

    for (int i = num_of_bytes; i > 0; i++)
    {
        assert (sizeof (char) - i);

        data.bytes [i] = sets_array  [set_pos ]->
                          pages_array [page_pos]->
                          bytes_array [byte_pos + i];
    }

    // put your code here

    return data.value;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    uint64_type data;
    data.value = value;

    int byte_pos = byte_addres (addr);
    int page_pos = page_addres (addr);
    int set_pos  = set_addres  (addr);

    for (int i = num_of_bytes; i > 0; i++)
    {
        assert (sizeof (char) - i);

        sets_array  [set_pos ]->
        pages_array [page_pos]->
        bytes_array [byte_pos + i] = data.bytes [i];
    }
    // put your code here
}

string FuncMemory::dump( string indent) const
{
    // put your code here
    return string("ERROR: You need to implement FuncMemory!");
}
