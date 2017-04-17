/**
 * elf_parser.cpp - Implementation of ELF binary perser
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2013 MIPT-MIPS iLab project
 */

// Genereic C
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <gelf.h>
#include <cstdlib>
#include <cerrno>
#include <cassert>

// Generic C++
#include <iostream>
#include <string>
#include <sstream>

#include <unistd.h>

// uArchSim modules
#include "elf_parser.h"

ElfSection::ElfSection()
{
    std::cerr << "ERROR: the constructor without parameters "
              << "of ElfSection class is prohobited" << std::endl;
    exit( EXIT_FAILURE);
}

ElfSection::ElfSection( const ElfSection& that)
    : name( that.name), size( that.size), start_addr( that.start_addr)
{
    this->content = new uint8[size];
    std::memcpy(this->content, that.content, this->size);
}

ElfSection::ElfSection( const char* name_c, uint64 start_addr,
                        uint64 size, const uint8* content)
    : name(name_c)
{
    this->size = size;
    this->start_addr = start_addr;

    this->content = new uint8[ this->size + sizeof( uint64)];
    std::memcpy( this->content, content, size);
}

void ElfSection::getAllElfSections( const char* elf_file_name,
                                    std::vector<ElfSection>& sections_array /*is used as output*/)
{
    // open the binary file, we have to use C-style open,
    // because it is required by elf_begin function
    int file_descr = open( elf_file_name, O_RDONLY);
    if ( file_descr < 0)
    {
        std::cerr << "ERROR: Could not open file " << elf_file_name << ": "
                  << std::strerror( errno) << std::endl;
        std::exit( EXIT_FAILURE);
    }

    // set ELF library operating version
    if ( elf_version( EV_CURRENT) == EV_NONE)
    {
        std::cerr << "ERROR: Could not set ELF library operating version:"
                  <<  elf_errmsg( elf_errno()) << std::endl;
        std::exit( EXIT_FAILURE);
    }

    // open the file in ELF format
    Elf* elf = elf_begin( file_descr, ELF_C_READ, NULL);
    if ( !elf)
    {
        std::cerr << "ERROR: Could not open file " << elf_file_name
                  << " as ELF file: "
                  <<  elf_errmsg( elf_errno()) << std::endl;
	std::exit( EXIT_FAILURE);
    }

    size_t shstrndx;
    elf_getshdrstrndx( elf, &shstrndx);

    Elf_Scn *section = NULL;
    while ( (section = elf_nextscn( elf, section)) != NULL)
    {
        GElf_Shdr shdr;
        gelf_getshdr( section, &shdr);

        char* name = elf_strptr( elf, shstrndx, shdr.sh_name);
        uint64 start_addr = static_cast<uint64>( shdr.sh_addr);

        if ( start_addr == 0)
            continue;

        uint64 size = static_cast<uint64>( shdr.sh_size);
        uint64 offset = static_cast<uint64>( shdr.sh_offset);
	    uint8* content = new uint8[ size];

        lseek( file_descr, offset, SEEK_SET);
        FILE *file = fdopen( file_descr, "r");
        if ( !file )
        {
            std::cerr << "ERROR: Could not open file " << elf_file_name << ": "
                 << std::strerror(errno) << std::endl;
            std::exit( EXIT_FAILURE);
        }

        // fill the content by the section data
        auto result = std::fread( content, sizeof( uint8), size, file);
        (void)(result);
        ElfSection section( name, start_addr, size, content);
	    sections_array.push_back( ElfSection( name, start_addr, size, content));
        delete [] content;
    }

    // close all used files
    elf_end( elf);
    close( file_descr);
}

ElfSection::~ElfSection()
{
    delete [] this->content;
}

std::string ElfSection::dump( std::string indent) const
{
    std::ostringstream oss;

    oss << indent << "Dump ELF section \"" << this->name << "\"" << std::endl
        << indent << "  size = " << this->size << " Bytes" << std::endl
        << indent << "  start_addr = 0x"
        << std::hex << this->start_addr << std::dec << std::endl
        << indent << "  Content:" << std::endl;

    std::string str = this->strByBytes();

    // split the contents into words of 4 bytes
    bool skip_was_printed = false;
    for ( size_t offset = 0; offset < this->size; offset += sizeof( uint32))
    {
        std::string substr =  str.substr( 2 * offset, // 2 hex digits is need per byte
                                     sizeof( uint64));

        if ( substr.compare( "00000000") == 0)
        {
            if ( !skip_was_printed)
            {
                oss << indent << "  ....  " << std::endl;
                skip_was_printed = true;
            }
        }
        else
        {
            oss << indent << "    0x"
                << std::hex << ( this->start_addr + offset)
                << indent << ":    " << substr << std::endl;
            skip_was_printed = false;
        }
    }

    return oss.str();
}

std::string ElfSection::strByBytes() const
{
    // temp stream is used to convert numbers into the output string
    std::ostringstream oss;
    oss << std::hex;

    // convert each byte into 2 hex digits
    for( size_t i = 0; i < this->size; ++i)
    {
        oss.width( 2); // because we need two hex symbols to print a byte (e.g. "ff")
        oss.fill( '0'); // thus, number 8 will be printed as "08"
        uint16 value = *( this->content + i); // need converting to uint16
                                              // to be not preinted as an alphabet symbol

        // print a value of
        oss << value;
    }

    return oss.str();
}

std::string ElfSection::strByWords() const
{
    // temp stream is used to convert numbers into the output string
    std::ostringstream oss;
    oss << std::hex;

    // convert each words of 4 bytes into 8 hex digits
    for( size_t i = 0; i < this->size/sizeof( uint32); ++i)
    {
        oss.width( 8); // because we need 8 hex symbols to print a word (e.g. "ffffffff")
        oss.fill( '0'); // thus, number a44f will be printed as "0000a44f"

        oss << *( reinterpret_cast<uint32*>(this->content) + i);
    }

    return oss.str();
}

