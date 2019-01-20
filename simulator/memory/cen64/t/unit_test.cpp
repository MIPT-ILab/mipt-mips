/**
 * Unit tests for CEN64 memory model
 * Copyright 2012-2018 MIPT-MIPS iLab project
 */
 
#include <catch.hpp>

#include "../../t/check_coherency.h"
#include "../cen64_memory.h"
#include "../cen64_stub.h"
#include <memory/elf/elf_loader.h>
#include <memory/memory.h>

TEST_CASE( "bus_controller mock" )
{
    auto bus = get_bus_controller_stub();
    uint32 data;

    bus_write_word( bus.get(), 0x153, 0x22334455, all_ones<uint32>());
    bus_read_word( bus.get(), 0x153, &data);

    CHECK( data == 0x22334455);
}

TEST_CASE( "CEN64Memory: nullptr" )
{
    CHECK_NOTHROW( create_cen64_memory( nullptr) );
}

TEST_CASE( "CEN64Memory: unsupported" )
{
    auto bus = get_bus_controller_stub();
    auto cen64_memory = create_cen64_memory( bus.get());
    CHECK_THROWS_AS( cen64_memory->dump(), CEN64MemoryUnsupportedInterface);
    CHECK_THROWS_AS( cen64_memory->duplicate_to( FuncMemory::create_plain_memory()), CEN64MemoryUnsupportedInterface);
    CHECK_THROWS_AS( cen64_memory->strlen( 0x0), CEN64MemoryUnsupportedInterface);
}

TEST_CASE( "CEN64Memory: write word" )
{
    auto bus = get_bus_controller_stub();
    auto cen64_memory = create_cen64_memory( bus.get());
    cen64_memory->write<uint32, Endian::little>( 0x33445566, 0x11);
    CHECK( 0x33445566 == cen64_memory->read<uint32, Endian::little>( 0x11));
}

TEST_CASE( "CEN64Memory: write byte" )
{
    auto bus = get_bus_controller_stub();
    auto cen64_memory = create_cen64_memory( bus.get());
    cen64_memory->write<uint8, Endian::little>( 0xAB, 0x11);
    CHECK( 0xAB == cen64_memory->read<uint8, Endian::little>( 0x11));
}

TEST_CASE( "CEN64Memory" )
{
    static const std::string valid_elf_file = TEST_DATA_PATH "mips_bin_exmpl.out";
    static const uint64 dataSectAddr = 0x4100c0;

    auto golden_memory = FuncMemory::create_hierarchied_memory();
    auto bus = get_bus_controller_stub();
    auto cen64_memory = create_cen64_memory( bus.get());

    ElfLoader( valid_elf_file, -0x400000).load_to( golden_memory.get());
    golden_memory->duplicate_to( bus.memory);
    
    check_coherency( golden_memory.get(), cen64_memory.get(), dataSectAddr - 0x400000);
}
