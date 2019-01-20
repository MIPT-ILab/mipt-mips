/*
 * cen64_stub.cpp - memory interface between MIPT-MIPS and CEN64
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include <memory/memory.h>
#include <memory>

// Mock CEN64 with our casual FuncMemory implementation
struct bus_controller
{
    bus_controller() : memory( FuncMemory::create_hierarchied_memory()) {}
    const std::shared_ptr<FuncMemory> memory;
};

std::unique_ptr<bus_controller> { return std::make_shared<bus_controller>(); }

extern "C" 
{
    int bus_read_word(const bus_controller *bus, uint32_t address, uint32_t *word)
    {
        *word = bus->memory->read<uint32, Endian::big>( address);
        return bytewidth<uint32>;
    }

    int bus_write_word(bus_controller *bus, uint32_t address, uint32_t word, uint32_t dqm)
    {
        bus->memory->masked_write<uint32, Endian::big>( word, address, dqm);
        return bytewidth<uint32>;
    }
}
