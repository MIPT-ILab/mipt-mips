/**
 * Unit tests for CEN64 memory model
 * Copyright 2012-2018 MIPT-MIPS iLab project
 */
 
#include <catch.hpp>

#include "../cen64_memory.h"
#include "../../t/check_coherency.h"
#include <memory/elf/elf_loader.h>
#include <memory/memory.h>

// The idea is mock CEN64 using our casual FuncMemory implementation
struct bus_controller
{
    bus_controller() : memory( FuncMemory::create_hierarchied_memory()) {}
    const std::shared_ptr<FuncMemory> memory;
};

extern "C" 
{
    int bus_read_word(const bus_controller *bus, uint32_t address, uint32_t *word)
    {
        *word = bus->memory->read<uint32, Endian::big>( address);
        return bytewidth<uint32>;
    }

    int bus_write_word(bus_controller *bus, uint32_t address, uint32_t word, uint32_t dqm)
    {
        bus->memory->masked_write<uint32, Endian::big>( address, word, dqm);
        return bytewidth<uint32>;
    }
}

TEST_CASE( "CEN64Memory: nullptr" )
{
    auto cen64_memory = create_cen64_memory( nullptr);
}

/*
TEST_CASE( "CEN64Memory" )
{
    static const std::string valid_elf_file = TEST_DATA_PATH "mips_bin_exmpl.out";
    static const uint64 dataSectAddr = 0x4100c0;
    
    auto golden_memory = FuncMemory::create_hierarchied_memory();
    bus_controller bus;
    auto cen64_memory = create_cen64_memory(&bus);

    ElfLoader( valid_elf_file, -0x400000).load_to( golden_memory.get());
    golden_memory->duplicate_to( bus.memory);
    
    check_coherency( golden_memory.get(), cen64_memory.get(), dataSectAddr - 0x400000);
}
*/
