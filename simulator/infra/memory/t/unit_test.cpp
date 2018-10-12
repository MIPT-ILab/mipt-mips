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
    CHECK( func_mem.read<uint32, Endian::little>( dataSectAddr) == 0x03020100);
    CHECK( func_mem.read<uint32, Endian::big>( dataSectAddr) == 0x010203);

    // read 3 bytes from the func_mem start addr + 1
    CHECK( func_mem.read<uint32, Endian::little>( dataSectAddr + 1, 0xFFFFFFull) == 0x030201);
    CHECK( func_mem.read<uint32, Endian::big>( dataSectAddr + 1, 0xFFFFFF00ull) == 0x01020300);

    // read 2 bytes from the func_mem start addr + 2
    CHECK( func_mem.read<uint16, Endian::little>( dataSectAddr + 2) == 0x0302);
    CHECK( func_mem.read<uint16, Endian::big>( dataSectAddr + 2) == 0x0203);

    // read 1 bytes from the func_mem start addr + 3
    CHECK( func_mem.read<uint8, Endian::little>( dataSectAddr + 3) == 0x03);
    CHECK( func_mem.read<uint8, Endian::big>( dataSectAddr + 3) == 0x03);

    // check hadling the situation when read
    // from not initialized or written data
    CHECK( func_mem.read<uint8, Endian::little>( 0x300000) == NO_VAL8);
}

TEST_CASE( "Func_memory: Write_Read_Initialized_Mem_Test")
{
    FuncMemory func_mem;
    ::load_elf_file( &func_mem, valid_elf_file);

    // write 1 into the byte pointed by dataSectAddr
    func_mem.write<uint8, Endian::little>( 1, dataSectAddr);
    uint64 right_ret = 0x03020101; // before write it was 0x03020100
    CHECK( func_mem.read<uint32, Endian::little>( dataSectAddr) == right_ret);

    // write 0x7777 into the two bytes pointed by ( dataSectAddr + 1)
    func_mem.write<uint16, Endian::little>( 0x7777, dataSectAddr + 1);
    right_ret = 0x03777701; // before write it was 0x03020101
    CHECK( func_mem.read<uint32, Endian::little>( dataSectAddr) == right_ret);

    // write 0x00000000 into the four bytes pointed by dataSectAddr
    func_mem.write<uint32, Endian::little>( 0x00000000, dataSectAddr, 0xFFFFFFFFull);
    right_ret = 0x00000000; // before write it was 0x03777701
    CHECK( func_mem.read<uint32, Endian::little>( dataSectAddr) == right_ret);
}

TEST_CASE( "Func_memory: Write_Read_Not_Initialized_Mem_Test")
{
    FuncMemory func_mem;
    ::load_elf_file( &func_mem, valid_elf_file);

    uint64 write_addr = 0x3FFFFE;

    // write 0x03020100 into the four bytes pointed by write_addr
    func_mem.write<uint32, Endian::little>( 0x03020100, write_addr);
    uint64 right_ret = 0x0100;
    CHECK( func_mem.read<uint16, Endian::little>( write_addr) == right_ret);

    right_ret = 0x0201;
    CHECK( func_mem.read<uint16, Endian::little>( write_addr + 1) == right_ret);

    right_ret = 0x0302;
    CHECK( func_mem.read<uint16, Endian::little>( write_addr + 2) == right_ret);
}

TEST_CASE( "Func_memory: Host_Guest_Memcpy_1b")
{
    FuncMemory func_mem;

    // Single byte
    const Byte write_data_1{ 0xA5};
    Byte read_data_1{ 0xFF};

    // Write
    CHECK( func_mem.memcpy_host_to_guest_noexcept( dataSectAddr, &write_data_1, 1) == 1);

    // Check if read correctly
    CHECK( func_mem.read<uint8, Endian::little>( dataSectAddr) == static_cast<uint8>( write_data_1));
    CHECK( func_mem.memcpy_guest_to_host_noexcept( &read_data_1, dataSectAddr, 1) == 1);
    CHECK( read_data_1 == write_data_1);
}

TEST_CASE( "Func_memory: Host_Guest_Memcpy_8b")
{
    FuncMemory func_mem;

    // 8 bytes
    const constexpr size_t size = 8;
    const std::array<Byte, size> write_data_8 = {{Byte{0x11}, Byte{0x22}, Byte{0x33}, Byte{0x44}, Byte{0x55}, Byte{0x66}, Byte{0x77}, Byte{0x88}}};
    std::array<Byte, size> read_data_8{};
    read_data_8.fill(Byte{ 0xFF});

    // Write
    CHECK( func_mem.memcpy_host_to_guest_noexcept( dataSectAddr, write_data_8.data(), size) == size);

    // Check if read correctlly
    for (size_t i = 0; i < size; i++)
        CHECK( func_mem.read<uint8, Endian::little>( dataSectAddr + i) == uint8( write_data_8.at(i)));

    CHECK( func_mem.memcpy_guest_to_host_noexcept( read_data_8.data(), dataSectAddr, size) == size);
    for (size_t i = 0; i < size; i++)
        CHECK( read_data_8.at(i) == write_data_8.at(i));

}

TEST_CASE( "Func_memory: Host_Guest_Memcpy_1024b")
{
    FuncMemory func_mem;

    // 1 KByte
    const  constexpr size_t size = 1024;
    std::array<Byte, size> write_data_1024{};
    for (size_t i = 0; i < size; i++)
        write_data_1024.at(i) = Byte( i & 0xFFu);

    std::array<Byte, size> read_data_1024{};
    read_data_1024.fill(Byte( 0xFF));

    CHECK( func_mem.memcpy_host_to_guest_noexcept( dataSectAddr, write_data_1024.data(), size) == size);
    for (size_t i = 0; i < size; i++)
        CHECK( func_mem.read<uint8, Endian::little>( dataSectAddr + i) == uint8( write_data_1024.at( i)));

    CHECK( func_mem.memcpy_guest_to_host_noexcept( read_data_1024.data(), dataSectAddr, size) == size);
    for (size_t i = 0; i < size; i++)
        CHECK( read_data_1024.at(i) == write_data_1024.at(i));
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

TEST_CASE( "Func_memory: Duplicate")
{
    FuncMemory mem1;
    FuncMemory mem2( 48, 15, 10);
    ::load_elf_file( &mem1, valid_elf_file);
    mem1.duplicate_to( &mem2);
    
    CHECK( mem1.dump() == mem2.dump());

    CHECK( mem1.read<uint32, Endian::little>( dataSectAddr) == mem2.read<uint32, Endian::little>( dataSectAddr));
    CHECK( mem1.read<uint32, Endian::little>( dataSectAddr + 1, 0xFFFFFFull) == mem2.read<uint32, Endian::little>( dataSectAddr + 1, 0xFFFFFFull));
    CHECK( mem1.read<uint32, Endian::little>( dataSectAddr + 2, 0xFFFFull) == mem2.read<uint32, Endian::little>( dataSectAddr + 2, 0xFFFFull));
    CHECK( mem1.read<uint16, Endian::little>( dataSectAddr + 2) == mem2.read<uint16, Endian::little>( dataSectAddr + 2));
    CHECK( mem1.read<uint8, Endian::little>( dataSectAddr + 3) == mem2.read<uint8, Endian::little>( dataSectAddr + 3));
    CHECK( mem1.read<uint8, Endian::little>( 0x300000) == mem2.read<uint8, Endian::little>( 0x300000));
    
    mem1.write<uint8, Endian::little>( 1, dataSectAddr);
    CHECK( mem1.read<uint32, Endian::little>( dataSectAddr) != mem2.read<uint32, Endian::little>( dataSectAddr));

    mem2.write<uint8, Endian::little>( 1, dataSectAddr);
    CHECK( mem1.read<uint32, Endian::little>( dataSectAddr) == mem2.read<uint32, Endian::little>( dataSectAddr));

    mem1.write<uint16, Endian::little>( 0x7777, dataSectAddr + 1);
    CHECK( mem1.read<uint32, Endian::little>( dataSectAddr) != mem2.read<uint32, Endian::little>( dataSectAddr));

    mem2.write<uint16, Endian::little>( 0x7777, dataSectAddr + 1);
    CHECK( mem1.read<uint32, Endian::little>( dataSectAddr) == mem2.read<uint32, Endian::little>( dataSectAddr));

    mem1.write<uint32, Endian::little>( 0x00000000, dataSectAddr, 0xFFFFFFFFull);
    mem2.write<uint32, Endian::little>( 0x00000000, dataSectAddr, 0xFFFFFFFFull);

    CHECK( mem1.read<uint32, Endian::little>( dataSectAddr) == mem1.read<uint32, Endian::little>( dataSectAddr));
}
