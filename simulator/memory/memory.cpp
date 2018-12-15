/**
 * memory.cpp - programer-visible memory space
 * @author Pavel Kryukov <pavel.kryukov@phystech.edu>
 * Copyright 2012-2018 uArchSim iLab project
 */

#include "memory.h"

#include <vector>

std::string ReadableMemory::read_string( Addr addr) const
{
    std::vector<char> tmp( strlen( addr));
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast) Cast from characters
    memcpy_guest_to_host( reinterpret_cast<Byte*>( tmp.data()), addr, tmp.size());
    return std::string( tmp.begin(), tmp.end());
}

void WriteableMemory::write_string( const std::string& value, Addr addr)
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast) Cast from characters
    memcpy_host_to_guest( addr, reinterpret_cast<const Byte*>( value.c_str()), value.size());
}

size_t ZeroMemory::memcpy_guest_to_host( Byte* dst, Addr /* src */, size_t size) const noexcept
{
    std::fill_n( dst, size, Byte{});
    return size;
}
