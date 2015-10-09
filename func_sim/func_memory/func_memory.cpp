/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Roman Zavodskikh <roman.zavodskikh@phystech.edu>
 * Copyright 2015 uArchSim iLab project
 */

// Generic C
#include <string.h>
#include <math.h>

// Generic C++
#include <iostream>
#include <string>
#include <sstream>

// uArchSim modules
#include <func_memory.h>
#include <elf_parser.h>

const int POISON = -1;

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

    vector<ElfSection> sections_array_priv;

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
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    // put your code here
    using namespace std;
    if( num_of_bytes > 8)
    {
        cerr << "Cannot read more than eight (" << num_of_bytes << ") bytes" << endl;
        return 0;
    }

    unsigned set_size = this->addr_size_priv - this->page_num_size_priv - this->offset_size_priv;
    unsigned num_of_set = addr & ( ( -1) >> this->addr_size_priv - set_size);
    if ( this->sets_array_priv[ num_of_set] == NULL)
    {
        cerr << "Tryed to read from unitialized memory (set)" << endl;
        return 0;
    }

    unsigned num_of_page = ( addr >> this->offset_size_priv) & ( ( -1) >> this->addr_size_priv - this->page_num_size_priv);
    if ( this->sets_array_priv[ num_of_set][ num_of_page] == NULL)
    {
        cerr << "Tryed to read from unitialized memory (page)" << endl;
        return 0;
    }

    unsigned offset = addr & ( ( -1) >> this->addr_size_priv - this->offset_size_priv) + ( num_of_bytes);
    uint64 rtr_val = 0;
    for ( unsigned i = 0; i < num_of_bytes; ++i)
    {
        rtr_val <<= 8;
        clog << "Read rtr_val(before adding)==" << rtr_val << endl;
        rtr_val += this->sets_array_priv[ num_of_set][ num_of_page][ offset];
        clog << "Read rtr_val(after adding)==" << rtr_val << endl;
        ++offset;
    }
    return rtr_val;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    // put your code here
    using namespace std;
    if( num_of_bytes > 8)
    {
        cerr << "Cannot write more than eight (" << num_of_bytes << ") bytes" << endl;
        return ;
    }
    clog << "Passed checking num_of_bytes > 8" << endl;

    unsigned set_size = this->addr_size_priv - this->page_num_size_priv - this->offset_size_priv;
    unsigned num_of_set = addr & ( ( -1) >> this->addr_size_priv - set_size);
    if ( this->sets_array_priv[ num_of_set] == NULL)
    {
        clog << "Initializing set number " << num_of_set << " with " << num_of_pages_priv << " pages." << endl;
        this->sets_array_priv[ num_of_set] = new uint8*[ num_of_pages_priv];
        memset( this->sets_array_priv[ num_of_set], 0, num_of_pages_priv * sizeof( *( this->sets_array_priv[ num_of_set])));
        clog << "Initialized set number " << num_of_set << " with " << num_of_pages_priv << " pages." << endl;
    }

    unsigned num_of_page = ( addr >> this->offset_size_priv) & ( ( -1) >> this->addr_size_priv - this->page_num_size_priv);
    clog << "Counted num_of_page: " << num_of_page << endl;
    if ( this->sets_array_priv[ num_of_set][ num_of_page] == NULL)
    {
        clog << "Initializing page number " << num_of_page << " in " << num_of_set << " set with " << max_offset_priv << " bytes." << endl;
        this->sets_array_priv[ num_of_set][ num_of_page] = new uint8[ max_offset_priv];
        memset( this->sets_array_priv[ num_of_set][ num_of_page], 0, max_offset_priv * sizeof( *( this->sets_array_priv[ num_of_set][ num_of_page])));
        clog << "Initialized page number " << num_of_page << " in " << num_of_set << " set with " << max_offset_priv << " bytes." << endl;
    }

    unsigned offset = addr & ( ( -1) >> this->addr_size_priv - this->offset_size_priv);
    for ( unsigned i = 0; i < num_of_bytes; ++i)
    {   
        uint8 val_to_write = ( value & ( 0xFF << ( num_of_bytes-i-1)*8)) >> ( ( num_of_bytes-i-1)*8);
        clog << "value==" <<value<< ";val_to_write==" << (unsigned)val_to_write << ";value&0xFF<<*==" << ( (value & ( 0xFF << ( num_of_bytes-i-1)*8)) >> (num_of_bytes-i-1 ))  << endl;
        this->sets_array_priv[ num_of_set][ num_of_page][ offset] = val_to_write;
        ++offset;
    }
    this->sets_array_priv[ num_of_set][ num_of_page][ offset] = value;
}

string FuncMemory::dump( string indent) const
{
    // put your code here
    return "ERROR: You need to implement FuncMemory!\nTHIS IS VERY STUPID DUMP!!!\nDON\'T USE IT IN THE PRODUCTION!!!";
}
