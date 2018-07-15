/**
 * memory.h - Header of module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// protection from multi-include
#ifndef FUNC_MEMORY__FUNC_MEMORY_H
#define FUNC_MEMORY__FUNC_MEMORY_H

// Generic C++
#include <string>
#include <vector>

// uArchSim modules
#include <infra/types.h>
#include <infra/macro.h>
#include <infra/elf_parser/elf_parser.h>

struct FuncMemoryBadMapping final : std::exception
{
    const std::string message;
    FuncMemoryBadMapping(std::string msg) : message(std::move(msg)) {}
    char const * what() const final noexcept {
        using namespace std::literals::string_literals;
        return ("Invalid FuncMemory mapping: "s + message).c_str();
    }
};

class FuncMemory
{
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

        using Page = std::vector<uint8>;
        using Set  = std::vector<Page>;
        using Mem  = std::vector<Set>;
        Mem memory = {};
        Addr startPC_addr = NO_VAL32;

        inline size_t get_set( Addr addr) const
        {
            return ( addr & set_mask) >> ( page_bits + offset_bits);
        }

        inline size_t get_page( Addr addr) const
        {
            return ( addr & page_mask) >> offset_bits;
        }

        inline size_t get_offset( Addr addr) const
        {
            return ( addr & offset_mask);
        }

        inline Addr get_addr( Addr set, Addr page, Addr offset) const
        {
            return (set << (page_bits + offset_bits)) | (page << offset_bits) | offset;
        }

        inline uint8 read_byte( Addr addr) const
        {
            return memory[get_set(addr)][get_page(addr)][get_offset(addr)];
        }

        inline void write_byte( Addr addr, uint8 value)
        {
            memory[get_set(addr)][get_page(addr)][get_offset(addr)] = value;
        }

        void alloc( Addr addr);
        bool check( Addr addr) const;
    public:
        explicit FuncMemory ( const std::string& executable_file_name,
                     uint32 addr_bits = 32,
                     uint32 page_bits = 10,
                     uint32 offset_bits = 12);

        template<typename T>
        T read( Addr addr, T mask = all_ones<T>()) const;

        template<typename T>
        void write( T value, Addr addr, T mask = all_ones<T>());

        inline uint64 startPC() const { return startPC_addr; }
        std::string dump() const;
};

#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H
