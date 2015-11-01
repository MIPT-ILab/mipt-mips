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
#include <string.h>
#include <stdio.h>
//#include <stdlib.h>
#include <cassert>
#include <iostream>
#include <sstream>
#include <cmath>

uint64 BYTES_ARRAY_SIZE = 4096;
uint64 PAGES_ARRAY_SIZE = 1024;
uint64 SETS_ARRAY_SIZE  = 1024;

page::page ()
{
    //for (int i = 0; i < BYTES_ARRAY_SIZE; i++)
        bytes_array = NULL;//new /*unsigned char*/uint8 [BYTES_ARRAY_SIZE];
}


page::~page()
{
    //for (int i = 0; i < BYTES_ARRAY_SIZE; i++)
        //delete bytes_array;
        
    delete [] bytes_array;
    
    printf ("bytes deleted\n");
}

Set::Set ()
{
    for (int i = 0; i < PAGES_ARRAY_SIZE; i++)
    pages_array = new page* [PAGES_ARRAY_SIZE];
    
    for (int i = 0; i < PAGES_ARRAY_SIZE; i++)
        pages_array [i] = NULL;
        
        
    //printf ("pages_array created\n");
}


Set::~Set ()
{
    for (int i = 0; i < PAGES_ARRAY_SIZE; i++)
    {
        if (!pages_array [i]) continue;
        
        delete pages_array [i];
        
        printf ("deleted pages [%x]\n", i);
    }
    
    delete [] pages_array;
    
    printf ("deleted pages_array \n");
}


void FuncMemory::allocate_new_memory (uint64 addr)
{
    uint64 byte_pos = byte_address (addr);
    uint64 page_pos = page_address (addr);
    uint64 set_pos  = set_address  (addr);

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
        sets_array [set_pos]->pages_array [page_pos]->bytes_array = new uint8 [BYTES_ARRAY_SIZE];
    }
}


uint64 FuncMemory::byte_address (uint64 address) const
{
    uint64 byte_address = address;
    byte_address &= ((uint64)1 << _offset_bits) - 1;//0b00000000000000000000111111111111;
    
    /*std::cout << "addr = " << hex << address << " "
              << "byte bits = " << hex << _offset_bits << " "
              << "offset = " << hex << ((uint64)1 << _offset_bits) - 1 << " "
              << "byte_address = " << hex << byte_address << std::endl;*/
    
    assert (/*printf ("byte_address = %d\n", byte_address) &&*/ BYTES_ARRAY_SIZE - byte_address);
    
    return byte_address;
}


uint64 FuncMemory::page_address (uint64 address) const
{
    uint64 page_address = address;
    page_address >>= _offset_bits;
    page_address &= ((uint64)1 << _page_bits) - 1;//0b00000000001111111111000000000000;
    
    /*std::cout << "addr = " << hex << address << " "
              << "page bits = " << hex << _page_bits << " "
              << "offset = " << hex << ((uint64)1 << _page_bits) - 1 << " "
              << "page_address = " << hex << page_address << std::endl;*/
    
    
    assert (PAGES_ARRAY_SIZE - page_address);
    
    return page_address;
}


uint64 FuncMemory::set_address  (uint64 address) const
{
    uint64 set_address = address;
    set_address >>= _page_bits + _offset_bits;
    
    //printf ("max size = %u\n", (uint64)1 << (_addr_size - _page_bits - _offset_bits) - 1);
    
    set_address &= ((uint64)1 << (_addr_size - _page_bits - _offset_bits)) - 1;//0b11111111110000000000000000000000;
    
    /*std::cout << "addr = " << hex << address << " "
              << "addr bits = " << hex << _addr_size - _page_bits - _offset_bits << " "
              << "offset = " << hex << ((uint64)1 << (_addr_size - _page_bits - _offset_bits)) - 1 << " "
              << "set_address = " << hex << set_address << std::endl;*/
    
    if (SETS_ARRAY_SIZE - set_address == 0)
    {
        std::cout << "addr = 0x" << hex << address << std::endl; 
        
        std::cout << "sets_array_size = " << SETS_ARRAY_SIZE << " " <<hex << SETS_ARRAY_SIZE << std::endl;
        std::cout << "sets_address = " << set_address << " " << hex << set_address << std::endl;
        
        assert (SETS_ARRAY_SIZE - set_address);
    }
    return set_address;
}

void FuncMemory::uint64_to_chars_array (uint64 source, uint8 dest_array [], int empty_space) const
{
    //printf ("\nstart uint64 to chars\n");

    uint64_type converter;
    converter.value = source;
    
    //printf ("content = %x, uint64_value = %lu, empty_space = %d, uint8_values_array [0] = %d\n", content.value, uint64_value, empty_space, uint8_values_array [0]);
    
    for (int i = 0; i < sizeof (source); i++)
    {
        
        if (i < empty_space) 
            dest_array [i] = 0;
        else
            dest_array [i] = converter.bytes [i];
    }
    //printf ("finish converting uint64 to chars\n\n");
}


void FuncMemory::chars_array_to_uint64 (uint8 source_array [], uint64* dest, int empty_space) const
{
    //printf ("\nstart converting char to uint64\n");
    
    /*for (int i = 0; i < 8; i++)
        printf ("%0.2x", source_array [i]);
    printf ("\n");*/
    
    uint64_type converter;
    
    for (int  i = 0; i < sizeof (*dest); i++)
    {
        if (i < empty_space) 
            converter.bytes [i] = 0;
        else
            converter.bytes [i] = source_array [i];
    }
    *dest = converter.value;
    
    //printf ("finish convrting from chars to uint64\n");
}


FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
{
    printf ("%s\n", executable_file_name); 
    printf ("%d\n", addr_size); 
    printf ("%d\n", page_bits); 
    printf ("%d\n", offset_bits); 
    
    _addr_size   = addr_size  ;
    _page_bits   = page_bits  ;
    _offset_bits = offset_bits;
    
    BYTES_ARRAY_SIZE = (uint64)1 << offset_bits;
    PAGES_ARRAY_SIZE = (uint64)1 << page_bits;
    SETS_ARRAY_SIZE  = (uint64)1 << (addr_size - page_bits - offset_bits);
    
    std::cout << "BYTES_ARRAY_SIZE = " << BYTES_ARRAY_SIZE << std::endl;
    std::cout << "PAGES_ARRAY_SIZE = " << PAGES_ARRAY_SIZE << std::endl;
    std::cout << "SETS_ARRAY_SIZE = "  << SETS_ARRAY_SIZE  << std::endl;
    
    //assert (0);

    sets_array = new Set* [SETS_ARRAY_SIZE];

    for (int i = 0; i < SETS_ARRAY_SIZE; i++)
    {
        //printf ("i = %d\n", i);
        sets_array [i] = NULL;
    }

    ElfSection::getAllElfSections(executable_file_name, sections_array);
    
    for (int i = 0; i < sections_array.size (); i++)
    {
        printf ("section is %s\nsize is %ld\n", sections_array [i].name, sections_array [i].size);
    
        //int stop = sections_array [i].size - sections_array [i].size % sizeof (uint64);
        //printf ("        stop = %d\n", stop);
    
        for (int j = 0; j < sections_array [i].size; j += 4)
        {
            //printf ("\n============== j = %d\n", j);
            
            uint8  source_values_array [sizeof (uint64)];
            uint64 dest_value                           ;
            
            memcpy (source_values_array + 4, sections_array [i].content + j, 4);
            
            //printf ("addr = %x\n", sections_array [i].start_addr + j);
            
            /*printf ("source_values_array = ");
            for (int k = 0; k < 8; k++)
                printf ("%0.2x", source_values_array [k]);
        
            printf ("\n");*/

            //if (j < stop) 
            //{
                chars_array_to_uint64 (source_values_array, &dest_value);
                
                //printf ("dest_value = ");
                
                //printf ("%d: uint64_value = %x\n", __LINE__, uint64_value);
                //allocate_new_memory (sections_array [i].start_addr + j);
                
                write (dest_value, sections_array [i].start_addr + j);// + 4 * j);
            //}
            /*else          
            {
                printf ("        else...\n");
            
                chars_array_to_uint64 (source_values_array, &dest_value, sections_array [i].size % sizeof (uint64));
                write ( dest_value, sections_array [i].start_addr + j, sections_array [i].size % sizeof (uint64));
            }*/
            
            uint64 read_value = read (sections_array [i].start_addr + j);
            
            //printf ("read ...\n");
            
            //cout << strByWord (sections_array [i].start_addr + j) << endl; 
            /*for (int n = 0; n < 8; n++)
                printf ("%0.2x",((uint64_type *)(&read_value))->bytes [n]);
            printf ("\n");*/
        }
    }
    
    //cout << endl << endl << endl << "read is " << strByWord (0x4100c0) << endl;
    // put your code here
}

FuncMemory::~FuncMemory()
{
    /*for (int i = 0; i < SETS_ARRAY_SIZE; i++)
    {
        if (!sets_array [i]) continue;
        
        delete sets_array [i];
        
        printf ("pages deleted\n");
    }   
    delete [] sets_array;*/
    // put your code here
}

uint64 FuncMemory::startPC() const
{
    printf ("strat PC ()\n");

    for (int i = 0; i < sections_array.size (); i ++)
    {
        if (!strcmp (sections_array [i].name, ".text")) return sections_array [i].start_addr;
    } 
    
    return NULL;
    // put your code here
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{

    uint64 byte_pos = byte_address (addr);
    uint64 page_pos = page_address (addr);
    uint64 set_pos  = set_address  (addr);
    
    uint8  source_values_array [sizeof (uint64) + 1] = {};
    uint64 dest_value                                ;
            
    assert (BYTES_ARRAY_SIZE - byte_pos    );
    assert (BYTES_ARRAY_SIZE - byte_pos - 1);
    assert (BYTES_ARRAY_SIZE - byte_pos - 2);
    assert (BYTES_ARRAY_SIZE - byte_pos - 3);
    /*
    printf ("addr is  = %x\n", &(sets_array [set_pos]->pages_array [page_pos]->bytes_array [byte_pos]));
        
    cout << "source_val ... ="
         << source_values_array [4]
         << endl;
    
    
    cout << "read : "
         << hex 
         << set_pos 
         << " "
         << hex 
         << page_pos 
         << " "     
         << hex 
         << byte_pos 
         << " " 
         << " addr is 0x" 
         << hex 
         << addr 
         << endl;*/
    
    for (int  i = 0; i < num_of_bytes; i++)
    {
	assert (BYTES_ARRAY_SIZE - byte_pos - i);

        source_values_array [sizeof (uint64) - num_of_bytes + i] = (char)(sets_array  [set_pos ]->
                                      pages_array [page_pos]->
                                      bytes_array [byte_pos + i]);
        //printf ("i = %d\n", i);
    }
    
            
    /*for (int i = 0; i < 8; i++)
        printf ("%.2x", source_values_array [i]);
    printf ("\n\n");*/
            
    chars_array_to_uint64 (source_values_array, &dest_value, sizeof (uint64) - num_of_bytes);

    /*for (int i = 0; i < 8; i++)
         printf ("[%d] = %0.2x\n",
         i,
         source_values_array [i]);
    printf ("\n");*/
    // put your code here
    /*
    cout << "read : "
         << hex 
         << set_pos 
         << " "
         << hex 
         << page_pos 
         << " "     
         << hex 
         << byte_pos 
         << " " 
         << " addr is 0x" 
         << hex 
         << addr 
         << endl;*/

    return dest_value;
}

void FuncMemory::write(uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    printf ("addr = %x\n", addr);
    printf ("value = %x\n", value);
    printf ("num_of_bytes = %d\n", num_of_bytes);

    uint64 byte_pos = byte_address (addr);
    uint64 page_pos = page_address (addr);
    uint64 set_pos  = set_address  (addr);
    
    
    /*cout << "byte" << hex << byte_pos 
         << "page" << hex << page_pos
         << "set " << hex << set_pos << endl;*/
    
    allocate_new_memory (addr);
    

    uint8 dest_values_array [sizeof (uint64)];
    uint64_to_chars_array (value, dest_values_array, sizeof (uint64) - num_of_bytes);
    
    for (int offset = sizeof (uint64) - num_of_bytes; offset < sizeof (uint64); offset++)
    {
        sets_array  [set_pos ]->
        pages_array [page_pos]->
        bytes_array [byte_pos + offset - (sizeof (uint64) - num_of_bytes)] = dest_values_array [offset];
        
        
        if (!(BYTES_ARRAY_SIZE - (byte_pos + offset - (sizeof (uint64) - num_of_bytes))))
        {
            printf ("CRITICAL ERROE!!!!!!!!!!!!!!!!!\n");
        
            assert (BYTES_ARRAY_SIZE - (byte_pos + offset - (sizeof (uint64) - num_of_bytes)));
        }
        /*printf ("[%x][%x][%x] = %0.2x\n", 
                set_pos, page_pos, byte_pos + offset - (sizeof (uint64) - num_of_bytes), 
                sets_array  [set_pos ]->
                pages_array [page_pos]->
                bytes_array [byte_pos + offset - (sizeof (uint64) - num_of_bytes)]);*/
    }
    //printf ("\n");
    /*printf ("addr = %x\n", addr);
    printf ("value = %x\n", value);
    printf ("num_of_bytes = %d\n", num_of_bytes);*/

    
    // put your code here
}

using namespace std;

string FuncMemory::strByWord (uint64 addr) const
{
    // temp stream is used to convert numbers into the output string
    // temp stream is used to convert numbers into the output string
    ostringstream oss;
    
    uint64 source;
    uint8  dest_array [sizeof (uint64)] = {};
    
    source  = read (addr);
    
    uint64_to_chars_array (source, dest_array);
    
    /*for (int  i = 0; i < 8; i++)
        printf ("%.2x", dest_array [i]);
    printf ("\n");*/
    
    // convert each words of 4 bytes into 8 hex digits
    for( size_t i = 4; i < 8; i++)
    {
        oss.width( 2); // because we need 8 hex symbols to print a word (e.g. "ffffffff")
        oss.fill( '0'); // thus, number a44f will be printed as "0000a44f"
        
        oss << hex << (int)dest_array [i];
    }
    
    //cout << "string = " << oss.str () << " addr is 0x" << hex << addr << endl;
    
    return oss.str();
}

string FuncMemory::dump( string indent) const
{
    ostringstream oss;
    
    string null_str = "00000000";
    
    bool skip_was_printed = false;
    
    //cout << "start dump" << endl;
    
    uint64 max_size = 0xffffffff;
    
    ostringstream convert;
    
    for (unsigned set_offset = 0; set_offset < SETS_ARRAY_SIZE; set_offset++)
    {
        //addr |= set_offset;
        //addr <<= 10;
        if (!sets_array [set_offset]) continue;
    
        for (unsigned page_offset = 0; page_offset < PAGES_ARRAY_SIZE; page_offset++)
        {
            if (!sets_array [set_offset]->pages_array [page_offset]) continue;
            
            //addr |= page_offset;
            //addr <<= 10;

            for (unsigned byte_offset = 0; byte_offset < BYTES_ARRAY_SIZE; byte_offset += 4)
            {
                //addr |= byte_offset;

                //cout << "string = " << strByWord (addr) << " addr is 0x" << hex << addr << endl;
                uint64 addr = byte_offset;
                addr += set_offset  << 22;
                addr += page_offset << 12;
                
                //addr <<= 32;
                
                //printf ("addr = ");
                //for (int i = 0; i < 8; i++)
                //    printf ("%x", ((uint64_type*)(&addr))->bytes [i]);
                //printf ("\n");
                
                /*cout << hex 
                     << set_offset 
                     << " "
                     << hex 
                     << page_offset 
                     << " "     
                     << hex 
                     << byte_offset 
                     << " " 
                     << " addr is 0x" 
                     << hex 
                     << addr
                     << " value = "
                     <<  //sets_array  [set_offset ]->
                         //pages_array [page_offset]->
                         //bytes_array  [byte_offset]
                         strByWord (addr)
                     << endl;*/
                     
                
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
                    oss << indent << "    0x" << hex << addr// this->start_addr + offset) 
                        << indent << ":    " << strByWord (addr) << endl;
                    skip_was_printed = false;
                }
            }
        }
    }
    
    
    return oss.str();
    
        //string substr =  str.substr( 2 * offset, // 2 hex digits is need per byte
        //                            sizeof( uint64));
        
        //cout << "addr = " << hex << offset << " str = " << hex << strByWord (offset) << endl;
        
        //printf ("%d\n", (strByWord (offset) == null_str));
        
        

    /*FILE* f_log = fopen ("log.txt", "w");
    
    printf ("start Dump\n");
    
    for (int i = 0; i < SETS_ARRAY_SIZE; i++)
    {
        //fprintf (f_log, "Set [%d] = %x\n", i, sets_array [i]);
        printf ("sets i = %d\n", i);
        
        for (int n = 0; n < PAGES_ARRAY_SIZE; n++)
        {
            //fprintf (f_log, "    page [%d] = %x\n", n, sets_array [i]->pages_array [n]);
            
            for (int j = 0; j < BYTES_ARRAY_SIZE; j++)
            {
                fprintf (f_log, "%x", sets_array [i]->pages_array [n]->bytes_array [j]);
            }
        }
    }
    
    printf ("dumping finished\n");
    
    fclose (f_log);

    // put your code here*/
    return string("ERROR: You need to implement FuncMemory!");
}
