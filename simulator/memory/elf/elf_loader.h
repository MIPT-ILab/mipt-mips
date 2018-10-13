/**
 * elf-loader.h: loader of elf files to guest memory
 * @author Pavel Kryukov
 * Copyright 2018 uArchSim iLab project
 */
 
#ifndef ELF_LOADER_H
#define ELF_LOADER_H

#include <infra/exception.h>

#include <string>

struct InvalidElfFile final : Exception
{
    explicit InvalidElfFile(const std::string& name)
        : Exception("Invalid elf file", name)
    { }
};

class FuncMemory;

void load_elf_file(FuncMemory* memory, const std::string& filename);

#endif
