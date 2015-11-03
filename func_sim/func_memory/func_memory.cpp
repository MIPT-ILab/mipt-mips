/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory addresss.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C

// Generic C++

// uArchSim modules
#include <func_memory.h>
#include <cstring>
#include <cassert>
#include <iostream>
#include <sstream>
#include <cmath>

uint64 BYTES_ARRAY_SIZE = 4096;
uint64 PAGES_ARRAY_SIZE = 1024;
uint64 SETS_ARRAY_SIZE  = 1024;

page::page ()
{
    bytes_array = NULL;
}


page::~page()
{
    delete [] bytes_array;
}

Set::Set ()
{
    pages_array = new page* [PAGES_ARRAY_SIZE];
    
    for (int i = 0; i < PAGES_ARRAY_SIZE; i++)
        pages_array [i] = NULL;
}


Set::~Set ()
{
    for (int i = 0; i < PAGES_ARRAY_SIZE; i++)
    {
        if (!pages_array [i]) continue;
        
        delete pages_array [i];
    }
    delete [] pages_array;
}


void FuncMemory::allocate_new_memory (uint64 addr)
{
    uint64 byte_pos = byteAddress (addr);
    uint64 page_pos = pageAddress (addr);
    uint64 set_pos  = setAddress  (addr);

    if (!sets_array [set_pos])
    {
        sets_array [set_pos] = new Set;
    }
    
    if (!sets_array [set_pos]->pages_array [page_pos])
    {
        sets_array [set_pos]->pages_array [page_pos] = new page;
    }
    
    if (!sets_array [set_pos]->pages_array [page_pos]->bytes_array)
    {
        sets_array  [set_pos ]->
        pages_array [page_pos]-> bytes_array = new uint8 [BYTES_ARRAY_SIZE];
    }
}


uint64 FuncMemory::byteAddress (uint64 address) const
{
    uint64 byteAddress = address;
    byteAddress &= ((uint64)1 << _offset_bits) - 1;

    assert (BYTES_ARRAY_SIZE - byteAddress);
    
    return byteAddress;
}


uint64 FuncMemory::pageAddress (uint64 address) const
{
    uint64 pageAddress = address;
    pageAddress >>= _offset_bits;
    pageAddress &= ((uint64)1 << _page_bits) - 1;

    assert (PAGES_ARRAY_SIZE - pageAddress);
    
    return pageAddress;
}


uint64 FuncMemory::setAddress  (uint64 address) const
{
    uint64 setAddress = address;
    setAddress >>= _page_bits + _offset_bits;

    setAddress &= ((uint64)1 << (_addr_size - _page_bits - _offset_bits)) - 1;
    
    if (SETS_ARRAY_SIZE - setAddress == 0)
    {
        std::cout << "addr = 0x" << hex << address << std::endl; 
        
        std::cout << "sets_array_size = " 
                  << SETS_ARRAY_SIZE << " " 
                  << hex 
                  << SETS_ARRAY_SIZE << std::endl;
        std::cout << "sets_address = "  << setAddress << " " 
                  << hex << setAddress << std::endl;
        
        assert (SETS_ARRAY_SIZE - setAddress);
    }
    return setAddress;
}

void FuncMemory::uint64ToCharsArray (uint64 source       , 
                                     uint8  dest_array []) const
{
    uint64 value = source;
    
    for (int  i = 0; i < sizeof (uint64); i++)
        dest_array [i] = '\0';

    for (int i = 0; i < sizeof (uint64); i++)
    {
        value >>= i * 8               ;
        value &=  ((uint64)1 << 8) - 1;

        dest_array [i] = value ;
        value          = source;
    }
}


void FuncMemory::charsArrayToUint64 (uint8   source_array [], 
                                     uint64* dest           ) const
{
    *dest = (uint64)0;
    
    for (int i = 0; i < sizeof (uint64); i++)
    {
        *dest |= (uint64)source_array [i] << i * 8;
    }
}


FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size                ,
                        uint64 page_bits                ,
                        uint64 offset_bits              )
{
    std::cout << executable_file_name << std::endl; 
    std::cout << addr_size            << std::endl; 
    std::cout << page_bits            << std::endl; 
    std::cout << offset_bits          << std::endl; 
    
    _addr_size   = addr_size  ;
    _page_bits   = page_bits  ;
    _offset_bits = offset_bits;
    
    BYTES_ARRAY_SIZE = (uint64)1 << offset_bits;
    PAGES_ARRAY_SIZE = (uint64)1 << page_bits;
    SETS_ARRAY_SIZE  = (uint64)1 << (addr_size - page_bits - offset_bits);
    
    std::cout << "BYTES_ARRAY_SIZE = " << BYTES_ARRAY_SIZE << std::endl;
    std::cout << "PAGES_ARRAY_SIZE = " << PAGES_ARRAY_SIZE << std::endl;
    std::cout << "SETS_ARRAY_SIZE = "  << SETS_ARRAY_SIZE  << std::endl;
    
    sets_array = new Set* [SETS_ARRAY_SIZE];

    for (int i = 0; i < SETS_ARRAY_SIZE; i++)
    {
        sets_array [i] = NULL;
    }
    ElfSection::getAllElfSections(executable_file_name, sections_array);
    
    for (int i = 0; i < sections_array.size (); i++)
    {
        std::cout << "section is " << sections_array [i].name << std::endl
                  << "size is "    << sections_array [i].size << std::endl;
    
        for (int j = 0; j < sections_array [i].size; j += 4)
        {
            uint8  source_values_array [sizeof (uint64)] = {};
            uint64 dest_value                            = 0 ;
            
            memcpy (source_values_array, sections_array [i].content + j, 4);
            
            charsArrayToUint64 (source_values_array, &dest_value);
            
            write (dest_value, sections_array [i].start_addr + j);
        }
    }
}

FuncMemory::~FuncMemory()
{
    for (int i = 0; i < SETS_ARRAY_SIZE; i++)
    {
        if (!sets_array [i]) continue;
        
        delete sets_array [i];
    }   
    delete [] sets_array;
}

uint64 FuncMemory::startPC() const
{
    std::cout <<  "strat PC ()" << std::endl;

    for (int i = 0; i < sections_array.size (); i ++)
    {
        if (string (sections_array [i].name) == string (".text")) 
            return sections_array [i].start_addr;
    }
    return 0;
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{

    uint64 byte_pos = byteAddress (addr);
    uint64 page_pos = pageAddress (addr);
    uint64 set_pos  = setAddress  (addr);
    
    uint8  source_values_array [sizeof (uint64) + 1] = {};
    uint64 dest_value                                = 0 ;

    if (sets_array [set_pos]                          == NULL ||
        sets_array [set_pos ]->pages_array [page_pos] == NULL ||
        num_of_bytes                                  == 0      )
    {
        std::cout << "CRITICAL ERROR!!!!" << std::endl;
        std::cout << "num_of_bytes = " 
                  << num_of_bytes << std::endl;
            
        terminate ();
    }
    for (int i = 0; i < num_of_bytes; i++)
    {
        assert (i + 1);

        source_values_array [i] = (uint8)(sets_array  [set_pos ]->
                                          pages_array [page_pos]->
                                          bytes_array [byte_pos]);
                                      
        if (BYTES_ARRAY_SIZE - (byte_pos + 1 )) 
            byte_pos ++;
        else if (PAGES_ARRAY_SIZE - (page_pos + 1))
        {
            page_pos ++;
            byte_pos = 0;
        }
        else if (SETS_ARRAY_SIZE - (set_pos + 1))
        {
            page_pos = 0;
            byte_pos = 0;
            set_pos++;
        }
        else assert (0);
    }
    charsArrayToUint64 (source_values_array,
                        &dest_value        );
                        
    return dest_value;
}

void FuncMemory::write(uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    if (num_of_bytes == 0)
    {
        cout << "CRITICAL ERROR IN WRITE!!!!\n" << endl;
        cout << "num_of_bytes = " << num_of_bytes << endl;
            
        terminate ();
    }

    uint64 byte_pos = byteAddress (addr);
    uint64 page_pos = pageAddress (addr);
    uint64 set_pos  = setAddress  (addr);

    allocate_new_memory (addr);

    uint8 dest_values_array [sizeof (uint64)];
    uint64ToCharsArray (value, dest_values_array);

    for (int offset = 0; offset <  num_of_bytes; offset++)
    {
        sets_array  [set_pos ]->
        pages_array [page_pos]->
        bytes_array [byte_pos] = dest_values_array [offset];
        
        if (BYTES_ARRAY_SIZE - (byte_pos + 1 )) 
            byte_pos ++;
        else if (PAGES_ARRAY_SIZE - (page_pos + 1))
        {
            page_pos ++;
            byte_pos = 0;
        }
        else if (SETS_ARRAY_SIZE - (set_pos + 1))
        {
            page_pos = 0;
            byte_pos = 0;
            set_pos++;
        }
        else assert (0);
    }
}

using namespace std;

string FuncMemory::strByWord (uint64 addr) const
{
    ostringstream oss;
    
    oss << hex;
    
    uint64 source;
    uint8  dest_array [sizeof (uint64)] = {};
    
    source  = read (addr);
    
    uint64ToCharsArray (source, dest_array);

    for(int i = sizeof (uint32) - 1; i >= 0; i--)
    {
        oss.width( 2); 
        oss.fill( '0');
        
        oss << (int)dest_array [i];
    }
    return oss.str();
}

string FuncMemory::dump( string indent) const
{
    cout << "start dump" << endl;

    ostringstream oss;
    
    string null_str = "00000000";
    
    bool skip_was_printed = false;
    
    uint64 max_size = 0xffffffff;
    
    ostringstream convert;
    
    for (unsigned set_offset = 0; set_offset < SETS_ARRAY_SIZE; set_offset++)
    {
        if (!sets_array [set_offset]) continue;
    
        for (unsigned page_offset = 0      ;
             page_offset < PAGES_ARRAY_SIZE;
             page_offset++)
        {
            if (!sets_array [set_offset]->pages_array [page_offset]) continue;

            for (unsigned byte_offset = 0      ;
                 byte_offset < BYTES_ARRAY_SIZE;
                 byte_offset += 4)
            {
                uint64 addr = byte_offset;
                addr |= set_offset  << _offset_bits + _page_bits;
                addr |= page_offset << _offset_bits;

                if (strByWord (addr) == null_str)
                {   
                    if ( !skip_was_printed)
                    {
                        oss << indent << "  ....  " << endl;
                        skip_was_printed = true;
                    }
                }
                else
                { 
                    oss << indent << "    0x" << hex << addr
                        << indent << ":    " << strByWord (addr) << endl;
                    skip_was_printed = false;
                }
            }
        }
    }
    return oss.str();
}
