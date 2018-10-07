/**
 * elf-loader.cpp: loader of elf files to guest memory
 * @author Pavel Kryukov
 * Copyright 2018 uArchSim iLab project
 */

#include "elf_loader.h"

#include <elfio/elfio.hpp>
#include <infra/memory/memory.h>

static void load_elf_section( FuncMemory* memory, const ELFIO::section* section)
{
    if ( section->get_address() == 0)
        return;

    if ( section->get_name() == ".text")
        memory->set_startPC( section->get_address());

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast) Connecting ELFIO to our guidelines
    memory->memcpy_host_to_guest( section->get_address(), reinterpret_cast<const Byte*>(section->get_data()), section->get_size());
}


void load_elf_file( FuncMemory* memory, const std::string& executable_file_name)
{
    ELFIO::elfio reader;

    if ( !reader.load( executable_file_name))
        throw InvalidElfFile( executable_file_name);

    for ( const auto& section : reader.sections)
        load_elf_section( memory, section);
}
