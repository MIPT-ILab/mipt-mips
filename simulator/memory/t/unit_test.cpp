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
#include "check_coherency.h"

static const std::string valid_elf_file = TEST_DATA_PATH "mips_bin_exmpl.out";
// the address of the ".data" section
static const uint64 dataSectAddr = 0x4100c0;
static const uint64 dataSectAddrShifted = 0x100c0;

//
// Check that all incorect input params of the constructor
// are properly handled.
//
TEST_CASE( "Func_memory_init: Process_Wrong_Args_Of_Constr")
{
    CHECK_NOTHROW( FuncMemory::create_hierarchied_memory( )); // check memory initialization with default parameters
    CHECK_NOTHROW( FuncMemory::create_hierarchied_memory( 48, 15, 10)); // check memory initialization with custom parameters
    CHECK_THROWS_AS( FuncMemory::create_hierarchied_memory( 64, 15, 32), FuncMemoryBadMapping); // check memory initialization with 4GB bytes page
    CHECK_THROWS_AS( FuncMemory::create_hierarchied_memory( 48, 32, 10), FuncMemoryBadMapping); // check memory initialization with 4GB pages set
    CHECK_THROWS_AS( FuncMemory::create_hierarchied_memory( 48,  6, 10), FuncMemoryBadMapping); // check memory initialization with 4GB sets
}

TEST_CASE( "Func_memory_init: Process_Correct_ElfInit")
{
    auto ptr = FuncMemory::create_hierarchied_memory();
    CHECK_NOTHROW( ElfLoader( valid_elf_file).load_to( ptr.get()));
}

TEST_CASE( "Func_memory_init: Process_Correct_ElfInit custom mapping")
{
    auto ptr = FuncMemory::create_hierarchied_memory( 48, 15, 10);
    CHECK_NOTHROW( ElfLoader( valid_elf_file).load_to( ptr.get()));
}

TEST_CASE( "Func_memory_init: Process_Wrong_ElfInit")
{
    // test behavior when the file name does not exist
    CHECK_THROWS_AS( ElfLoader( "./1234567890/qwertyuiop"), InvalidElfFile);
}

TEST_CASE( "Func_memory: StartPC_Method_Test")
{
    CHECK( ElfLoader( valid_elf_file).get_startPC() == 0x4000b0u /*address of the ".text" section*/);
}

TEST_CASE( "Plain memory: out of range")
{
    std::array<Byte, 16> arr{};
    auto ptr = FuncMemory::create_plain_memory( 10);
    CHECK_THROWS_AS( ptr->memcpy_host_to_guest( 0xFF0000, arr.data(), 16), FuncMemoryOutOfRange );
    CHECK_THROWS_AS( ptr->memcpy_host_to_guest( 0x3fc, arr.data(), 16), FuncMemoryOutOfRange );
    CHECK_THROWS_AS( ptr->memcpy_host_to_guest( 0x0, arr.data(), 2048), FuncMemoryOutOfRange );

    CHECK( ptr->memcpy_host_to_guest_noexcept( 0x0, arr.data(), 2048) == 0 );
}

TEST_CASE( "Hierarchied memory: out of range")
{
    std::array<Byte, 16> arr{};
    auto ptr = FuncMemory::create_hierarchied_memory( 10, 3, 4);
    CHECK_THROWS_AS( ptr->memcpy_host_to_guest( 0xFF0000, arr.data(), 16), FuncMemoryOutOfRange );
    CHECK_THROWS_AS( ptr->memcpy_host_to_guest( 0x3fc, arr.data(), 16), FuncMemoryOutOfRange );
    CHECK_THROWS_AS( ptr->memcpy_host_to_guest( 0x0, arr.data(), 2048), FuncMemoryOutOfRange );
}

TEST_CASE( "Func_memory: Read_Method_Test")
{
    auto func_mem = FuncMemory::create_hierarchied_memory();
    ElfLoader( valid_elf_file).load_to( func_mem.get());

    // read 4 bytes from the func_mem start addr
    CHECK( func_mem->read<uint32, Endian::little>( dataSectAddr) == 0x03020100);
    CHECK( func_mem->read<uint32, Endian::big>( dataSectAddr) == 0x010203);

    // read 3 bytes from the func_mem start addr + 1
    CHECK( func_mem->read<uint32, Endian::little>( dataSectAddr + 1, 0xFFFFFFull) == 0x030201);
    CHECK( func_mem->read<uint32, Endian::big>( dataSectAddr + 1, 0xFFFFFF00ull) == 0x01020300);

    // read 2 bytes from the func_mem start addr + 2
    CHECK( func_mem->read<uint16, Endian::little>( dataSectAddr + 2) == 0x0302);
    CHECK( func_mem->read<uint16, Endian::big>( dataSectAddr + 2) == 0x0203);

    // read 1 bytes from the func_mem start addr + 3
    CHECK( func_mem->read<uint8, Endian::little>( dataSectAddr + 3) == 0x03);
    CHECK( func_mem->read<uint8, Endian::big>( dataSectAddr + 3) == 0x03);

    // check hadling the situation when read
    // from not initialized or written data
    CHECK( func_mem->read<uint8, Endian::little>( 0x300000) == 0);
}

TEST_CASE( "Func_memory: Write_Read_Initialized_Mem_Test")
{
    auto func_mem = FuncMemory::create_hierarchied_memory();
    ElfLoader( valid_elf_file).load_to( func_mem.get());

    // write 1 into the byte pointed by dataSectAddr
    func_mem->write<uint8, Endian::little>( 1, dataSectAddr);
    uint64 right_ret = 0x03020101; // before write it was 0x03020100
    CHECK( func_mem->read<uint32, Endian::little>( dataSectAddr) == right_ret);

    // write 0x7777 into the two bytes pointed by ( dataSectAddr + 1)
    func_mem->write<uint16, Endian::little>( 0x7777, dataSectAddr + 1);
    right_ret = 0x03777701; // before write it was 0x03020101
    CHECK( func_mem->read<uint32, Endian::little>( dataSectAddr) == right_ret);

    // write 0x00000000 into the four bytes pointed by dataSectAddr
    func_mem->write<uint32, Endian::little>( 0x00000000, dataSectAddr);
    right_ret = 0x00000000; // before write it was 0x03777701
    CHECK( func_mem->read<uint32, Endian::little>( dataSectAddr) == right_ret);
}

TEST_CASE( "Func_memory: Write_Read_Not_Initialized_Mem_Test")
{
    auto func_mem = FuncMemory::create_hierarchied_memory();
    ElfLoader( valid_elf_file).load_to( func_mem.get());

    uint64 write_addr = 0x3FFFFE;

    // write 0x03020100 into the four bytes pointed by write_addr
    func_mem->write<uint32, Endian::little>( 0x03020100, write_addr);
    uint64 right_ret = 0x0100;
    CHECK( func_mem->read<uint16, Endian::little>( write_addr) == right_ret);

    right_ret = 0x0201;
    CHECK( func_mem->read<uint16, Endian::little>( write_addr + 1) == right_ret);

    right_ret = 0x0302;
    CHECK( func_mem->read<uint16, Endian::little>( write_addr + 2) == right_ret);
}

TEST_CASE( "Func_memory: Host_Guest_Memcpy_1b")
{
    auto func_mem = FuncMemory::create_hierarchied_memory();

    // Single byte
    const Byte write_data_1{ 0xA5};
    Byte read_data_1{ 0xFF};

    // Write
    CHECK( func_mem->memcpy_host_to_guest_noexcept( dataSectAddr, &write_data_1, 1) == 1);

    // Check if read correctly
    CHECK( func_mem->read<uint8, Endian::little>( dataSectAddr) == uint8{ 0xA5});
    CHECK( func_mem->memcpy_guest_to_host( &read_data_1, dataSectAddr, 1) == 1);
    CHECK( read_data_1 == write_data_1);
}

TEST_CASE( "Func_memory: Host_Guest_Memcpy_8b")
{
    auto func_mem = FuncMemory::create_hierarchied_memory();

    // 8 bytes
    const constexpr size_t size = 8;
    const std::array<Byte, size> write_data_8 = {{Byte{0x11}, Byte{0x22}, Byte{0x33}, Byte{0x44}, Byte{0x55}, Byte{0x66}, Byte{0x77}, Byte{0x88}}};
    std::array<Byte, size> read_data_8{};
    read_data_8.fill(Byte{ 0xFF});

    // Write
    CHECK( func_mem->memcpy_host_to_guest_noexcept( dataSectAddr, write_data_8.data(), size) == size);

    // Check if read correctlly
    for (size_t i = 0; i < size; i++)
        CHECK( func_mem->read<uint8, Endian::little>( dataSectAddr + i) == uint8( write_data_8.at(i)));

    CHECK( func_mem->memcpy_guest_to_host( read_data_8.data(), dataSectAddr, size) == size);
    for (size_t i = 0; i < size; i++)
        CHECK( read_data_8.at(i) == write_data_8.at(i));
}

TEST_CASE( "Func_memory: Host_Guest_Memcpy_1024b")
{
    auto func_mem = FuncMemory::create_hierarchied_memory();

    // 1 KByte
    const  constexpr size_t size = 1024;
    std::array<Byte, size> write_data_1024{};
    for (size_t i = 0; i < size; i++)
        write_data_1024.at(i) = Byte( i & 0xFFu);

    std::array<Byte, size> read_data_1024{};
    read_data_1024.fill(Byte( 0xFF));

    CHECK( func_mem->memcpy_host_to_guest_noexcept( dataSectAddr, write_data_1024.data(), size) == size);
    for (size_t i = 0; i < size; i++)
        CHECK( func_mem->read<uint8, Endian::little>( dataSectAddr + i) == uint8( write_data_1024.at( i)));

    CHECK( func_mem->memcpy_guest_to_host( read_data_1024.data(), dataSectAddr, size) == size);
    for (size_t i = 0; i < size; i++)
        CHECK( read_data_1024.at(i) == write_data_1024.at(i));
}

TEST_CASE( "Func_memory: Dump")
{
    auto func_mem = FuncMemory::create_hierarchied_memory();
    ElfLoader( valid_elf_file).load_to( func_mem.get());

    CHECK( func_mem->dump() ==
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
    auto mem1 = FuncMemory::create_hierarchied_memory();
    auto mem2 = FuncMemory::create_hierarchied_memory( 48, 15, 10);

    ElfLoader( valid_elf_file, -0x400000).load_to( mem1.get());
    mem1->duplicate_to( mem2);

    CHECK( mem1->dump() == mem2->dump());
    check_coherency( mem1.get(), mem2.get(), dataSectAddr - 0x400000);
}

TEST_CASE( "Func_memory: Plain Memory")
{
    auto mem1 = FuncMemory::create_hierarchied_memory();
    auto mem2 = FuncMemory::create_plain_memory( 24);

    ElfLoader( valid_elf_file, -0x400000).load_to( mem1.get());
    mem1->duplicate_to( mem2);

    CHECK( mem1->dump() == mem2->dump());
    check_coherency( mem1.get(), mem2.get(), dataSectAddr - 0x400000);
}

TEST_CASE( "Func_memory: Duplicate Plain Memory")
{
    auto mem1 = FuncMemory::create_plain_memory( 24);
    auto mem2 = FuncMemory::create_hierarchied_memory();

    ElfLoader( valid_elf_file, -0x400000).load_to( mem1.get());
    mem1->duplicate_to( mem2);

    CHECK( mem1->dump() == mem2->dump());
    check_coherency( mem1.get(), mem2.get(), dataSectAddr - 0x400000);
}

TEST_CASE( "Func_memory: ZeroMemory")
{
    ZeroMemory zm;
    CHECK( zm.read<uint32, Endian::big>(0x12355) == 0 );
}

TEST_CASE( "Func_memory: String length in zero memory")
{
    CHECK( ZeroMemory().strlen(0x10) == 0);
}

TEST_CASE( "Func_memory: String length, no zero bytes")
{
    auto mem = FuncMemory::create_hierarchied_memory( 1, 0, 0);
    mem->write<uint16, Endian::big>( 0xABCD, 0x0);
    CHECK( mem->strlen( 0x0) == 2);
}

TEST_CASE( "Func_memory: String length")
{
    const char hw[] = "Hello World!";
    static_assert( countof(hw) == 13);
    auto mem = FuncMemory::create_hierarchied_memory( 24);
    mem->memcpy_host_to_guest( 0x10, byte_cast<13>( hw), countof( hw));
    CHECK( mem->strlen( 0x10) == 12);
    CHECK( mem->strlen( 0x12) == 10);
    CHECK( mem->read_string( 0x10) == "Hello World!");
    CHECK( mem->read_string( 0x12) == "llo World!");
    CHECK( mem->read_string_limited( 0x10, 50) == "Hello World!");
    CHECK( mem->read_string_limited( 0x10, 5) == "Hello");
}

TEST_CASE( "Func_memory: String length, plain memory")
{
    const char hw[] = "Hello World!";
    static_assert( countof(hw) == 13);
    auto mem = FuncMemory::create_plain_memory();
    mem->memcpy_host_to_guest( 0x10, byte_cast<13>( hw), countof( hw));
    CHECK( mem->strlen( 0x10) == 12);
}

TEST_CASE( "Func_memory: Write string")
{
    auto mem = FuncMemory::create_plain_memory();
    mem->write_string( "MIPT-MIPS is cool", 0x20); 
    CHECK( mem->read_string( 0x20) == "MIPT-MIPS is cool");
}

TEST_CASE( "Func_memory: Write string limited")
{
    auto mem = FuncMemory::create_plain_memory();
    mem->write_string_limited( "MIPT-MIPS is cool", 0x20, 9);
    CHECK( mem->read_string( 0x20) == "MIPT-MIPS");
}
