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
    //printf("%s\n", executable_file_name);
    this->memory = new VirtualMemory(addr_size, page_bits, offset_bits);
    vector<ElfSection> sections;
    ElfSection::getAllElfSections( executable_file_name, sections);
    vector<ElfSection>::iterator section;
    uint64 i;
    for ( section = sections.begin(); section != sections.end(); section++)
    {
        uint64 section_start_addr = section->start_addr;
        uint64 section_size = section->size;
        for( i =0 ; i< section_size; i++)
        {
            this->memory->write( section->content[i] , i + section_start_addr, 4 );
        }
        if( strcmp( section->name, ".text") == 0)
            this->memory->setStartPC( section->start_addr );
    }

}

FuncMemory::~FuncMemory()
{
    this->memory->~VirtualMemory();
}

uint64 FuncMemory::startPC() //const
{
   return this->memory->getStartPC();
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) //const
{
    return this->memory->read( addr, num_of_bytes );
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    this->memory->write( value, addr, num_of_bytes);
}

string FuncMemory::dump( string indent) //const
{
    // put your code here
    return string("ERROR: You need to implement FuncMemory!");
}
