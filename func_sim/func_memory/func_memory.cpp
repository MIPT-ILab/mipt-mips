/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <cstring>

// Generic C++
#include <sstream>

// uArchSim modules
#include <func_memory.h>
#include <elf_parser.h>


Mem_const::Mem_const()
{
    name = new char;
    content = new uint8;
}

Mem_const::~Mem_const()
{}

using std::map;

map< uint64, map< uint64, map< uint64, Mem_const> > > f_memory;
map< uint64, map< uint64, map< uint64, Mem_const> > >::iterator iter_i;
map< uint64, map< uint64, Mem_const> >::iterator iter_j;
map< uint64, Mem_const>::iterator iter_k;

uint64 a = 0;

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
{
    //Parsing file
    vector<ElfSection> section_array;
    ElfSection::getAllElfSections( executable_file_name, section_array);
    cout << "parsed" << endl;
                        
    //Filling map
    for( int i = 0; i < section_array.size(); i++)
    {
        //Getting set, page, offset
        uint64 set_bits = 64 - page_bits + offset_bits;
        uint64 set = section_array[ i].start_addr >> ( page_bits + offset_bits);
        uint64 page = section_array[ i].start_addr << set_bits;
        page = page >> ( set_bits + offset_bits);
        uint64 offset = section_array[ i].start_addr << ( set_bits + page_bits);
        offset = offset >> ( set_bits + page_bits);
        cout << "set, page and offset generated" << endl;

        f_memory[ set][ page][ offset].size = section_array[ i].size;
        f_memory[ set][ page][ offset].start_addr = section_array[ i].start_addr;
        strcpy( f_memory[ set][ page][ offset].name, section_array[ i].name);
        memcpy( f_memory[ set][ page][ offset].content, section_array[ i].content, sizeof( uint8));

    }
    
    
}

FuncMemory::~FuncMemory()
{
    //Cleaning memory
    f_memory.clear();
    cout << "map deleted" << endl;
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
    cout << "dumping" << endl;

    for( iter_i = f_memory.begin(); iter_i!= f_memory.end(); iter_i++)
    {
        for( iter_j = iter_i->second.begin(); iter_j!= iter_i->second.end(); iter_j++)
        {
            for( iter_k = iter_j->second.begin(); iter_k!= iter_j->second.end(); iter_k++)
            {
                oss << indent << "Dump Elf section \"" << ( *iter_k).second.name << "\"" << endl
                    << indent << "  size = " << ( *iter_k).second.size << " Bytes" << endl
                    << indent << "  start_addr = 0x" << ( *iter_k).second.start_addr << endl
                    << indent << "  Content:" << ( *iter_k).second.content << endl; //Must be changed
            }
        }
    }
    return oss.str();;
}
