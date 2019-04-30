/**
 * argv_loader.cpp: loader of arguments to guest memory
 * @author Rustem Yunusov <yunusov.rr@phystech.edu>
 * Copyright 2019 MIPT-MIPS iLab project
 */

#include "argv_loader.h"
#include <infra/exception.h>

struct InvalidArgs : std::invalid_argument
{
    explicit InvalidArgs(const std::string& msg) :
            invalid_argument( msg) { }
};

struct ArgvLoaderError : Exception
{
    explicit ArgvLoaderError(const std::string& msg) :
            Exception("Error while loading arguments to guest memory", msg) { }
};

ArgvLoader::ArgvLoader( const char* const* argv, const char* const* envp)
        : argc( count_argc( argv))
        , argv( argv)
        , envp( envp)
        , envp_shift( 0)
{
    if ( !argc)
        throw InvalidArgs( "argc == 0");

    if ( !argv)
        throw InvalidArgs( "argv == nullptr");
}

void ArgvLoader::load_argv_to( FuncMemory* mem, Addr addr)
{
    std::shared_ptr<FuncMemory> plain_mem = mem -> create_plain_memory();

    try { plain_mem -> memcpy_host_to_guest( addr, byte_cast( &argc), 4); }
    catch( FuncMemoryOutOfRange const& e) { throw ArgvLoaderError( std::string( "argc") + e.what()); }

    try { plain_mem -> memcpy_host_to_guest( addr + 4, byte_cast( argv), argc * 4); }
    catch( FuncMemoryOutOfRange const& e) { throw ArgvLoaderError( std::string( "argv") + e.what()); }

    while ( envp + envp_shift)
    {
        try { plain_mem -> memcpy_host_to_guest( addr + ( 1 + argc) * 4 + envp_shift, byte_cast( envp + envp_shift), 4); }
        catch( FuncMemoryOutOfRange const& e) { throw ArgvLoaderError( std::string( "envp") + e.what()); }
        envp_shift += 4;
    }
}
