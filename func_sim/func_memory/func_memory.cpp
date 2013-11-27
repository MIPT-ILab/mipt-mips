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

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
{
    //Parsing file
    vector<ElfSection> section_array;
    ElfSection::getAllElfSections( executable_file_name, section_array);
    
    //Filling map
    for( int i = 0; i < section_array.size(); i++)
    {
        uint32 set_bits = addr_size - page_bits + offset_bits;
        uint32 set = section_array[ i].start_addr >> ( page_bits + offset_bits);
        uint32 page = section_array[ i].start_addr << set_bits;
        page = page >> ( set_bits + offset_bits);
        uint32 offset = section_array[ i].start_addr << ( set_bits + page_bits);
        offset = offset >> ( set_bits + page_bits);

        memory[set][page][offset].name = section_array[ i].name;
        memory[set][page][offset].content = section_array[ i].content;

    }
}

FuncMemory::~FuncMemory()
{
    //Cleaning memory
    memory.clear();
}

uint64 FuncMemory::startPC() const
{
    
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{

    /*uint32 set_size = -1 >> page_bits + offset_bits;
    uint32 page_size = -1 << set_size >> ( set_size + page_bits);*/
    return 0;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    // put your code here
}

string FuncMemory::dump( string indent) const
{
    string output;

    /*for( int i = 0; i < memory.size(); i++)
    {
        for( int j = 0; j < memory.size(); j++)
        {
            for( int k = 0; k < memory.size(); k++)
            {
                if( memory[i][j][k]!= NULL)
                {
                    output+= "Dump ELF section \"%s\"\n";
                    output+= "size = %d Bytes\n";
                    output+= "start_addr = %x\n";
                    output+= "Content:\n";
                    output+= memory[i][j][k];
                }
            }
        }
    }*/
    return output;
}
