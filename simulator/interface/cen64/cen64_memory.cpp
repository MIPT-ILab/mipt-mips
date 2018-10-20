/*
 * cen64_memory.cpp - memory interface between MIPT-MIPS and CEN64
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include <infra/endian.h>
#include <memory/memory.h>

#include <cassert>
#include <memory>

struct bus_controller;
 
extern "C" {
    
int bus_read_word(const bus_controller *bus, uint32_t address, uint32_t *word);
int bus_write_word(bus_controller *bus, uint32_t address, uint32_t word, uint32_t dqm);

}

class CEN64Memory : public FuncMemory
{
public:
    explicit CEN64Memory(bus_controller* value) { bus = value; }

    size_t memcpy_host_to_guest( Addr dst, const Byte* src, size_t size) final;
    size_t memcpy_guest_to_host( Byte* dst, Addr src, size_t size) const noexcept final;
    std::string dump() const final { assert(0); return {}; }
    void duplicate_to( FuncMemory*) const final { assert(0); }
private:
    bus_controller* bus = nullptr;

    size_t memcpy_guest_to_host_word( Byte* dst, Addr src, size_t size = 4) const noexcept {
        uint32 val;
        size_t result = bus_read_word( bus, src, &val);
        auto tmp = unpack_array<uint32, Endian::big>( val);
        for (size_t i = 0; i < size; ++i)
            dst[i] = tmp[i];
        return result;
    }

    size_t memcpy_host_to_guest_word( Addr dst, const Byte* src, size_t size = 4) const {
        std::array<Byte, 4> tmp;
        for (size_t i = 0; i < size; ++i)
            tmp[i] = src[i];

        return bus_write_word( bus, dst, pack_array<uint32, Endian::big>( tmp), bitmask<uint32>(size * CHAR_BIT));
    }
};

size_t CEN64Memory::memcpy_guest_to_host( Byte* dst, Addr src, size_t length) const noexcept
{
    size_t result = 0;
    for (size_t i = 0; i < length; i += 4)
        result += memcpy_guest_to_host_word(dst + i, src + i);

    size_t remainder = length % 4;
    result += memcpy_guest_to_host_word(dst + length - remainder, src + length - remainder, remainder);
    return result;
}

size_t CEN64Memory::memcpy_host_to_guest( Addr dst, const Byte* src, size_t length)
{
    size_t result = 0;
    for (size_t i = 0; i < length; i += 4)
        result += memcpy_host_to_guest_word(dst + i, src + i);

    size_t remainder = length % 4;
    result += memcpy_host_to_guest_word(dst + length - remainder, src + length - remainder, remainder);
    return result;
}

std::unique_ptr<FuncMemory> generate_cen64_memory( bus_controller * bus_ptr)
{
    return std::make_unique<CEN64Memory>( bus_ptr);
}
