/*
* This is an open source non-commercial project. Dear PVS-Studio, please check it.
* PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
*/
/**
 * elf_parser.cpp - Implementation of ELF binary perser
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2013 MIPT-MIPS iLab project
 */

// Genereic C
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <cassert>

// Generic C++
#include <iostream>
#include <sstream>
#include <memory>

// LibELF
#include <libelf.h>

// uArchSim modules
#include <infra/macro.h>
#include "elf_parser.h"

ElfSection::ElfSection( const ElfSection& that)
    : name( that.name), size( that.size), start_addr( that.start_addr), content( new uint8[size])
{
    std::memcpy(this->content.get(), that.content.get(), this->size);
}

std::list<ElfSection> ElfSection::getAllElfSections( const std::string& elf_file_name)
{
    // open the binary file, we have to use C-style open,
    // because it is required by elf_begin function
    std::unique_ptr<FILE, decltype(&fclose)> file( fopen( elf_file_name.c_str(), "rb"), fclose);
    if ( file == nullptr)
    {
        std::cerr << "ERROR: Could not open file " << elf_file_name << ": "
                  << std::strerror( errno) << std::endl;
        return {};
    }

    // set ELF library operating version
    if ( elf_version( EV_CURRENT) == EV_NONE)
    {
        std::cerr << "ERROR: Could not set ELF library operating version:"
                  <<  elf_errmsg( elf_errno()) << std::endl;
        return {};
    }

    // open the file in ELF format
    Elf* elf = elf_begin( fileno( file.get()), ELF_C_READ, nullptr);
    if ( elf == nullptr)
    {
        std::cerr << "ERROR: Could not open file " << elf_file_name
                  << " as ELF file: "
                  <<  elf_errmsg( elf_errno()) << std::endl;
        return {};
    }

    size_t shstrndx;
    elf_getshdrstrndx( elf, &shstrndx);

    std::list<ElfSection> sections;

    Elf_Scn *section = nullptr;
    while ( (section = elf_nextscn( elf, section)) != nullptr)
    {
        Elf32_Shdr shdr = *elf32_getshdr( section);

        char* name = elf_strptr( elf, shstrndx, shdr.sh_name);
        Addr start_addr = shdr.sh_addr;

        if ( start_addr == 0)
            continue;

        size_t size = shdr.sh_size;
        auto offset = shdr.sh_offset;
        auto content = std::make_unique<uint8[]>( size);

        fseek( file.get(), offset, SEEK_SET);

        // fill the content by the section data
        ignored( std::fread( content.get(), sizeof( uint8), size, file.get()));
        sections.emplace( sections.end(), name, start_addr, size, std::move(content));
    }

    // close all used files
    elf_end( elf);

    return sections;
}

std::string ElfSection::dump( const std::string& indent) const
{
    std::ostringstream oss;

    oss << indent << R"("Dump ELF section ")" << this->name << R"(")" << std::endl
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

        if ( substr == "00000000")
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
        // need converting to uint16 to be not preinted as an alphabet symbol
        oss << static_cast<uint16>(get_byte(i));
    }

    return oss.str();
}

std::string ElfSection::strByWords() const
{
    // temp stream is used to convert numbers into the output string
    std::ostringstream oss;
    oss << std::hex;

    union uint64_8
    {
        uint8 bytes[ sizeof( uint64) / sizeof( uint8)];
        uint64 val = 0ull;
    } value;

    // convert each words of 4 bytes into 8 hex digits
    for ( size_t i = 0; i < this->size; ++i)
    {
        oss.width( 8); // because we need 8 hex symbols to print a word (e.g. "ffffffff")
        oss.fill( '0'); // thus, number a44f will be printed as "0000a44f"
        auto mask = sizeof( uint64) / sizeof( uint8) - 1;
        value.bytes[ i & mask] = get_byte(i);
        if ( ( i & mask) == mask)
            oss << value.val;
    }

    return oss.str();
}
