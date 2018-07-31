/**
 * memory.h - Header of module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

#ifndef FUNC_MEMORY__FUNC_MEMORY_H
#define FUNC_MEMORY__FUNC_MEMORY_H

// uArchSim modules
#include <infra/macro.h>
#include <infra/types.h>

// Generic C++
#include <stdexcept>
#include <string>
#include <vector>

struct InvalidElfFile final : std::runtime_error
{
    InvalidElfFile(const std::string& name, const std::string& msg)
        : std::runtime_error(name + " is not a valid ELF file:" + msg + '\n')
    {}
};

struct FuncMemoryBadMapping final : std::runtime_error
{
    explicit FuncMemoryBadMapping(const std::string& msg)
        : std::runtime_error(std::string("Invalid FuncMemory mapping: ") + msg + '\n')
    {}
};

namespace ELFIO {
    class section;
} // namespace ELFIO

class FuncMemory
{
    public:
        explicit FuncMemory ( uint32 addr_bits = 32,
                              uint32 page_bits = 10,
                              uint32 offset_bits = 12);

        template<typename T>
        T read( Addr addr, T mask = all_ones<T>()) const;

        template<typename T>
        void write( T value, Addr addr, T mask = all_ones<T>());

        uint64 startPC() const { return startPC_addr; }
        std::string dump() const;
        
        void load_elf_file(const std::string& executable_file_name);
    private:
        const uint32 page_bits;
        const uint32 offset_bits;
        const uint32 set_bits;

        const Addr addr_mask;
        const Addr offset_mask;
        const Addr page_mask;
        const Addr set_mask;

        const size_t page_cnt;
        const size_t set_cnt;
        const size_t page_size;

        using Page = std::vector<Byte>;
        using Set  = std::vector<Page>;
        using Mem  = std::vector<Set>;
        Mem memory = {};
        Addr startPC_addr = NO_VAL32;

        size_t get_set( Addr addr) const;
        size_t get_page( Addr addr) const;
        size_t get_offset( Addr addr) const;

        Addr get_addr( Addr set, Addr page, Addr offset) const;
        Addr get_addr(const Mem::const_iterator& set_it,
                      const Set::const_iterator& page_it,
                      const Page::const_iterator& byte_it) const;

        bool check( Addr addr) const;
        Byte read_byte( Addr addr) const;
        Byte check_and_read_byte( Addr addr) const;

        void alloc( Addr addr);
        void write_byte( Addr addr, Byte value);
        void alloc_and_write_byte( Addr addr, Byte value);
        void memcpy_host_to_guest( Addr dst, const Byte* src, size_t size);
        
        void load_elf_section( const ELFIO::section* section);
};

#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H
