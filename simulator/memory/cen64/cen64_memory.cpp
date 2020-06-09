/*
 * cen64_memory.cpp - memory interface between MIPT-MIPS and CEN64
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include <infra/endian.h>
#include <memory/memory.h>

#include <cassert>
#include <memory>

#include "cen64_memory.h"
 
extern "C" {
    
int bus_read_word(const bus_controller *bus, uint32_t address, uint32_t *word);
int bus_write_word(bus_controller *bus, uint32_t address, uint32_t word, uint32_t dqm);

}

class CEN64Memory : public FuncMemory
{
public:
    explicit CEN64Memory(bus_controller* value) : bus( value) { }

    size_t memcpy_host_to_guest( Addr dst, const std::byte* src, size_t size) final
    {
        return copy_by_words( dst, src, size);
    }

    size_t memcpy_guest_to_host( std::byte* dst, Addr src, size_t size) const noexcept final
    {
        return copy_by_words( dst, src, size);
    }

    std::string dump() const final
    {
        throw CEN64MemoryUnsupportedInterface("dump");
    }

    void duplicate_to( std::shared_ptr<WriteableMemory> /* target */) const final
    {
        throw CEN64MemoryUnsupportedInterface("duplication");
    }

    size_t strlen( Addr /* addr */) const final
    {
        throw CEN64MemoryUnsupportedInterface("string output");
    }

private:
    bus_controller* const bus = nullptr;

    size_t copy_word( std::byte* dst, Addr src, size_t size) const noexcept
    {
        uint32 val = 0;
        size_t result = bus_read_word( bus, src, &val);
        put_value_to_pointer<uint32, std::endian::big>( dst, val, size);
        return result;
    }

    size_t copy_word( Addr dst, const std::byte* src, size_t size) const noexcept
    {
        auto val = get_value_from_pointer<uint32, std::endian::big>( src, size);
        auto dqm = swap_endian( bitmask<uint32>( narrow_cast<uint32>( size * CHAR_BIT)));
        return bus_write_word( bus, dst, val, dqm);
    }

    template<typename D, typename S>
    size_t copy_by_words(D dst, S src, size_t size) const noexcept
    {
        static const constexpr size_t word_size = bytewidth<uint32>;
        size_t result = 0;
        size_t remainder = size % word_size;

        if ( remainder != 0)
            result += copy_word(dst, src, remainder); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

        for ( size_t i = remainder; i < size; i += word_size)
            result += copy_word(dst + i, src + i, word_size); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

        return result;
    }
};

std::shared_ptr<FuncMemory> create_cen64_memory( bus_controller * bus_ptr)
{
    return std::make_shared<CEN64Memory>( bus_ptr);
}
