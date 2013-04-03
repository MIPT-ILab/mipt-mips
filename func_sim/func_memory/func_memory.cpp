/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <cassert>
#include <cstdlib>
#include <cstring> // for strcmp function 

// Generic C++
#include <iostream>
#include <string> 
#include <sstream>

// uArchSim modules
#include <func_memory.h>

FuncMemory::FuncMemory( const char* executable_file_name)
           : start_PC( NO_VAL64) 
{
    const char* const section_names[] = {".data", ".text"};
    this->Init(executable_file_name, section_names, 2);
}

void FuncMemory::Init( const char* executable_file_name,
                       const char* const elf_sections_names[],
                       unsigned short num_of_elf_sections)
{
    assert( executable_file_name != NULL);
    assert( elf_sections_names != NULL);
    assert( num_of_elf_sections > 0); 

    for ( unsigned short i = 0; i < num_of_elf_sections; ++i)
    {
        // create the section
        ElfSection* sect = new ElfSection( executable_file_name,
                                           elf_sections_names[i]);
        
        // Insert the pointer to the section into the map
        // where the section start address is used as a key.
        pair<Iter, bool /*is not duplicated*/> status =  
            this->sections.insert( make_pair( sect->startAddr(), sect));
        
        // It is assumed that the first instruction in the ".text" section
        // is the first instruction of the program. 
        if ( strcmp( elf_sections_names[ i], ".text") == 0)
        {
            this->start_PC = sect->startAddr();
        }
            
        if ( status.second == false) // a section with the same start addr
        {                            // has already been added
            cerr << "ERROR: section \"" <<  elf_sections_names[i] << "\" exists more"
                 << endl << "then two time in the list of ELF sections passed into"
                 << endl << "the constructor of a functional memory object" << endl;
            exit( EXIT_FAILURE); 
        }
    }

    // Check that the start PC was initialized.
    if ( this->start_PC == NO_VAL64)
    {
        cerr << "ERROR: start PC was not set. It means that section \".text\""
             << endl << "was not provide to the constructor of the functional memory"
             << endl;
        exit( EXIT_FAILURE); 
    }  
}

FuncMemory::~FuncMemory()
{
    // free memory that was allocate in the constructor for ELF sections
    for ( Iter it = this->sections.begin(); it != this->sections.end(); ++it)
        delete (*it).second; // delete an ELF section object through a pointer to it
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    assert( num_of_bytes > 0);
    // check that the requested data can be returned via uint64
    assert( num_of_bytes <= sizeof( uint64));
    
    // Recieve an iterator of the section which start address 
    // is greater than the read address
    ConstIter nextSectIt = this->sections.upper_bound( addr);
    
    // Check that this section is not the first,
    // Otherwise that the read address refer to the location
    // outside the memory.
    assert( nextSectIt != this->sections.begin());

    // The data has be in the previous section
    const ElfSection* sect = (*( --nextSectIt)).second;	
   
    assert( sect->isInside( addr, num_of_bytes));

    return sect->read( addr, num_of_bytes); 
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    assert( num_of_bytes > 0);
    // check that the requested data can be returned via uint64
    assert( num_of_bytes <= sizeof( uint64));
    
    // Recieve an iterator of the section which start address 
    // is greater than the read address
    Iter nextSectIt = this->sections.upper_bound( addr);
    
    // Check that this section is not the first,
    // Otherwise that the read address refer to the location
    // outside the memory.
    assert( nextSectIt != this->sections.begin());

    // The data has be in the previous section
    ElfSection* sect = (*( --nextSectIt)).second;	
   
    assert( sect->isInside( addr, num_of_bytes));

    sect->write( value, addr, num_of_bytes); 
}

uint64 FuncMemory::startPC() const
{
    return this->start_PC;
}

string FuncMemory::dump( string indent) const
{
    ostringstream oss;
    
    oss << "Dump all the ELF sections of the functional memory" << endl;
    
    // dumpt content of each section with an indent of two blanks
    for ( ConstIter it = this->sections.begin(); it != this->sections.end(); ++it)
        oss << (( *it).second)->dump( "  ");
    
    return oss.str();
}

