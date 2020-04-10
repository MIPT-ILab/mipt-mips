/**
 * memory.cpp - programer-visible memory space
 * @author Pavel Kryukov <pavel.kryukov@phystech.edu>
 * Copyright 2012-2018 uArchSim iLab project
 */

#include <memory/memory.h>

#include <sstream>
#include <vector>

FuncMemoryBadMapping::FuncMemoryBadMapping( const std::string& msg) :
    Exception( "Invalid FuncMemory mapping", msg)
{ }

FuncMemoryOutOfRange::FuncMemoryOutOfRange( Addr addr, Addr mask) :
    Exception( "Out of memory range", generate_string( addr, mask))
{ }

std::string FuncMemoryOutOfRange::generate_string( Addr addr, Addr mask)
{
    std::ostringstream oss;
    oss <<  "address: 0x" << std::hex << addr << "; max address: 0x" << mask;
    return oss.str();
}

DestructableMemory::DestructableMemory() = default;
DestructableMemory::~DestructableMemory() = default;

class ZeroMemory : public ReadableMemory
{
public:
    size_t memcpy_guest_to_host( std::byte* dst, Addr /* src */, size_t size) const noexcept final;
    void duplicate_to( std::shared_ptr<WriteableMemory> /* target */) const final { }
    std::string dump() const final { return std::string( "empty memory\n"); }
    size_t strlen( Addr /* addr */) const final { return 0; }
};

std::shared_ptr<ReadableMemory> ReadableMemory::create_zero_memory()
{
    return std::make_shared<ZeroMemory>();
}


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

void WriteableMemory::memset( Addr addr, std::byte value, size_t size)
{
    for ( size_t i = 0; i < size; ++i)
    	memcpy_host_to_guest( addr + i, &value, 1);
}

size_t ZeroMemory::memcpy_guest_to_host( std::byte* dst, Addr /* src */, size_t size) const noexcept
{
    std::fill_n( dst, size, std::byte{});
    return size;
}
