/**
 * elf-loader.h: loader of elf files to guest memory
 * @author Pavel Kryukov
 * Copyright 2018 uArchSim iLab project
 */
 
#ifndef ELF_LOADER_H
#define ELF_LOADER_H

#include <infra/exception.h>
#include <infra/types.h>

#include <memory>
#include <string>

struct InvalidElfFile final : Exception
{
    explicit InvalidElfFile(const std::string& name)
        : Exception("Invalid elf file", name)
    { }
};

struct InvalidEntryPoint final : Exception
{
    explicit InvalidEntryPoint()
        : Exception("No _start or __start label found")
    { }
};

struct InvalidElfSection : Exception
{
    explicit InvalidElfSection(const std::string& section_name) :
        Exception("Malformed ELF section", section_name) { }
};

class WriteableMemory;

namespace ELFIO {
    class elfio;
} // namespace ELFIO

class ElfLoader
{
public:
    explicit ElfLoader( std::string_view filename);
    ~ElfLoader();

    // Regardless of 'const std::unique_ptr', we delete everything explicitly
    ElfLoader( const ElfLoader&) = delete;
    ElfLoader( ElfLoader&&) = delete;
    ElfLoader& operator=( const ElfLoader&) = delete;
    ElfLoader& operator=( ElfLoader&&) = delete;

    void load_to( WriteableMemory *memory, AddrDiff offset) const;
    void load_to( WriteableMemory *memory) const { load_to( memory, 0); }
    Addr get_startPC() const;
    Addr get_text_section_addr() const;
private:
    const std::unique_ptr<ELFIO::elfio> reader;
};
   
#endif
