/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Roman Zavodskikh <roman.zavodskikh@phystech.edu>
 * Copyright 2015 uArchSim iLab project
 */

// Generic C

// Generic C++
#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>

// uArchSim modules
#include <func_memory.h>
#include <elf_parser.h>

const int POISON = -1;

uint64 FuncMemory::get_num_of_set_priv( uint64 addr) const
{
    return addr >> ( addr_size_priv - set_num_size_priv);
}

uint64 FuncMemory::get_num_of_page_priv( uint64 addr) const
{
    return ( addr >> offset_size_priv) & ( ( 1 << page_num_size_priv) - 1);
}

uint64 FuncMemory::get_offset_priv( uint64 addr) const
{
    return addr & ( ( 1 << offset_size_priv) - 1);
}

void FuncMemory::increase_offset_priv ( uint64& offset,
            uint64& num_of_page,
            uint64& num_of_set, unsigned inc_by) const
{
    offset+=inc_by;
    if ( offset >= max_offset_priv)
    {
        offset-=max_offset_priv;
        ++num_of_page;
        if ( num_of_page >= num_of_pages_priv)
        {
            num_of_page-=num_of_pages_priv;
            ++num_of_set;
            if( num_of_set >= num_of_sets_priv)
            {
                cerr << "The END of the memory REACHED!!!" << endl;
                terminate();
            }
        }
    }
}

void FuncMemory::decrease_offset_priv ( uint64& offset,
        uint64& num_of_page,
        uint64& num_of_set, unsigned dec_by) const
{
    offset-=dec_by;
    if ( offset + dec_by < dec_by)
    {
        offset+=max_offset_priv;
        --num_of_page;
        if ( num_of_page + 1 < 1)
        {
            num_of_page+=num_of_pages_priv;
            --num_of_set;
            if( num_of_set + 1 < 1)
            {
                cerr << "The BEGINNING of the memory REACHED!!!" << endl;
                terminate();
            }
        }
    }
}
FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
    :addr_size_priv( addr_size),
     page_num_size_priv( page_bits),
     offset_size_priv( offset_bits),
     exe_file_name_priv( executable_file_name)
{
    vector<ElfSection> sections_array;
    ElfSection::getAllElfSections( exe_file_name_priv.c_str(), sections_array);
    
    set_num_size_priv = addr_size_priv - page_num_size_priv -
        offset_size_priv;

    num_of_sets_priv = (uint64)1 << set_num_size_priv; 
    num_of_pages_priv = (uint64)1 << page_num_size_priv;
    max_offset_priv = (uint64)1 << offset_size_priv;

    clog << "The max values: " << "offset==" << max_offset_priv 
         << ", page==" << num_of_pages_priv
         << ", set==" << num_of_sets_priv << endl;

    sets_array_priv = new uint8**[ num_of_sets_priv];
    clog << "Initalized array of " << num_of_sets_priv << " sets." << endl;
    #if 0
    memset( sets_array_priv, 0,
        num_of_sets_priv * sizeof( *( sets_array_priv)));
    #endif
    fill( sets_array_priv, sets_array_priv + num_of_sets_priv,
        (unsigned char**)'\0');

    for ( unsigned i = 0; i < sections_array.size( ); ++i)
    {
        uint64 cur_addr = sections_array[ i].start_addr;
        if ( string( sections_array[ i].name) ==
             string( ".text") )
        {
            start_pc_adress_priv = cur_addr;
            clog << "start_pc_adress finded and written: "
                 << start_pc_adress_priv << endl;
        }

        for ( size_t offset = 0; offset < sections_array[ i].size; offset++)
        {
            write( sections_array[ i].content[ offset], cur_addr, 1); 
            cur_addr++;
        }
    }
}

FuncMemory::~FuncMemory()
{
    for( unsigned i = 0; i < num_of_sets_priv; ++i)
    {
        if( sets_array_priv[ i] != NULL)
        {
            for (unsigned j = 0; j < num_of_pages_priv; ++j) {
                if( sets_array_priv[ i][ j] != NULL)
                {
                    delete [] sets_array_priv[ i][ j];
                }
            }
            delete [] sets_array_priv[ i];
        }
    }
    delete [] sets_array_priv;

    addr_size_priv=page_num_size_priv=offset_size_priv=POISON;
    num_of_sets_priv=num_of_pages_priv=max_offset_priv=POISON;
}

uint64 FuncMemory::startPC() const
{
    // put your code here
    return start_pc_adress_priv;
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    // put your code here
    clog << "FUNCTION FuncMemory::read" << endl;
    clog << "addr==" << addr << ", num_of_bytes==" << num_of_bytes << endl;

    if( num_of_bytes > 8)
    {
        cerr << "Cannot read more than eight ("
             << num_of_bytes << ") bytes" << endl;
        terminate();
    }
    else if( num_of_bytes == 0)
    {
        cerr << "Trying to read 0 bytes" << endl;
        terminate();
    }
    clog << "Passed num_of_bytes > 8 checking" << endl;

    uint64 num_of_set = get_num_of_set_priv( addr);
    uint64 num_of_page = get_num_of_page_priv( addr);
    uint64 offset = get_offset_priv( addr);

    clog << "Counted num_of_set (" << num_of_set 
         << ") num_of_page (" << num_of_page
         << ") and offset (" << offset << ")" << endl;

    if ( sets_array_priv[ num_of_set] == NULL)
    {
        cerr << "Tryed to read from unitialized memory (set)" << endl;
        terminate();
    }

    if ( sets_array_priv[ num_of_set][ num_of_page] == NULL)
    {
        cerr << "Tryed to read from unitialized memory (page)" << endl;
        terminate();
    }

    uint64 rtr_val = 0;
    increase_offset_priv( offset, num_of_page, num_of_set, num_of_bytes);
    for ( unsigned i = 0; i < num_of_bytes; ++i)
    {
        rtr_val <<= 8;
        if( sets_array_priv[ num_of_set] != NULL &&
            sets_array_priv[ num_of_set][ num_of_page] != NULL)
        {
            rtr_val += sets_array_priv[ num_of_set][ num_of_page][ offset];
        }
        decrease_offset_priv( offset, num_of_page, num_of_set, 1);
    }
    clog << "END_OF_FUNCTION FuncMemory::read. rtr_val == " << rtr_val << endl;
    return rtr_val;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    // put your code here
    clog << "FUNCTION FuncMemory::write" << endl;
    clog << "value==" << value << ", addr==" << addr 
         << ", num_of_bytes==" << num_of_bytes << endl;
    if( num_of_bytes > 8)
    {
        cerr << "Cannot write more than eight (" 
             << num_of_bytes << ") bytes" << endl;
        terminate();
    }
    else if( num_of_bytes == 0)
    {
        cerr << "Trying to write 0 bytes" << endl;
        terminate();
    }
    clog << "Passed checking num_of_bytes > 8" << endl;

    uint64 num_of_set = get_num_of_set_priv( addr);
    uint64 num_of_page = get_num_of_page_priv( addr);
    uint64 offset = get_offset_priv( addr);

    clog << "Counted num_of_set (" << num_of_set 
         << ") num_of_page (" << num_of_page;
    clog << ") and offset (" << offset << ")" << endl;

    increase_offset_priv( offset, num_of_page, num_of_set, num_of_bytes);
    for ( unsigned i = 0; i < num_of_bytes; ++i)
    {   
        if ( sets_array_priv[ num_of_set] == NULL)
        {
            clog << "Initializing set number " << num_of_set << " with " 
                 << num_of_pages_priv << " pages." << endl;
            sets_array_priv[ num_of_set] = new uint8*[ num_of_pages_priv];
            #if 0
            memset( sets_array_priv[ num_of_set], 0,
                num_of_pages_priv * sizeof( *( sets_array_priv[ num_of_set])));
            #endif
            fill( sets_array_priv[ num_of_set],
                sets_array_priv[ num_of_set] + num_of_pages_priv,
                (unsigned char*)'\0');
        }

        if ( sets_array_priv[ num_of_set][ num_of_page] == NULL)
        {
            clog << "Initializing page number " << num_of_page << " in " 
                 << num_of_set << " set with " 
                 << max_offset_priv << " bytes." << endl;
            sets_array_priv[ num_of_set][ num_of_page] =
                new uint8[ max_offset_priv];
            #if 0
            memset( sets_array_priv[ num_of_set][ num_of_page], 0,
                max_offset_priv *
                    sizeof( *( sets_array_priv[ num_of_set][ num_of_page])));
            #endif
        }

        uint64 val_to_write = ( value & ( 0xFF << ( num_of_bytes-i-1)*8))
            >> ( ( num_of_bytes-i-1)*8);
        clog << "value==" << value<< ";val_to_write==" << val_to_write;
        clog << "----num_of_set==" << num_of_set 
             << ", num_of_page==" << num_of_page 
             << ", offset==" << offset << endl;
        sets_array_priv[ num_of_set][ num_of_page][ offset] = val_to_write;
        decrease_offset_priv( offset, num_of_page, num_of_set, 1);
    }
    clog << "END_OF_FUNCTION FucnMemory::write" << endl;
}

string FuncMemory::dump( string indent) const
{
    // put your code here
    ostringstream oss;

    oss << indent << "Dump of FuncMemory" << endl
        << indent << "Content" << endl;

    oss << hex;
    bool skip_was_printed = false;
    for( unsigned set_num = 0; set_num < num_of_sets_priv; ++set_num)
    {
        if ( sets_array_priv[ set_num] != NULL) 
        {
            for( unsigned page_num = 0;
                page_num < num_of_pages_priv; ++page_num)
            {
                if ( sets_array_priv[ set_num][ page_num] != NULL)
                {
                    for ( unsigned offset = 0; offset < max_offset_priv;
                        offset+=sizeof( uint32))
                    {
                        uint64 addr = offset;
                        addr += set_num << page_num_size_priv+offset_size_priv;
                        addr += page_num << offset_size_priv;

                        uint32 val = read( addr, sizeof( uint32));
                        if ( val ==0)
                        {
                            if ( !skip_was_printed)
                            {
                                oss << indent << " ..... " << endl;
                                skip_was_printed = true;
                            }
                        }
                        else
                        {
                            ostringstream addr_str;
                            addr_str << hex;
                            addr_str.width( 8);
                            addr_str.fill( '0');
                            addr_str << addr;

                            ostringstream val_str;
                            val_str << hex;
                            val_str.width( 8);
                            val_str.fill( '0');
                            val_str << val; 

                            oss << indent << "Addr 0x"
                                << addr_str.str()
                                << " : "
                                << val_str.str() << endl;

                            skip_was_printed = false;
                        }
                    }
                }
            }
        }
    }
    return oss.str();
}
