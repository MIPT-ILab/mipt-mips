/**
 * elf_parser.cpp - Implementation of ELF binary perser
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Genereic C
#include <libelf.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <gelf.h>
#include <stdlib.h>
#include <errno.h>

// Generic C++
#include <iostream>
#include <string>
#include <sstream>

// uArchSim modules
#include <elf_parser.h>

using namespace std;

ElfSection::ElfSection( const char* elf_file_name, const char* section_name)
{
    // open the binary file, we have to use C-style open,
    // because it is required by elf_begin function
    int file_descr = open( elf_file_name, O_RDONLY); 
    if ( file_descr < 0)
    {
        cerr << "Error opening file " << elf_file_name << ": "
             << strerror( errno) << endl;
        abort();
    }

    // set ELF library operating version
    if ( elf_version( EV_CURRENT) == EV_NONE)
    {
        cerr << "Could not set ELF library operating version:"
             <<  elf_errmsg( elf_errno()) << endl;
        abort();
    }
   
    // open the file in ELF format 
    Elf* elf = elf_begin( file_descr, ELF_C_READ, NULL);
    if ( !elf)
    {
        cerr << "Could not open file " << elf_file_name
             << " as ELF file: "
             <<  elf_errmsg( elf_errno()) << endl;
        abort();
    }
    
    // set the name of the sections
    this->name = new char[ strlen( section_name) + 1];
    strcpy( this->name, section_name);
    
    // set the size, start address and offset
    uint64 offset = NO_VAL64;
    this->extractSectionParams( elf, section_name,
        offset, this->size, this->start_address);

    // allocate place for the content
    this->content = new uint8[ this->size + 1];
    
    lseek( file_descr, offset, SEEK_SET);
    FILE *file = fdopen( file_descr, "r");
    if ( !file )
    {
        cerr << "Error opening file " << elf_file_name << ": "
             << strerror(errno) << endl;
        abort();
    }
    
    // fill the content by the section data
    fread( this->content, sizeof( uint8), this->size, file);
    
    // close all used files
    fclose( file);
    elf_end( elf);
    close( file_descr);
}

ElfSection::~ElfSection()
{
    delete [] this->name;
    delete [] this->content;
}

void ElfSection::extractSectionParams( Elf* elf, const char* section_name,
                                       uint64& offset, uint64& size,
                                       uint64& start_addr)
{
    size_t shstrndx;
    elf_getshdrstrndx( elf, &shstrndx);

    // look through all sections and try to find the desired one
    Elf_Scn *section = NULL;
    while ( ( section = elf_nextscn( elf, section)) != NULL)
    {
        GElf_Shdr shdr;
        gelf_getshdr( section, &shdr);
        char* name = elf_strptr( elf, shstrndx, shdr.sh_name);
        
        // if a section with the decired name
        // then set its start address, size and offset
        // and return back from the function.
        if ( !strcmp( name, section_name))
        {
            offset = ( uint64)shdr.sh_offset;
            size = ( uint64)shdr.sh_size;
            start_addr = ( uint64)shdr.sh_addr;
            
            return;
        }
    }
    
    cerr << "Could not find section " << section_name
         << " in ELF file!" << endl;
    abort();
}

string ElfSection::Dump()
{
    ostringstream oss;

    oss << "Dump ELF section \"" << this->name << "\"" << endl
        << "  size = " << this->size << " Bytes" << endl
        << "  start_addr = 0x" << hex << this->start_address << dec << endl
        << "  Content:" << endl;
     
    string str = this->strByBytes();

    // split the contents into words of 4 bytes
    for ( size_t offset = 0; offset < this->size; offset += sizeof( uint32))
    {
        oss << "    0x" << hex << ( this->start_address + offset) 
            << ":    " << str.substr( 2 * offset, // because 2 hex digits is need per byte
                                    sizeof( uint64))
	        << endl;
    }

    return oss.str();
}

string ElfSection::strByBytes()
{
    // temp stream is used to convert numbers into the output string
    ostringstream oss;
    oss << hex;
	
    // convert each byte into 2 hex digits 
    for( size_t i = 0; i < this->size; ++i)
    {
        oss.width( 2); // because we need two hex symbols to print a byte (e.g. "ff")
        oss.fill( '0'); // thus, number 8 will be printed as "08"
        
        // print a value of 
        oss << (uint16) *( this->content + i); // need converting to uint16
                                               // to be not preinted as an alphabet symbol	
    }
    
    return oss.str();
}

string ElfSection::strByWords()
{
    // temp stream is used to convert numbers into the output string
    ostringstream oss;
    oss << hex;

    // convert each words of 4 bytes into 8 hex digits
    for( size_t i = 0; i < this->size; ++i)
    {
        oss.width( 8); // because we need 8 hex symbols to print a word (e.g. "ffffffff")
        oss.fill( '0'); // thus, number a44f will be printed as "0000a44f"
        
        oss << *( ( uint32*)this->content + i);
    }
    
    return oss.str();
}

