/**
 * argv_loader.h: loader of arguments to guest memory
 * @author Rustem Yunusov <yunusov.rr@phystech.edu>
 * Copyright 2019 MIPT-MIPS iLab project
 */

#ifndef ARGV_LOADER_H
#define ARGV_LOADER_H

#include <infra/argv.h>
#include <infra/byte.h>
#include <infra/types.h>
#include <memory/memory.h>

class ArgvLoader
{
public:
    explicit ArgvLoader( const char* const* argv, const char* const* envp = nullptr);

    size_t load_to( const std::shared_ptr<FuncMemory>& mem, Addr addr = 0);
private:
    const int argc;
    const char* const* argv;
    const char* const* envp;
    Addr offset;

    void place_nullptr( const std::shared_ptr<FuncMemory>& mem, Addr addr)
    {
        mem->write<Addr, Endian::little>( 0, addr);
    }

    void place_nullterminator( const std::shared_ptr<FuncMemory>& mem, Addr addr)
    {
        mem->write<char, Endian::little>( 0, addr);
    }

    void load_argv_contents( const std::shared_ptr<FuncMemory>& mem, Addr addr);
    void load_envp_contents( const std::shared_ptr<FuncMemory>& mem, Addr addr);
};

#endif // ARGV_LOADER_H