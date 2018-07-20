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
#include <elfio/elfio.hpp>

// uArchSim modules
#include <infra/macro.h>
#include "elf_parser.h"

ElfSection::ElfSection( std::string name, Addr start_addr, Addr size, const void* ptr)
    : name( std::move( name)), size( size), start_addr( start_addr), content( new uint8[size])
{
    std::memcpy(content.get(), ptr, size);
}

std::list<ElfSection> ElfSection::getAllElfSections( const std::string& elf_file_name)
{
    // open the binary file, we have to use C-style open,
    // because it is required by elf_begin function
    ELFIO::elfio reader;

    if ( !reader.load( elf_file_name))
        throw InvalidElfFile(elf_file_name, " file is not readable");

    std::list<ElfSection> sections;

    for ( const auto& sec : reader.sections)
        if ( sec->get_address() != 0)
            sections.emplace_back( sec->get_name(), sec->get_address(), sec->get_size(), sec->get_data());

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
