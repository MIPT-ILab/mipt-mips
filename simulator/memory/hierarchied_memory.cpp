/**
 * hierarchied_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012-2018 uArchSim iLab project
 */


// MIPT-MIPS modules
#include <infra/macro.h>
#include <infra/types.h>
#include <memory/memory.h>

// Generic C++
#include <algorithm>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

template<typename ... Args> constexpr size_t min_sizeof() noexcept { return (std::min)({sizeof(Args)...}); }
static_assert(min_sizeof<char, int, uint64>() == sizeof(char));

class HierarchiedMemoryArgumentChecker
{
    public:
        HierarchiedMemoryArgumentChecker( uint32 addr_bits, uint32 page_bits, uint32 offset_bits)
            : page_bits( page_bits)
            , offset_bits( offset_bits)
            , set_bits( addr_bits - offset_bits - page_bits)
        {
            using namespace std::literals::string_literals;
            if ( set_bits >= min_sizeof<uint32, size_t>() * 8)
                throw FuncMemoryBadMapping("Too many ( 2 ** "s + std::to_string( set_bits) + ") sets");

            if ( page_bits >= min_sizeof<uint32, size_t>() * 8)
                throw FuncMemoryBadMapping("Too many ( 2 ** "s + std::to_string( page_bits) + ") pages");

            if ( offset_bits >= min_sizeof<uint32, size_t>() * 8)
                throw FuncMemoryBadMapping("Each page is too large ( 2 ** "s + std::to_string( offset_bits) + " bytes)");
        }
    protected:
        const uint32 page_bits;
        const uint32 offset_bits;
        const uint32 set_bits;
};

class HierarchiedMemory : public FuncMemory, private HierarchiedMemoryArgumentChecker
{
    public:
        HierarchiedMemory ( uint32 addr_bits, uint32 page_bits, uint32 offset_bits);

        std::string dump() const final;
        size_t memcpy_host_to_guest( Addr dst, const std::byte* src, size_t size) final;
        size_t memcpy_guest_to_host( std::byte* dst, Addr src, size_t size) const noexcept final;
        size_t strlen( Addr addr) const final;
        void duplicate_to( std::shared_ptr<WriteableMemory> target) const final;

    private:
        const Addr addr_mask;
        const Addr offset_mask;
        const Addr page_mask;
        const Addr set_mask;

        const size_t page_cnt;
        const size_t set_cnt;
        const size_t page_size;

        using Page = std::vector<std::byte>;
        using Set  = std::vector<Page>;
        using Mem  = std::vector<Set>;
        Mem memory = {};

        size_t get_set( Addr addr) const noexcept;
        size_t get_page( Addr addr) const noexcept;
        size_t get_offset( Addr addr) const noexcept;

        Addr get_addr( Addr set, Addr page, Addr offset) const noexcept;
        Addr get_addr(const Mem::const_iterator& set_it,
                      const Set::const_iterator& page_it,
                      const Page::const_iterator& byte_it) const noexcept;

        bool check( Addr addr) const noexcept;
        std::byte read_byte( Addr addr) const noexcept;
        std::byte check_and_read_byte( Addr addr) const noexcept;

        void alloc( Addr addr);
        void write_byte( Addr addr, std::byte value);
        void alloc_and_write_byte( Addr addr, std::byte value);
};

std::shared_ptr<FuncMemory>
FuncMemory::create_hierarchied_memory( uint32 addr_bits, uint32 page_bits, uint32 offset_bits)
{
    return std::make_shared<HierarchiedMemory>( addr_bits, page_bits, offset_bits);
}

HierarchiedMemory::HierarchiedMemory( uint32 addr_bits,
                        uint32 page_bits,
                        uint32 offset_bits) :
    HierarchiedMemoryArgumentChecker( addr_bits, page_bits, offset_bits),
    addr_mask( bitmask<Addr>( std::min<uint32>( addr_bits, bitwidth<Addr>))),
    offset_mask( bitmask<Addr>( offset_bits)),
    page_mask ( bitmask<Addr>( page_bits) << offset_bits),
    set_mask ( bitmask<Addr>( set_bits) << ( page_bits + offset_bits)),
    page_cnt ( 1ULL << page_bits ),
    set_cnt ( 1ULL << set_bits ),
    page_size ( 1ULL << offset_bits)
{
    memory.resize(set_cnt);
}

size_t HierarchiedMemory::memcpy_host_to_guest( Addr dst, const std::byte* src, size_t size)
{
    if (size > addr_mask + 1)
        throw FuncMemoryOutOfRange( dst + size, addr_mask + 1);

    if (dst > addr_mask + 1)
        throw FuncMemoryOutOfRange( dst, addr_mask + 1);

    if (dst > addr_mask + 1 - size)
        throw FuncMemoryOutOfRange( dst, addr_mask + 1);

    size_t offset = 0;
    for (; offset < size; ++offset)
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic) Low level access
        alloc_and_write_byte( dst + offset, src[offset]);
    return offset;
}

size_t HierarchiedMemory::memcpy_guest_to_host( std::byte *dst, Addr src, size_t size) const noexcept
{
    size_t offset = 0;
    for (; offset < size; ++offset)
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic) Low level access
        dst[offset] = check_and_read_byte( src + offset);
    return offset;
}

void HierarchiedMemory::alloc_and_write_byte( Addr addr, std::byte value)
{
    alloc( addr);
    write_byte( addr, value);
}

void HierarchiedMemory::alloc( Addr addr)
{
    auto& set = memory[get_set(addr)];
    if ( set.empty())
        set.resize(page_cnt);

    auto& page = set[get_page(addr)];
    if ( page.empty())
        page.resize(page_size, std::byte());
}

bool HierarchiedMemory::check( Addr addr) const noexcept
{
    const auto& set = memory[get_set(addr)];
    return !set.empty() && !set[get_page(addr)].empty();
}

void HierarchiedMemory::duplicate_to( std::shared_ptr<WriteableMemory> target) const
{
    for ( auto set_it = memory.begin(); set_it != memory.end(); ++set_it)
        for ( auto page_it = set_it->begin(); page_it != set_it->end(); ++page_it)
            target->memcpy_host_to_guest( get_addr( set_it, page_it, page_it->begin()),
                                          page_it->data(),
                                          page_it->size());
}

std::string HierarchiedMemory::dump() const
{
    std::ostringstream oss;
    oss << std::setfill( '0') << std::hex;

    for ( auto set_it = memory.begin(); set_it != memory.end(); ++set_it)
        for ( auto page_it = set_it->begin(); page_it != set_it->end(); ++page_it)
            for ( auto byte_it = page_it->begin(); byte_it != page_it->end(); ++byte_it)
                if ( uint32( *byte_it) != 0)
                    oss << "addr 0x" << get_addr( set_it, page_it, byte_it)
                        << ": data 0x" << uint32( *byte_it) << std::endl;

    return std::move( oss).str();
}

Addr HierarchiedMemory::get_addr(const Mem::const_iterator& set_it, const Set::const_iterator& page_it, const Page::const_iterator& byte_it) const noexcept
{
    return get_addr( std::distance( memory.begin(), set_it),
                     std::distance( set_it->begin(), page_it),
                     std::distance( page_it->begin(), byte_it));
}

inline size_t HierarchiedMemory::get_set( Addr addr) const noexcept
{
    return ( addr & set_mask) >> ( page_bits + offset_bits);
}

inline size_t HierarchiedMemory::get_page( Addr addr) const noexcept
{
    return ( addr & page_mask) >> offset_bits;
}

inline size_t HierarchiedMemory::get_offset( Addr addr) const noexcept
{
    return ( addr & offset_mask);
}

inline Addr HierarchiedMemory::get_addr( Addr set, Addr page, Addr offset) const noexcept
{
    return (set << (page_bits + offset_bits)) | (page << offset_bits) | offset;
}

inline std::byte HierarchiedMemory::read_byte( Addr addr) const noexcept
{
    return memory[get_set(addr)][get_page(addr)][get_offset(addr)];
}

std::byte HierarchiedMemory::check_and_read_byte( Addr addr) const noexcept
{
    return check( addr) ? read_byte( addr) : std::byte{};
}

inline void HierarchiedMemory::write_byte( Addr addr, std::byte value)
{
    memory[get_set(addr)][get_page(addr)][get_offset(addr)] = value;
}

size_t HierarchiedMemory::strlen( Addr addr) const
{
    for (size_t i = 0; i <= addr_mask; ++i)
        if ( check_and_read_byte( addr + i) == std::byte{})
            return i;

    return addr_mask + 1;
}
