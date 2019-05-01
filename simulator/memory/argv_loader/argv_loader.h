/**
 * argv_loader.h: loader of arguments to guest memory
 * @author Rustem Yunusov <yunusov.rr@phystech.edu>
 * Copyright 2019 MIPT-MIPS iLab project
 */

#ifndef ARGV_LOADER_H
#define ARGV_LOADER_H

#include <infra/types.h>
#include <memory/memory.h>
#include <infra/argv.h>
#include <infra/byte.h>

struct ArgvLoaderError : Exception
{
    explicit ArgvLoaderError(const std::string& msg) :
            Exception("Error while loading arguments to guest memory", msg) { }
};

class ArgvLoader
{
public:
    explicit ArgvLoader( const char* const* argv, const char* const* envp = nullptr);
    ~ArgvLoader();

    // We delete everything explicitly
    ArgvLoader( const ArgvLoader&) = delete;
    ArgvLoader( ArgvLoader&&) = delete;
    ArgvLoader& operator=( const ArgvLoader&) = delete;
    ArgvLoader& operator=( ArgvLoader&&) = delete;

    void load_argv_to( FuncMemory* mem, Addr addr);
private:
    const int argc;
    const char* const* argv;
    const char* const* envp;
    Addr offset;
    Addr envp_offset;

    size_t place_nullptr( const std::shared_ptr<FuncMemory>& plain_mem, Addr addr)
    {
        const char* null = nullptr;
        try {
            return plain_mem -> memcpy_host_to_guest( addr, byte_cast( &null), 8);
        }
        catch( FuncMemoryOutOfRange const& e) { throw ArgvLoaderError( std::string( "can't place nullptr") + e.what()); }
    }

    void load_argv_contents( const std::shared_ptr<FuncMemory>& plain_mem, Addr addr);
    void load_envp_contents( const std::shared_ptr<FuncMemory>& plain_mem, Addr addr);
};

#endif // ARGV_LOADER_H
