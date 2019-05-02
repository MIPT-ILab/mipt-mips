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

    size_t load_argv_to( const std::shared_ptr<FuncMemory>& mem, Addr addr);
private:
    const int argc;
    const char* const* argv;
    const char* const* envp;
    Addr offset;
    Addr envp_offset;

    size_t place_nullptr( const std::shared_ptr<FuncMemory>& plain_mem, Addr addr)
    {
        const char* null = nullptr;
        return plain_mem -> memcpy_host_to_guest( addr, byte_cast( &null), bytewidth<Addr>);
    }

    void load_argv_contents( const std::shared_ptr<FuncMemory>& mem, Addr addr);
    void load_envp_contents( const std::shared_ptr<FuncMemory>& mem, Addr addr);
};

#endif // ARGV_LOADER_H
