/**
 * elf-loader.cpp: loader of elf files to guest memory
 * @author Pavel Kryukov
 * Copyright 2018 uArchSim iLab project
 */

#include "elf_loader.h"

#include <elfio/elfio.hpp>
#include <memory/memory.h>

#include <string>

static void load_elf_section( WriteableMemory* memory, const ELFIO::section& section, AddrDiff offset)
{
    using namespace std::literals::string_literals;

    if ( section.get_address() == 0)
        throw InvalidElfSection( "\""s + section.get_name() + "\""s);

    if ( section.get_data() == nullptr) // BSS
        memory->memset( section.get_address() + offset, std::byte{}, section.get_size());
    else
        memory->memcpy_host_to_guest( section.get_address() + offset, byte_cast( section.get_data()), section.get_size());
}

ElfLoader::ElfLoader( std::string_view filename)
    : reader( std::make_unique<ELFIO::elfio>())
{
    std::string name( filename);
    if ( !reader->load( name))
        throw InvalidElfFile( name);
}

void ElfLoader::load_to( WriteableMemory *memory, AddrDiff offset) const
{
    for ( const auto& section : reader->sections)
        if ( ( section->get_flags() & narrow_cast<decltype(section->get_flags())>( SHF_ALLOC)) != 0)
            load_elf_section( memory, *section, offset);
}

Addr ElfLoader::get_text_section_addr() const
{
    return reader->sections[ ".text"] != nullptr ? reader->sections[ ".text"]->get_address() : 0;
}

static std::pair<bool, ELFIO::Elf64_Addr>
is_start_section( const ELFIO::symbol_section_accessor& symbols, ELFIO::Elf_Xword id)
{
    std::string name;
    ELFIO::Elf64_Addr value = 0;
    ELFIO::Elf_Xword size = 0;
    unsigned char bind = 0;
    unsigned char type = 0;
    ELFIO::Elf_Half section_index = 0;
    unsigned char other = 0;
    symbols.get_symbol( id, name, value, size, bind, type, section_index, other);
    return std::pair( name == "__start" || name == "_start", value);
}

Addr ElfLoader::get_startPC() const
{
    for ( const auto& section : reader->sections) {
        if ( section->get_type() != SHT_SYMTAB)
            continue;

        ELFIO::symbol_section_accessor symbols(*reader, section);
        for ( ELFIO::Elf_Xword j = 0; j < symbols.get_symbols_num(); ++j) {
            auto [result, addr] = is_start_section( symbols, j);
            if ( result)
                return addr;
        }
    }

    throw InvalidEntryPoint();
}

ElfLoader::~ElfLoader() = default;
