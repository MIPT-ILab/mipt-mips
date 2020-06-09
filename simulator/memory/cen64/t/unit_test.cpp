/**
 * Unit tests for CEN64 memory model
 * Copyright 2012-2018 MIPT-MIPS iLab project
 */
 
#include <catch.hpp>

#include <memory/cen64/cen64_memory.h>
#include <memory/elf/elf_loader.h>
#include <memory/memory.h>
#include <memory/t/check_coherency.h>

// Mock CEN64 with our casual FuncMemory implementation
struct bus_controller
{
    bus_controller() : memory( FuncMemory::create_default_hierarchied_memory()) {}
    const std::shared_ptr<FuncMemory> memory;
};

extern "C" 
{
    int bus_read_word(const bus_controller *bus, uint32_t address, uint32_t *word)
    {
        *word = bus->memory->read<uint32, std::endian::big>( address);
        return bytewidth<uint32>;
    }

    int bus_write_word(bus_controller *bus, uint32_t address, uint32_t word, uint32_t dqm)
    {
        bus->memory->masked_write<uint32, std::endian::big>( word, address, dqm);
        return bytewidth<uint32>;
    }
}

TEST_CASE( "bus_controller mock" )
{
    bus_controller bus;
    uint32 data = 0;

    bus_write_word( &bus, 0x153, 0x22334455, all_ones<uint32>());
    bus_read_word( &bus, 0x153, &data);

    CHECK( data == 0x22334455);
}

TEST_CASE( "CEN64Memory: nullptr" )
{
    CHECK_NOTHROW( create_cen64_memory( nullptr) );
}

TEST_CASE( "CEN64Memory: unsupported" )
{
    bus_controller bus;
    auto cen64_memory = create_cen64_memory(&bus);
    CHECK_THROWS_AS( cen64_memory->dump(), CEN64MemoryUnsupportedInterface);
    CHECK_THROWS_AS( cen64_memory->duplicate_to( FuncMemory::create_4M_plain_memory()), CEN64MemoryUnsupportedInterface);
    CHECK_THROWS_AS( cen64_memory->strlen( 0x0), CEN64MemoryUnsupportedInterface);
}

TEST_CASE( "CEN64Memory: write word" )
{
    bus_controller bus;
    auto cen64_memory = create_cen64_memory(&bus);
    cen64_memory->write<uint32, std::endian::little>( 0x33445566, 0x11);
    CHECK( 0x33445566 == cen64_memory->read<uint32, std::endian::little>( 0x11));
}

TEST_CASE( "CEN64Memory: write byte" )
{
    bus_controller bus;
    auto cen64_memory = create_cen64_memory(&bus);
    cen64_memory->write<uint8, std::endian::little>( 0xAB, 0x11);
    CHECK( 0xAB == cen64_memory->read<uint8, std::endian::little>( 0x11));
}

TEST_CASE( "CEN64Memory" )
{
    static const std::string valid_elf_file = TEST_PATH "/elf/mips_bin_exmpl.out";
    static const uint64 dataSectAddr = 0x4100c0;

    auto golden_memory = FuncMemory::create_default_hierarchied_memory();
    bus_controller bus;
    auto cen64_memory = create_cen64_memory(&bus);

    ElfLoader( valid_elf_file).load_to( golden_memory.get(), -0x400000);
    golden_memory->duplicate_to( bus.memory);
    
    check_coherency( golden_memory.get(), cen64_memory.get(), dataSectAddr - 0x400000);
}
