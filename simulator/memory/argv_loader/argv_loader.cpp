/**
 * argv_loader.cpp: loader of arguments to guest memory
 * @author Rustem Yunusov <yunusov.rr@phystech.edu>
 * Copyright 2019 MIPT-MIPS iLab project
 */

#include "argv_loader.h"
#include <infra/exception.h>
#include <cstring>

ArgvLoader::ArgvLoader( const char* const* argv, const char* const* envp)
        : argc( argv ? count_argc( argv) : 0)
        , argv( argv)
        , envp( envp)
        , offset( 0)
        , envp_offset( 0)
{
    if ( !argc)
        throw InvalidArgs( "argc == 0");

    if ( !argv)
        throw InvalidArgs( "argv == nullptr");
}

void ArgvLoader::load_argv_to( const std::shared_ptr<FuncMemory>& mem, Addr addr)
{
    const std::shared_ptr<FuncMemory> plain_mem = mem -> create_plain_memory();

    try { offset += plain_mem -> memcpy_host_to_guest( addr + offset, byte_cast( &argc), 4); }
    catch( FuncMemoryOutOfRange const& e) { throw ArgvLoaderError( std::string( "argc") + e.what()); }

    try { offset += plain_mem -> memcpy_host_to_guest( addr + offset, byte_cast( argv), argc * 8); }
    catch( FuncMemoryOutOfRange const& e) { throw ArgvLoaderError( std::string( "argv") + e.what()); }

    offset += place_nullptr( plain_mem, addr + offset);

    if ( envp)
    {
        while ( *( envp + envp_offset))
        {
            try {
                Addr size = plain_mem->memcpy_host_to_guest(addr + offset, byte_cast( envp + envp_offset), 8);
                offset += size;
                envp_offset += size;
            }
            catch( FuncMemoryOutOfRange const &e) { throw ArgvLoaderError( std::string( "envp") + e.what()); }
        }

        offset += place_nullptr( plain_mem, addr + offset);
    }

    try { load_argv_contents( plain_mem, addr); }
    catch( FuncMemoryOutOfRange const &e) { throw ArgvLoaderError( std::string( "argv contents") + e.what()); }

    if ( envp)
    {
        try { load_envp_contents( plain_mem, addr); }
        catch( FuncMemoryOutOfRange const &e) { throw ArgvLoaderError( std::string( "envp contents") + e.what()); }
    }
}

void ArgvLoader::load_argv_contents( const std::shared_ptr<FuncMemory>& plain_mem, Addr addr)
{
    for ( int content_offset = 0; content_offset < argc; content_offset++)
    {
        if ( !argv[content_offset])
            throw ArgvLoaderError( std::string( "argv [")
                                 + std::to_string( content_offset)
                                 + std::string( "] == nullptr"));

        offset += plain_mem -> memcpy_host_to_guest( addr + offset, byte_cast( argv[content_offset]), strlen( argv[content_offset]));
    }
}

void ArgvLoader::load_envp_contents( const std::shared_ptr<FuncMemory>& plain_mem, Addr addr)
{
    for ( int content_offset = 0; envp[content_offset] != nullptr; content_offset++)
        offset += plain_mem -> memcpy_host_to_guest( addr + offset, byte_cast( envp[content_offset]), strlen( envp[content_offset]));
}

ArgvLoader::~ArgvLoader() = default;