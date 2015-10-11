/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Roman Zavodskikh <roman.zavodskikh@phystech.edu>
 * Copyright 2015 uArchSim iLab project
 */

// Generic C
#include <string.h>
#include <math.h>
#include <stdlib.h>

// Generic C++
#include <iostream>
#include <string>
#include <sstream>

// uArchSim modules
#include <func_memory.h>
#include <elf_parser.h>

const int POISON = -1;

void FuncMemory::increase_offset_priv ( unsigned& offset, unsigned& num_of_page, unsigned& num_of_set, unsigned inc_by) const
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
                clog << "The END of the memory REACHED!!!" << endl;
                exit( -1);
            }
        }
    }
}

void FuncMemory::decrease_offset_priv ( unsigned& offset, unsigned& num_of_page, unsigned& num_of_set, unsigned dec_by) const
{
    offset-=dec_by;
    if ( offset < 0)
    {
        offset+=max_offset_priv;
        --num_of_page;
        if ( num_of_page < 0)
        {
            num_of_page+=num_of_pages_priv;
            --num_of_set;
            if( num_of_set < 0)
            {
                clog << "The BEGINNING of the memory REACHED!!!" << endl;
                exit( -1);
            }
        }
    }
}
FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
    :addr_size_priv( addr_size), page_num_size_priv( page_bits), offset_size_priv( offset_bits)
{
    this->num_of_sets_priv =  floor( pow( 2, (this->addr_size_priv - this->page_num_size_priv - this->offset_size_priv)));
    this->num_of_pages_priv = floor( pow( 2, this->page_num_size_priv));
    this->max_offset_priv = floor( pow( 2, this->offset_size_priv));

    this->exe_file_name_priv = new char[ strlen( executable_file_name) + 1];
    strcpy( this->exe_file_name_priv, executable_file_name);

    this->sets_array_priv = new uint8**[ this->num_of_sets_priv];
    clog << "Initalized array of " << num_of_sets_priv << " sets." << endl;
    memset( this->sets_array_priv, 0, this->num_of_sets_priv * sizeof( *( this->sets_array_priv)));

    vector<ElfSection> sections_array;
    ElfSection::getAllElfSections( this->exe_file_name_priv, sections_array);
    
    for ( unsigned i = 0; i < sections_array.size( ); ++i)
    {
        uint64 cur_addr = sections_array[ i].start_addr;
        if ( strcmp( sections_array[ i].name, ".text") == 0)
        {
            this->start_pc_adress_priv = cur_addr;
            clog << "start_pc_adress finded and written: " << this->start_pc_adress_priv << endl;
        }

        for ( size_t offset = 0; offset < sections_array[ i].size; offset++)
        {
            this->write( sections_array[ i].content[ offset], cur_addr, 1); 
            cur_addr++;
        }
    }
}

FuncMemory::~FuncMemory()
{
    for( unsigned i = 0; i < num_of_sets_priv; ++i)
    {
        if( this->sets_array_priv[ i] != NULL)
        {
            for (unsigned j = 0; j < num_of_pages_priv; ++j)
            {
                if( this->sets_array_priv[ i][ j] != NULL)
                {
                    delete [] this->sets_array_priv[ i][ j];
                }
            }
            delete [] this->sets_array_priv[ i];
        }
    }
    delete [] sets_array_priv;

    delete [] this->exe_file_name_priv;
    this->addr_size_priv=this->page_num_size_priv=this->offset_size_priv=POISON;
    this->num_of_sets_priv=this->num_of_pages_priv=this->max_offset_priv=POISON;
}

uint64 FuncMemory::startPC() const
{
    // put your code here
    return start_pc_adress_priv;
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    // put your code here
    using namespace std;
    clog << "FUNCTION FuncMemory::read" << endl;
    if( num_of_bytes > 8)
    {
        cerr << "Cannot read more than eight (" << num_of_bytes << ") bytes" << endl;
        return 0;
    }
    clog << "Passed num_of_bytes > 8 checking" << endl;

    clog << "addr==" << addr << ", num_of_bytes==" << num_of_bytes << endl;
    clog << "for this: " << endl << "---addr_size==" << addr_size_priv << endl << "---page_num_size==" << page_num_size_priv << endl;
    clog << "---offset_size_priv==" << offset_size_priv << endl;
    unsigned set_size = addr_size_priv - page_num_size_priv - offset_size_priv;
    unsigned num_of_set = ( addr & ( ( ( -1) >> addr_size_priv - set_size) << addr_size_priv - set_size )) >> addr_size_priv - set_size;
    unsigned num_of_page = ( addr >> offset_size_priv) & ( ( 1 << ( page_num_size_priv)) - 1);
    unsigned offset = addr & ( ( 1 << offset_size_priv) - 1);
    clog << "Counted num_of_set (" << num_of_set <<") num_of_page (" << num_of_page;
    clog << ") and offset (" << offset << ")" << endl;
    if ( this->sets_array_priv[ num_of_set] == NULL)

    {
        cerr << "Tryed to read from unitialized memory (set)" << endl;
        return 0;
    }

    if ( this->sets_array_priv[ num_of_set][ num_of_page] == NULL)
    {
        cerr << "Tryed to read from unitialized memory (page)" << endl;
        return 0;
    }

    uint64 rtr_val = 0;
    for ( unsigned i = 0; i < num_of_bytes; ++i)
    {
        rtr_val <<= 8;
        clog << "Read rtr_val(before adding)==" << rtr_val << endl;
        if( this->sets_array_priv[ num_of_set] != NULL && this->sets_array_priv[ num_of_set][ num_of_page] != NULL)
        {
            rtr_val += this->sets_array_priv[ num_of_set][ num_of_page][ offset];
        }
        clog << "Read rtr_val(after adding)==" << rtr_val << endl;
        increase_offset_priv( offset, num_of_page, num_of_set, 1);
    }
    clog << "END_OF_FUNCTION FuncMemory::read. rtr_val == " << rtr_val << endl;
    return rtr_val;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    // put your code here
    using namespace std;
    clog << "FUNCTION FuncMemory::write" << endl;
    if( num_of_bytes > 8)
    {
        cerr << "Cannot write more than eight (" << num_of_bytes << ") bytes" << endl;
        return ;
    }
    clog << "Passed checking num_of_bytes > 8" << endl;

    clog << "value==" << value << ", addr==" << addr << ", num_of_bytes==" << num_of_bytes << endl;
    clog << "for this: " << endl << "---addr_size==" << addr_size_priv << endl << "---page_num_size==" << page_num_size_priv << endl;
    clog << "---offset_size_priv==" << offset_size_priv << endl;
    unsigned set_size = addr_size_priv - page_num_size_priv - offset_size_priv;
    unsigned num_of_set = ( addr & ( ( ( -1) >> addr_size_priv - set_size) << addr_size_priv - set_size )) >> addr_size_priv - set_size;
    unsigned num_of_page = ( addr >> offset_size_priv) & ( ( 1 << ( page_num_size_priv)) - 1);
    unsigned offset = addr & ( ( 1 << offset_size_priv) - 1);
    clog << "Counted num_of_set (" << num_of_set <<") num_of_page (" << num_of_page;
    clog << ") and offset (" << offset << ")" << endl;

    for ( unsigned i = 0; i < num_of_bytes; ++i)
    {   
        if ( this->sets_array_priv[ num_of_set] == NULL)
        {
            clog << "Initializing set number " << num_of_set << " with " << num_of_pages_priv << " pages." << endl;
            this->sets_array_priv[ num_of_set] = new uint8*[ num_of_pages_priv];
            memset( this->sets_array_priv[ num_of_set], 0, num_of_pages_priv * sizeof( *( this->sets_array_priv[ num_of_set])));
            clog << "Initialized set number " << num_of_set << " with " << num_of_pages_priv << " pages." << endl;
        }

        if ( this->sets_array_priv[ num_of_set][ num_of_page] == NULL)
        {
            clog << "Initializing page number " << num_of_page << " in " << num_of_set << " set with " << max_offset_priv << " bytes." << endl;
            this->sets_array_priv[ num_of_set][ num_of_page] = new uint8[ max_offset_priv];
            memset( this->sets_array_priv[ num_of_set][ num_of_page], 0, max_offset_priv * sizeof( *( this->sets_array_priv[ num_of_set][ num_of_page])));
            clog << "Initialized page number " << num_of_page << " in " << num_of_set << " set with " << max_offset_priv << " bytes." << endl;
        }

        uint8 val_to_write = ( value & ( 0xFF << ( num_of_bytes-i-1)*8)) >> ( ( num_of_bytes-i-1)*8);
        clog << "value==" <<value<< ";val_to_write==" << (unsigned)val_to_write;
        clog << ";value&0xFF<<*==" << ( (value & ( 0xFF << ( num_of_bytes-i-1)*8)) >> (num_of_bytes-i-1 ))  << endl;
        clog << "----num_of_set==" << num_of_set << ", num_of_page==" << num_of_page << ", offset==" << offset << endl;
        this->sets_array_priv[ num_of_set][ num_of_page][ offset] = val_to_write;
        increase_offset_priv( offset, num_of_page, num_of_set, 1);
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
        if ( this->sets_array_priv[ set_num] != NULL) 
        {
            for( unsigned page_num = 0; page_num < num_of_pages_priv; ++page_num)
            {
                if ( this->sets_array_priv[ set_num][ page_num] != NULL)
                {
                    for ( unsigned offset = 0; offset < max_offset_priv; offset+=sizeof( uint32))
                    {
                        uint64 addr = ( ( set_num << page_num_size_priv+offset_size_priv) + ( page_num << offset_size_priv) + ( offset));
                        uint32 val = this->read( addr, sizeof( uint32));
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
    oss << "ERROR: You need to implement FuncMemory!\nTHIS IS VERY STUPID DUMP!!!\nDON\'T USE IT IN THE PRODUCTION!!!";
    return oss.str();
}
