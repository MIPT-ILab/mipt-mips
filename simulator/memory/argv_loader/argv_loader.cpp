/**
 * argv_loader.cpp: loader of arguments to guest memory
 * @author Rustem Yunusov <yunusov.rr@phystech.edu>
 * Copyright 2019 MIPT-MIPS iLab project
 */

#include "argv_loader.h"

ArgvLoader::ArgvLoader( const char* const* argv, const char* const* envp)
        : argc( count_argc( argv))
        , argv( argv)
        , envp( envp)
{}

void ArgvLoader::load_argv_to( FuncMemory* mem, Addr addr)
{
    std::shared_ptr<FuncMemory> plain_mem = mem -> create_plain_memory();
    plain_mem -> memcpy_host_to_guest( addr, byte_cast( &argc), 4);
}
