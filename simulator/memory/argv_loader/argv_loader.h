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
    int envp_shift;
};

#endif // ARGV_LOADER_H
