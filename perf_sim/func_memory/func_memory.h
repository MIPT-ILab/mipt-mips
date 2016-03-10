/**
 * func_memory.h - Header of module implementing the concept of 
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// protection from multi-include
#ifndef FUNC_MEMORY__FUNC_MEMORY_H
#define FUNC_MEMORY__FUNC_MEMORY_H

// Generic C++
#include <string>
#include <iostream>
#include <cassert>

// uArchSim modules
#include <types.h>
#include <elf_parser.h>

class FuncMemory
{
    private:
        uint8*** memory;
        uint64 startPC_addr;
    
        uint64 addr_bits;
        uint64 set_bits;
        uint64 page_bits;
        uint64 offset_bits;
        
        uint64 set_mask;
        uint64 page_mask;
        uint64 offset_mask;        
        
        inline size_t get_set( uint64 addr) const
        {
            return ( addr & set_mask) >> ( page_bits + offset_bits);
        }
        
        inline size_t get_page( uint64 addr) const
        {
            return ( addr & page_mask) >> offset_bits;
        }

        inline size_t get_offset( uint64 addr) const
        {
            return ( addr & offset_mask);
        }
        
        inline uint64 get_addr( uint64 set, uint64 page, uint64 offset) const
        {
            return (set << (page_bits + offset_bits)) | (page << offset_bits) | offset;
        }
        
        inline uint8* get_host_addr( uint64 addr) const
        {
            return &memory[get_set(addr)][get_page(addr)][get_offset(addr)];
        }

        inline uint8 read_byte( uint64 addr) const
        {
            return *get_host_addr(addr);
        }
        
        inline void write_byte( uint64 addr, uint8 value)
        {
           *get_host_addr(addr) = value;
        }
        
        void alloc( uint64 addr);
        bool check( uint64 addr) const;

    public:
        FuncMemory ( const char* executable_file_name,
                     uint64 addr_size = 32,
                     uint64 page_num_size = 10,
                     uint64 offset_size = 12);
        virtual ~FuncMemory();
        uint64 read( uint64 addr, unsigned short num_of_bytes = 4) const;
        void write( uint64 value, uint64 addr, unsigned short num_of_bytes = 4);
        inline uint64 startPC() const { return startPC_addr; }
        std::string dump( string indent = "") const;
};

#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H
