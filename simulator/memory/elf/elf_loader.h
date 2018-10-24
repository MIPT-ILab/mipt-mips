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

class FuncMemory;

namespace ELFIO {
    class elfio;
} // namespace ELFIO

class ElfLoader
{
public:
    explicit ElfLoader( const std::string& filename, AddrDiff offset = 0);
    ~ElfLoader();

    // Regardless of 'const std::unique_ptr', we delete everything explicitly
    ElfLoader( const ElfLoader&) = delete;
    ElfLoader( ElfLoader&&) = delete;
    ElfLoader& operator=( const ElfLoader&) = delete;
    ElfLoader& operator=( ElfLoader&&) = delete;

    void load_to( std::shared_ptr<FuncMemory> memory) const;
    Addr get_startPC() const;
private:
    const std::unique_ptr<ELFIO::elfio> reader;
    AddrDiff offset;
};
   
#endif
