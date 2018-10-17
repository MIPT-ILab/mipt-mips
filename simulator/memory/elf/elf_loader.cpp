/**
 * elf-loader.cpp: loader of elf files to guest memory
 * @author Pavel Kryukov
 * Copyright 2018 uArchSim iLab project
 */

#include "elf_loader.h"

#include <elfio/elfio.hpp>
#include <memory/memory.h>

static void load_elf_section( FuncMemory* memory, const ELFIO::section& section, AddrDiff offset)
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast) Connecting ELFIO to our guidelines
    memory->memcpy_host_to_guest( section.get_address() + offset, reinterpret_cast<const Byte*>(section.get_data()), section.get_size());
}

static void set_startPC( FuncMemory* memory, const ELFIO::elfio& reader, AddrDiff offset)
{
    if ( reader.sections[ ".text"] != nullptr)
        memory->set_startPC( reader.sections[ ".text"]->get_address() + offset);
}

static void load_all_elf_sections( FuncMemory* memory, const ELFIO::elfio& reader, AddrDiff offset)
{
    for ( const auto& section : reader.sections)
        if ( section->get_address() != 0)
            load_elf_section( memory, *section, offset);
}

static ELFIO::elfio get_elfio_reader( const std::string& filename)
{
    ELFIO::elfio reader;

    if ( !reader.load( filename))
        throw InvalidElfFile( filename);

    return reader;
}

void load_elf_file( FuncMemory* memory, const std::string& filename, AddrDiff offset)
{
    const auto& reader = get_elfio_reader( filename);
    load_all_elf_sections( memory, reader, offset);
    set_startPC( memory, reader, offset);
}
