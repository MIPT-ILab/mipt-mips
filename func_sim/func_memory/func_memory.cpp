/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <cstring>
#include <cassert>
#include <cerrno>

// Generic C++
#include <iostream>
#include <sstream>
#include <iomanip>

// uArchSim modules
#include <func_memory.h>

// Local data types
class Address
{
    
    Address(){};
    
public:
    
    Address( uint64 addr,
             uint64 addr_size,
             uint64 page_num_size,
             uint64 offset_size);
    ~Address(){};
    
    uint64 page;
    uint64 set;
    uint64 offset;
};

// Externally visible functions implementation

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
{
    assert( executable_file_name);
    assert( page_bits + offset_bits <= addr_size);

    this->page_size          = 1 << offset_bits;
    this->set_size           = ( 1 << page_bits) * this->page_size;
    this->addr_size          = addr_size;
    this->page_bits          = page_bits;
    this->offset_bits        = offset_bits;
    this->max_addr           = (1 << addr_size) - 1;
    this->set_count          = 1 << ( addr_size - page_bits - offset_bits);
    this->page_in_set_count  = 1 << page_bits;

    vector<ElfSection> sections_array;
    ElfSection::getAllElfSections( executable_file_name, sections_array);
    assert( !sections_array.empty());

    for ( int i = 0; i < sections_array.size(); i++)
    {
        if ( !strcmp( sections_array[i].name, ".text"))
        {
            text_start = sections_array[i].start_addr;
        }
        
        uint64 start_addr = sections_array[i].start_addr;
        
        for ( uint64 cur_addr = start_addr; 
              cur_addr - start_addr < sections_array[i].size;
              cur_addr ++)
        {
            Address parsed_addr( cur_addr, addr_size, page_bits, offset_bits);
            memory[ parsed_addr.set]
                  [ parsed_addr.page]
                  [ parsed_addr.offset] = sections_array[i].content[ cur_addr - start_addr];
        }
    }
}


uint64 FuncMemory::startPC() const
{
    return text_start;
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    assert( num_of_bytes);
    assert( addr < max_addr);
    
    uint64 cur_addr = addr;
    uint64 res = 0;
    uint64 counter = 0;
    
    while ( cur_addr - addr < num_of_bytes)
    {
        Address parsed_addr( cur_addr, addr_size, page_bits, offset_bits);
        
        
        assert( memory.find( parsed_addr.set) == memory.end());                    //checking
        map< uint64, map< uint64, uint8 > > set = memory.at( parsed_addr.set);     //data
                                                                                   //initialization
        assert( set.find( parsed_addr.page) == set.end());                         //
        map< uint64, uint8> page = set.at( parsed_addr.page);                      //
                                                                                   //
        assert( page.find( parsed_addr.offset) == page.end());                     //
        
        
        res += ( page.at(parsed_addr.offset) << counter);                               
        counter += 8;                                                               
        cur_addr++;
    }
    return res;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    assert( num_of_bytes);
    assert( addr < max_addr);
    
    uint64 cur_addr = addr;
    uint64 byte_offset = 0;
    
    while ( cur_addr - addr < num_of_bytes)
    {
        Address parsed_addr( cur_addr, addr_size, page_bits, offset_bits);
        memory[ parsed_addr.set]
              [ parsed_addr.page]
              [ parsed_addr.offset] = ( value & (0xFF << byte_offset)) >> byte_offset;
        byte_offset += 8;
        cur_addr++;
    }
}

string FuncMemory::dump( string indent) const
{
    Address x( text_start, addr_size, page_bits, offset_bits);
    cout << x.set << '\t'  <<  x.page << '\t' << x.offset << endl;
    ostringstream oss;
    oss.fill( '0');
    
    oss << indent << "Memory dump" << endl
        << indent << "  addr_size = "         << this->addr_size << " Bits" << endl
        << indent << "  page_bits = "         << this->page_bits << " Bits" << endl
        << indent << "  offset_bits = "       << this->offset_bits << " Bits" << endl
        << indent << "  set_size = "          << this->set_size << " Bytes" << endl
        << indent << "  page_size = "         << this->page_size << " Bytes" << endl
        << indent << "  text_start = 0x"      << hex << this->text_start << dec << endl
        << indent << "  max_addr = 0x"        << hex << this->max_addr << dec << endl
        << indent << "  set_count = "         << this->set_count << endl
        << indent << "  page_in_set_count = " << this->page_in_set_count << endl
        << indent << "  Data:"                << endl;
    
    for ( uint64 set_num = 0; set_num < set_count; set_num++)
    {
    
    //cout << memory.find( set_num)->first << endl;
        if ( memory.find( set_num) != memory.end())
        {
            oss << indent << "\t set_num = " << set_num << endl;
            map< uint64, map< uint64, uint8 > > set = memory.at( set_num);
            for ( uint64 page_num = 0; page_num < page_in_set_count; page_num++)
            {
                if ( set.find( page_num) != set.end())
                {
                    oss << indent << "\t\t page_num = " << page_num;
                    map< uint64, uint8> page = set.at( page_num);
                    for ( uint64 offset = 0; offset < page_size; offset++)
                    {
                        if ( (offset % 16) == 0)
                        {
                            oss << endl << "\t\t\t";
                        }
                        if ( page.find( offset) != page.end())
                        {
                            oss << "0x" << setw( 2) << hex << ( int) page.at( offset) << dec << " ";
                        } else
                        {
                            oss << "null ";
                        }
                    }
                }
            }
        }
    }
    
    return oss.str();
    
}


// Local functions implementation

Address::Address( uint64 addr,
                  uint64 addr_size,
                  uint64 page_bits,
                  uint64 offset_bits)
{
    assert( page_bits + offset_bits < addr_size);
    
    offset = addr & ( ( 1 << offset_bits) - 1);
    page = ( addr >> offset_bits) & ( (1 << page_bits) - 1);
    set    = addr >> ( offset_bits + page_bits);
}