/**
 * argv_loader.cpp: loader of arguments to guest memory
 * @author Rustem Yunusov <yunusov.rr@phystech.edu>
 * Copyright 2019 MIPT-MIPS iLab project
 */

#include "argv_loader.h"

#include <cstring>
#include <infra/exception.h>

ArgvLoader::ArgvLoader( const char* const* argv, const char* const* envp)
        : argc( argv ? count_argc( argv) : 0)
        , argv( argv)
        , envp( envp)
        , offset( 0)
{}

size_t ArgvLoader::load_argv_to( const std::shared_ptr<FuncMemory>& mem, Addr addr)
{
    mem->write<int, Endian::little>( argc, addr + offset);
    offset += bytewidth<int>;

    offset += argc * bytewidth<Addr>; //reserved space for argv[]

    place_nullptr( mem, addr + offset);
    offset += bytewidth<Addr>;

    if ( envp)
    {
        int contents_offset = 0;

        while ( envp[ contents_offset++])
        {
            offset += bytewidth<Addr>;
        }

        place_nullptr( mem, addr + offset);
        offset += bytewidth<Addr>;
    }

    load_argv_contents( mem, addr);

    if ( envp)
    {
        load_envp_contents( mem, addr);
    }

    return offset;
}

void ArgvLoader::load_argv_contents( const std::shared_ptr<FuncMemory>& mem, Addr addr)
{
    for ( int contents_offset = 0; contents_offset < argc; contents_offset++)
    {
        mem->write<Addr, Endian::little>( offset, addr + bytewidth<int> + contents_offset * bytewidth<Addr>);

        std::string content( argv[contents_offset]);
        mem->write_string( content,  addr + offset);
        offset += content.size();
    }
}

void ArgvLoader::load_envp_contents( const std::shared_ptr<FuncMemory>& mem, Addr addr)
{
    for ( int contents_offset = 0; envp[contents_offset] != nullptr; contents_offset++)
    {
        mem->write<Addr, Endian::little>( offset, addr + bytewidth<int> + ( argc + 1 + contents_offset) * bytewidth<Addr>);

        std::string content( envp[contents_offset]);
        mem->write_string( content,  addr + offset);
        offset += content.size();
    }
}
