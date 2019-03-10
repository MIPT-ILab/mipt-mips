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
        if ( ( section->get_flags() & SHF_ALLOC) != 0)
            load_elf_section( memory, *section, offset);
}

Addr ElfLoader::get_startPC() const
{
    using namespace ELFIO;
    for ( const auto& section : reader->sections) {
        if ( section->get_type() != SHT_SYMTAB)
            continue;

        symbol_section_accessor symbols(*reader, section);
        for ( unsigned int j = 0; j < symbols.get_symbols_num(); ++j ) {
            std::string name;
            Elf64_Addr value = 0;
            Elf_Xword size;
            unsigned char bind;
            unsigned char type;
            Elf_Half section_index;
            unsigned char other;
            symbols.get_symbol( j, name, value, size, bind, type, section_index, other );
            if ( name == "_start")
                return offset + value;
        }
    }

    if ( reader->sections[ ".text"] != nullptr) {
        std::cout << "Warning: no _start label found, defaulting to '.text' section\n";
        return offset + reader->sections[ ".text"]->get_address();
    }

    std::cout << "Warning: no entry point found, returning 0x0\n";
    return offset;
}

ElfLoader::~ElfLoader() = default;
