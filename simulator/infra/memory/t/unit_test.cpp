/**
 * Unit tests for module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012-2018 MIPT-MIPS iLab project
 */
// Catch2
#include <catch.hpp>

// MIPT-MIPS modules
#include "../elf/elf_loader.h"
#include "../memory.h"

static const std::string valid_elf_file = TEST_DATA_PATH "mips_bin_exmpl.out";
// the address of the ".data" section
static const uint64 dataSectAddr = 0x4100c0;

//
// Check that all incorect input params of the constructor
// are properly handled.
//
TEST_CASE( "Func_memory_init: Process_Wrong_Args_Of_Constr")
{
    CHECK_NOTHROW( FuncMemory( )); // check memory initialization with default parameters
    CHECK_NOTHROW( FuncMemory( 48, 15, 10)); // check memory initialization with custom parameters
    CHECK_THROWS_AS( FuncMemory( 64, 15, 32), FuncMemoryBadMapping); // check memory initialization with 4GB bytes page
    CHECK_THROWS_AS( FuncMemory( 48, 32, 10), FuncMemoryBadMapping); // check memory initialization with 4GB pages set
    CHECK_THROWS_AS( FuncMemory( 48,  6, 10), FuncMemoryBadMapping); // check memory initialization with 4GB sets
}

TEST_CASE( "Func_memory_init: Process_Correct_ElfInit")
{
    FuncMemory mem;
    CHECK_NOTHROW( ::load_elf_file( &mem, valid_elf_file));
}

TEST_CASE( "Func_memory_init: Process_Correct_ElfInit custom mapping")
{
    FuncMemory mem( 48, 15, 10);
    CHECK_NOTHROW( ::load_elf_file( &mem, valid_elf_file));
}

TEST_CASE( "Func_memory_init: Process_Wrong_ElfInit")
{
    // test behavior when the file name does not exist
    const std::string wrong_file_name = "./1234567890/qwertyuiop";
    // must exit and return EXIT_FAILURE
    FuncMemory mem;
    CHECK_THROWS_AS( load_elf_file( &mem, wrong_file_name), InvalidElfFile);
}

TEST_CASE( "Func_memory: StartPC_Method_Test")
{
    FuncMemory func_mem;
    ::load_elf_file( &func_mem, valid_elf_file);

    CHECK( func_mem.startPC() == 0x4000b0u /*address of the ".text" section*/);
}

TEST_CASE( "Func_memory: Read_Method_Test")
{
    FuncMemory func_mem;
    ::load_elf_file( &func_mem, valid_elf_file);

    // read 4 bytes from the func_mem start addr
    uint64 right_ret = 0x03020100;
    CHECK( func_mem.read<uint32>( dataSectAddr) == right_ret);

    // read 3 bytes from the func_mem start addr + 1
    right_ret = 0x030201;
    CHECK( func_mem.read<uint32>( dataSectAddr + 1, 0xFFFFFFull) == right_ret);

    // read 2 bytes from the func_mem start addr + 2
    right_ret = 0x0302;
    CHECK( func_mem.read<uint32>( dataSectAddr + 2, 0xFFFFull) == right_ret);
    CHECK( func_mem.read<uint16>( dataSectAddr + 2) == right_ret);

    // read 1 bytes from the func_mem start addr + 3
    right_ret = 0x03;
    CHECK( func_mem.read<uint8>( dataSectAddr + 3) == right_ret);

    // check hadling the situation when read
    // from not initialized or written data
    CHECK( func_mem.read<uint8>( 0x300000) == NO_VAL8);
}

TEST_CASE( "Func_memory: Write_Read_Initialized_Mem_Test")
{
    FuncMemory func_mem;
    ::load_elf_file( &func_mem, valid_elf_file);

    // write 1 into the byte pointed by dataSectAddr
    func_mem.write<uint8>( 1, dataSectAddr);
    uint64 right_ret = 0x03020101; // before write it was 0x03020100
    CHECK( func_mem.read<uint32>( dataSectAddr) == right_ret);

    // write 0x7777 into the two bytes pointed by ( dataSectAddr + 1)
    func_mem.write<uint16>( 0x7777, dataSectAddr + 1);
    right_ret = 0x03777701; // before write it was 0x03020101
    CHECK( func_mem.read<uint32>( dataSectAddr) == right_ret);

    // write 0x00000000 into the four bytes pointed by dataSectAddr
    func_mem.write<uint32>( 0x00000000, dataSectAddr, 0xFFFFFFFFull);
    right_ret = 0x00000000; // before write it was 0x03777701
    CHECK( func_mem.read<uint32>( dataSectAddr) == right_ret);
}

TEST_CASE( "Func_memory: Write_Read_Not_Initialized_Mem_Test")
{
    FuncMemory func_mem;
    ::load_elf_file( &func_mem, valid_elf_file);

    uint64 write_addr = 0x3FFFFE;

    // write 0x03020100 into the four bytes pointed by write_addr
    func_mem.write<uint32>( 0x03020100, write_addr);
    uint64 right_ret = 0x0100;
    CHECK( func_mem.read<uint16>( write_addr) == right_ret);

    right_ret = 0x0201;
    CHECK( func_mem.read<uint16>( write_addr + 1) == right_ret);

    right_ret = 0x0302;
    CHECK( func_mem.read<uint16>( write_addr + 2) == right_ret);
}

TEST_CASE( "Func_memory: Host_Guest_Memcpy_1b")
{
    FuncMemory func_mem;

    // Single byte
    const Byte write_data_1 = Byte( 0xA5);
    Byte read_data_1 = Byte( 0xFF);
    CHECK( func_mem.memcpy_host_to_guest_noexcept( dataSectAddr, &write_data_1, 1) == 1);
    CHECK( func_mem.read<uint8>( dataSectAddr) == static_cast<uint8>( write_data_1));
    CHECK( func_mem.memcpy_guest_to_host_noexcept( &read_data_1, dataSectAddr, 1) == 1);
    CHECK( read_data_1 == write_data_1);

}

TEST_CASE( "Func_memory: Host_Guest_Memcpy_8b")
{
    FuncMemory func_mem;

    // 8 bytes
    const size_t size = 8;
    const uint8 write_data_8[size] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    uint8 read_data_8[size] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    CHECK( func_mem.memcpy_host_to_guest_noexcept( dataSectAddr, reinterpret_cast<const Byte*>(write_data_8),
                                                   size) == size);
    for (size_t i = 0; i < size; i++)
        CHECK( func_mem.read<uint8>( dataSectAddr + i) == write_data_8[i]);
    CHECK( func_mem.memcpy_guest_to_host_noexcept( reinterpret_cast<Byte *>( read_data_8), dataSectAddr,
                                                   size) == size);
    for (size_t i = 0; i < size; i++)
        CHECK( read_data_8[i] == write_data_8[i]);

}

TEST_CASE( "Func_memory: Host_Guest_Memcpy_1024b")
{
    FuncMemory func_mem;

    // 1 KByte
    const size_t size = 1024;
    uint8 write_data_1024[size], read_data_1024[size];
    for (size_t i = 0; i < size; i++) {
        write_data_1024[i] = static_cast<uint8>( i & 0xFF);
        read_data_1024[i] = 0xFF;
    }
    CHECK( func_mem.memcpy_host_to_guest_noexcept( dataSectAddr, reinterpret_cast<const Byte*>(write_data_1024),
                                                   size) == size);
    for (size_t i = 0; i < size; i++)
        CHECK( func_mem.read<uint8>( dataSectAddr + i) == write_data_1024[i]);
    CHECK( func_mem.memcpy_guest_to_host_noexcept( reinterpret_cast<Byte *>( read_data_1024), dataSectAddr,
                                                   size) == size);
    for (size_t i = 0; i < size; i++)
        CHECK( read_data_1024[i] == write_data_1024[i]);
}

TEST_CASE( "Func_memory: Dump")
{
    FuncMemory func_mem;
    ::load_elf_file( &func_mem, valid_elf_file);

    CHECK( func_mem.dump() ==
        "addr 0x400095: data 0xc\n"
        "addr 0x4000a8: data 0x70\n"
        "addr 0x4000a9: data 0x81\n"
        "addr 0x4000aa: data 0x41\n"
        "addr 0x4000b0: data 0x41\n"
        "addr 0x4000b2: data 0xb\n"
        "addr 0x4000b3: data 0x3c\n"
        "addr 0x4000b4: data 0xcc\n"
        "addr 0x4000b6: data 0x6b\n"
        "addr 0x4000b7: data 0x25\n"
        "addr 0x4000b8: data 0x4\n"
        "addr 0x4000ba: data 0x6a\n"
        "addr 0x4000bb: data 0x8d\n"
        "addr 0x4100c1: data 0x1\n"
        "addr 0x4100c2: data 0x2\n"
        "addr 0x4100c3: data 0x3\n"
        "addr 0x4100c4: data 0x4\n"
        "addr 0x4100c5: data 0x5\n"
        "addr 0x4100c6: data 0x6\n"
        "addr 0x4100c7: data 0x7\n"
        "addr 0x4100c8: data 0x8\n"
        "addr 0x4100c9: data 0x9\n"
        "addr 0x4100cc: data 0x7\n"
        "addr 0x4100d0: data 0xb\n"
        "addr 0x4100d4: data 0xd\n"
    );
}

TEST_CASE( "Func_memory: Invariancy")
{
    FuncMemory mem1;
    FuncMemory mem2( 48, 15, 10);
    ::load_elf_file( &mem1, valid_elf_file);
    ::load_elf_file( &mem2, valid_elf_file);
    
    CHECK( mem1.dump() == mem2.dump());

    CHECK( mem1.read<uint32>( dataSectAddr) == mem2.read<uint32>( dataSectAddr));
    CHECK( mem1.read<uint32>( dataSectAddr + 1, 0xFFFFFFull) == mem2.read<uint32>( dataSectAddr + 1, 0xFFFFFFull));
    CHECK( mem1.read<uint32>( dataSectAddr + 2, 0xFFFFull) == mem2.read<uint32>( dataSectAddr + 2, 0xFFFFull));
    CHECK( mem1.read<uint16>( dataSectAddr + 2) == mem2.read<uint16>( dataSectAddr + 2));
    CHECK( mem1.read<uint8>( dataSectAddr + 3) == mem2.read<uint8>( dataSectAddr + 3));
    CHECK( mem1.read<uint8>( 0x300000) == mem2.read<uint8>( 0x300000));
    
    mem1.write<uint8>( 1, dataSectAddr);
    CHECK( mem1.read<uint32>( dataSectAddr) != mem2.read<uint32>( dataSectAddr));

    mem2.write<uint8>( 1, dataSectAddr);
    CHECK( mem1.read<uint32>( dataSectAddr) == mem2.read<uint32>( dataSectAddr));

    mem1.write<uint16>( 0x7777, dataSectAddr + 1);
    CHECK( mem1.read<uint32>( dataSectAddr) != mem2.read<uint32>( dataSectAddr));

    mem2.write<uint16>( 0x7777, dataSectAddr + 1);
    CHECK( mem1.read<uint32>( dataSectAddr) == mem2.read<uint32>( dataSectAddr));

    mem1.write<uint32>( 0x00000000, dataSectAddr, 0xFFFFFFFFull);
    mem2.write<uint32>( 0x00000000, dataSectAddr, 0xFFFFFFFFull);

    CHECK( mem1.read<uint32>( dataSectAddr) == mem1.read<uint32>( dataSectAddr));
}
