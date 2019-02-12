/**
 * elf-loader.cpp: loader of elf files to guest memory
 * @author Pavel Kryukov
 * Copyright 2018 uArchSim iLab project
 */

#include "elf_loader.h"

#include <elfio/elfio.hpp>
#include <memory/memory.h>

#include <string>

struct InvalidElfSection : Exception
{
    explicit InvalidElfSection(const std::string& section_name) :
        Exception("Malformed ELF section", section_name) { }
};

static void load_elf_section( WriteableMemory* memory, const ELFIO::section& section, AddrDiff offset)
{
    using namespace std::literals::string_literals;
    if ( section.get_address() == 0 || section.get_data() == nullptr)
        throw InvalidElfSection( "\""s + section.get_name() + "\""s);

    memory->memcpy_host_to_guest( section.get_address() + offset, byte_cast( section.get_data()), section.get_size());
}

ElfLoader::ElfLoader( const std::string& filename, AddrDiff offset)
    : reader( std::make_unique<ELFIO::elfio>())
    , offset( offset)
{
    if ( !reader->load( filename))
        throw InvalidElfFile( filename);
}

void ElfLoader::load_to( WriteableMemory *memory) const
{
    for ( const auto& section : reader->sections)
        if ( section.get_flags() & SHF_ALLOC)
            load_elf_section( memory, *section, offset);
}

Addr ElfLoader::get_startPC() const
{
    return reader->sections[ ".text"] != nullptr
        ? offset + reader->sections[ ".text"]->get_address()
        : 0;
}

ElfLoader::~ElfLoader() = default;
