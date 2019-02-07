/**
 * memory.cpp - programer-visible memory space
 * @author Pavel Kryukov <pavel.kryukov@phystech.edu>
 * Copyright 2012-2018 uArchSim iLab project
 */

#include "memory.h"

#include <vector>

std::string ReadableMemory::read_string( Addr addr) const
{
    return read_string_by_size( addr, strlen( addr));
}

std::string ReadableMemory::read_string_limited( Addr addr, size_t size) const
{
    auto length = std::min<size_t>( size, strlen( addr));
    return read_string_by_size( addr, length);
}

std::string ReadableMemory::read_string_by_size( Addr addr, size_t size) const
{
    std::vector<char> tmp( size);
    memcpy_guest_to_host( byte_cast( tmp.data()), addr, tmp.size());
    return std::string( tmp.data(), tmp.size()); // but how to move?
}

void WriteableMemory::write_string( const std::string& value, Addr addr)
{
    write_string_by_size( value, addr, value.size());
}

void WriteableMemory::write_string_limited( const std::string& value, Addr addr, size_t size)
{
    auto length = std::min<size_t>( size, value.size());
    write_string_by_size( value, addr, length);
}

void WriteableMemory::write_string_by_size( const std::string& value, Addr addr, size_t size)
{
    memcpy_host_to_guest( addr, byte_cast( value.c_str()), size);
}

size_t ZeroMemory::memcpy_guest_to_host( Byte* dst, Addr /* src */, size_t size) const noexcept
{
    std::fill_n( dst, size, Byte{});
    return size;
}
