/**
 * elf-loader.cpp: loader of elf files to guest memory
 * @author Pavel Kryukov
 * Copyright 2018 uArchSim iLab project
 */

#include "elf_loader.h"

#include <elfio/elfio.hpp>
#include <infra/memory/memory.h>

static void load_elf_section( FuncMemory* memory, const ELFIO::section& section)
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast) Connecting ELFIO to our guidelines
    memory->memcpy_host_to_guest( section.get_address(), reinterpret_cast<const Byte*>(section.get_data()), section.get_size());
}

static void set_startPC( FuncMemory* memory, const ELFIO::elfio& reader)
{
    if ( reader.sections[ ".text"] != nullptr)
        memory->set_startPC( reader.sections[ ".text"]->get_address());
}

static void load_all_elf_sections( FuncMemory* memory, const ELFIO::elfio& reader)
{
    for ( const auto& section : reader.sections)
        if ( section->get_address() != 0)
            load_elf_section( memory, *section);
}

static ELFIO::elfio get_elfio_reader( const std::string& filename)
{
    ELFIO::elfio reader;

    if ( !reader.load( filename))
        throw InvalidElfFile( filename);

    return reader;
}

void load_elf_file( FuncMemory* memory, const std::string& filename)
{
    const auto& reader = get_elfio_reader( filename);
    load_all_elf_sections( memory, reader);
    set_startPC( memory, reader);
}
