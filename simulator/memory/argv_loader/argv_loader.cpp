/**
 * argv_loader.cpp: loader of arguments to guest memory
 * @author Rustem Yunusov <yunusov.rr@phystech.edu>
 * Copyright 2019 MIPT-MIPS iLab project
 */

#include "argv_loader.h"

#include <cstring>
#include <infra/exception.h>

template <typename T, std::endian endian>
ArgvLoader<T, endian>::ArgvLoader( const char* const* argv, const char* const* envp)
        : argc( argv != nullptr ? count_argc( argv) : 0)
        , argv( argv)
        , envp( envp)
        , offset( 0)
{}

template <typename T, std::endian endian>
size_t ArgvLoader<T, endian>::load_to( const std::shared_ptr<FuncMemory>& mem, Addr addr)
{
    mem->write<T, endian>( narrow_cast<T>( argc), addr + offset);
    offset += GUEST_WORD_SIZE;

    offset += argc * GUEST_WORD_SIZE; // reserved space for argv[]

    place_nullptr( mem, addr + offset);
    offset += GUEST_WORD_SIZE;

    if ( envp != nullptr)
    {
        offset += count_argc( envp) * GUEST_WORD_SIZE; // reserved space for envp[]

        place_nullptr( mem, addr + offset);
        offset += GUEST_WORD_SIZE;
    }

    load_argv_contents( mem, addr);

    if ( envp != nullptr)
        load_envp_contents( mem, addr);

    return offset;
}

template<typename T, std::endian endian>
void ArgvLoader<T, endian>::load_argv_contents( const std::shared_ptr<FuncMemory>& mem, Addr addr)
{
    for ( int contents_offset = 0; contents_offset < argc; contents_offset++)
    {
        mem->write<T, endian>( narrow_cast<T>( addr + offset), addr + ( 1 + contents_offset) * GUEST_WORD_SIZE);

        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        std::string content( argv[contents_offset]);
        mem->write_string( content,  addr + offset);
        offset += content.size();

        place_nullterminator( mem, addr + offset);
        offset += bytewidth<char>;
    }
}

template<typename T, std::endian endian>
void ArgvLoader<T, endian>::load_envp_contents( const std::shared_ptr<FuncMemory>& mem, Addr addr)
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    for ( int contents_offset = 0; envp[contents_offset] != nullptr; contents_offset++)
    {
        mem->write<T, endian>( narrow_cast<T>( addr + offset), addr + ( 1 + argc + 1 + contents_offset) * GUEST_WORD_SIZE);

        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        std::string content( envp[contents_offset]);
        mem->write_string( content,  addr + offset);
        offset += content.size();

        place_nullterminator( mem, addr + offset);
        offset += bytewidth<char>;
    }
}

template class ArgvLoader<uint32, std::endian::little>;
template class ArgvLoader<uint32, std::endian::big>;
template class ArgvLoader<uint64, std::endian::little>;
template class ArgvLoader<uint64, std::endian::big>;
