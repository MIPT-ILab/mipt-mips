/**
 * Unit tests for module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012-2018 MIPT-MIPS iLab project
 */
// Catch2
#include <catch.hpp>

// MIPT-MIPS modules
#include <func_sim/operation.h>
#include <memory/elf/elf_loader.h>
#include <memory/memory.h>
#include <memory/t/check_coherency.h>

static const std::string_view valid_elf_file = TEST_PATH "/elf/mips_bin_exmpl.out";
// the address of the ".data" section
static const uint64 dataSectAddr = 0x4100c0;

//
// Check that all incorect input params of the constructor
// are properly handled.
//
TEST_CASE( "Func_memory_init: Process_Wrong_Args_Of_Constr")
{
    CHECK_THROWS_AS( FuncMemory::create_hierarchied_memory( 64, 15, 32), FuncMemoryBadMapping); // check memory initialization with 4GB bytes page
    CHECK_THROWS_AS( FuncMemory::create_hierarchied_memory( 48, 32, 10), FuncMemoryBadMapping); // check memory initialization with 4GB pages set
    CHECK_THROWS_AS( FuncMemory::create_hierarchied_memory( 48,  6, 10), FuncMemoryBadMapping); // check memory initialization with 4GB sets
}

TEST_CASE( "Func_memory_init: Process_Correct_ElfInit")
{
    auto ptr = FuncMemory::create_default_hierarchied_memory();
    CHECK_NOTHROW( ElfLoader( valid_elf_file).load_to( ptr.get()));
}

TEST_CASE( "Func_memory_init: Process_Wrong_ElfInit")
{
    // test behavior when the file name does not exist
    CHECK_THROWS_AS( ElfLoader( "./1234567890/qwertyuiop"), InvalidElfFile);
}

TEST_CASE( "Func_memory_init: Process_Correct_ElfInit_BSS")
{
    auto ptr = FuncMemory::create_default_hierarchied_memory();
    CHECK_NOTHROW( ElfLoader( TEST_PATH "/elf/qsort.riscv").load_to( ptr.get()));
}

TEST_CASE( "Func_memory: StartPC_Method_Test")
{
    CHECK( ElfLoader( valid_elf_file).get_startPC() == 0x4000b0U /*address of the "__start" label*/);
}

TEST_CASE( "Func_memory: StartPC Invalid")
{
    CHECK_THROWS_AS( ElfLoader( TEST_PATH "/elf/nop.bin").get_startPC(), InvalidEntryPoint);
}

TEST_CASE( "Func_memory: Bad section")
{
    auto ptr = FuncMemory::create_default_hierarchied_memory();
    CHECK_THROWS_AS( ElfLoader( TEST_PATH "/elf/empty.bin").load_to( ptr.get()), InvalidElfSection);
    CHECK_THROWS_AS( ElfLoader( TEST_PATH "/elf/empty.bin").get_startPC(), InvalidEntryPoint);
}

TEST_CASE( "Plain memory: out of range")
{
    std::array<std::byte, 16> arr{};
    auto ptr = FuncMemory::create_plain_memory( 10);
    CHECK_THROWS_AS( ptr->memcpy_host_to_guest( 0xFF0000, arr.data(), 16), FuncMemoryOutOfRange );
    CHECK_THROWS_AS( ptr->memcpy_host_to_guest( 0x3fc, arr.data(), 16), FuncMemoryOutOfRange );
    CHECK_THROWS_AS( ptr->memcpy_host_to_guest( 0x0, arr.data(), 2048), FuncMemoryOutOfRange );

    CHECK( ptr->memcpy_host_to_guest_noexcept( 0x0, arr.data(), 2048) == 0 );
}

TEST_CASE( "Hierarchied memory: out of range")
{
    std::array<std::byte, 16> arr{};
    auto ptr = FuncMemory::create_hierarchied_memory( 10, 3, 4);
    CHECK_THROWS_AS( ptr->memcpy_host_to_guest( 0xFF0000, arr.data(), 16), FuncMemoryOutOfRange );
    CHECK_THROWS_AS( ptr->memcpy_host_to_guest( 0x3fc, arr.data(), 16), FuncMemoryOutOfRange );
    CHECK_THROWS_AS( ptr->memcpy_host_to_guest( 0x0, arr.data(), 2048), FuncMemoryOutOfRange );
}

TEST_CASE( "Func_memory: Read_Method_Test")
{
    auto func_mem = FuncMemory::create_default_hierarchied_memory();
    ElfLoader( valid_elf_file).load_to( func_mem.get());

    // read 4 bytes from the func_mem start addr
    CHECK( func_mem->read<uint32, std::endian::little>( dataSectAddr) == 0x03020100);
    CHECK( func_mem->read<uint32, std::endian::big>( dataSectAddr) == 0x010203);

    // read 3 bytes from the func_mem start addr + 1
    CHECK( func_mem->read<uint32, std::endian::little>( dataSectAddr + 1, 0xFFFFFFULL) == 0x030201);
    CHECK( func_mem->read<uint32, std::endian::big>( dataSectAddr + 1, 0xFFFFFF00ULL) == 0x01020300);

    // read 2 bytes from the func_mem start addr + 2
    CHECK( func_mem->read<uint16, std::endian::little>( dataSectAddr + 2) == 0x0302);
    CHECK( func_mem->read<uint16, std::endian::big>( dataSectAddr + 2) == 0x0203);

    // read 1 bytes from the func_mem start addr + 3
    CHECK( func_mem->read<uint8, std::endian::little>( dataSectAddr + 3) == 0x03);
    CHECK( func_mem->read<uint8, std::endian::big>( dataSectAddr + 3) == 0x03);

    // check hadling the situation when read
    // from not initialized or written data
    CHECK( func_mem->read<uint8, std::endian::little>( 0x300000) == 0);
}

TEST_CASE( "Func_memory: Write_Read_Initialized_Mem_Test")
{
    auto func_mem = FuncMemory::create_default_hierarchied_memory();
    ElfLoader( valid_elf_file).load_to( func_mem.get());

    // write 1 into the byte pointed by dataSectAddr
    func_mem->write<uint8, std::endian::little>( 1, dataSectAddr);
    uint64 right_ret = 0x03020101; // before write it was 0x03020100
    CHECK( func_mem->read<uint32, std::endian::little>( dataSectAddr) == right_ret);

    // write 0x7777 into the two bytes pointed by ( dataSectAddr + 1)
    func_mem->write<uint16, std::endian::little>( 0x7777, dataSectAddr + 1);
    right_ret = 0x03777701; // before write it was 0x03020101
    CHECK( func_mem->read<uint32, std::endian::little>( dataSectAddr) == right_ret);

    // write 0x00000000 into the four bytes pointed by dataSectAddr
    func_mem->write<uint32, std::endian::little>( 0x00000000, dataSectAddr);
    right_ret = 0x00000000; // before write it was 0x03777701
    CHECK( func_mem->read<uint32, std::endian::little>( dataSectAddr) == right_ret);
}

TEST_CASE( "Func_memory: Write_Read_Not_Initialized_Mem_Test")
{
    auto func_mem = FuncMemory::create_default_hierarchied_memory();
    ElfLoader( valid_elf_file).load_to( func_mem.get());

    uint64 write_addr = 0x3FFFFE;

    // write 0x03020100 into the four bytes pointed by write_addr
    func_mem->write<uint32, std::endian::little>( 0x03020100, write_addr);
    uint64 right_ret = 0x0100;
    CHECK( func_mem->read<uint16, std::endian::little>( write_addr) == right_ret);

    right_ret = 0x0201;
    CHECK( func_mem->read<uint16, std::endian::little>( write_addr + 1) == right_ret);

    right_ret = 0x0302;
    CHECK( func_mem->read<uint16, std::endian::little>( write_addr + 2) == right_ret);
}

TEST_CASE( "Func_memory: Host_Guest_Memcpy_1b")
{
    auto func_mem = FuncMemory::create_default_hierarchied_memory();

    // Single byte
    const std::byte write_data_1{ 0xA5};
    std::byte read_data_1{ 0xFF};

    // Write
    CHECK( func_mem->memcpy_host_to_guest_noexcept( dataSectAddr, &write_data_1, 1) == 1);

    // Check if read correctly
    CHECK( func_mem->read<uint8, std::endian::little>( dataSectAddr) == uint8{ 0xA5});
    CHECK( func_mem->memcpy_guest_to_host( &read_data_1, dataSectAddr, 1) == 1);
    CHECK( read_data_1 == write_data_1);
}

TEST_CASE( "Func_memory: Host_Guest_Memcpy_8b")
{
    auto func_mem = FuncMemory::create_default_hierarchied_memory();

    // 8 bytes
    const constexpr size_t size = 8;
    const std::array<std::byte, size> write_data_8 = {{std::byte{0x11}, std::byte{0x22}, std::byte{0x33}, std::byte{0x44}, std::byte{0x55}, std::byte{0x66}, std::byte{0x77}, std::byte{0x88}}};
    std::array<std::byte, size> read_data_8{};
    read_data_8.fill(std::byte{ 0xFF});

    // Write
    CHECK( func_mem->memcpy_host_to_guest_noexcept( dataSectAddr, write_data_8.data(), size) == size);

    // Check if read correctlly
    for (size_t i = 0; i < size; i++)
        CHECK( func_mem->read<uint8, std::endian::little>( dataSectAddr + i) == uint8( write_data_8.at(i)));

    CHECK( func_mem->memcpy_guest_to_host( read_data_8.data(), dataSectAddr, size) == size);
    for (size_t i = 0; i < size; i++)
        CHECK( read_data_8.at(i) == write_data_8.at(i));
}

TEST_CASE( "Func_memory: Host_Guest_Memcpy_1024b")
{
    auto func_mem = FuncMemory::create_default_hierarchied_memory();

    // 1 Kstd::byte
    const  constexpr size_t size = 1024;
    std::array<std::byte, size> write_data_1024{};
    for (size_t i = 0; i < size; i++)
        write_data_1024.at(i) = std::byte( i & 0xFFU);

    std::array<std::byte, size> read_data_1024{};
    read_data_1024.fill(std::byte( 0xFF));

    CHECK( func_mem->memcpy_host_to_guest_noexcept( dataSectAddr, write_data_1024.data(), size) == size);
    for (size_t i = 0; i < size; i++)
        CHECK( func_mem->read<uint8, std::endian::little>( dataSectAddr + i) == uint8( write_data_1024.at( i)));

    CHECK( func_mem->memcpy_guest_to_host( read_data_1024.data(), dataSectAddr, size) == size);
    for (size_t i = 0; i < size; i++)
        CHECK( read_data_1024.at(i) == write_data_1024.at(i));
}

TEST_CASE( "Func_memory: Dump")
{
    auto func_mem = FuncMemory::create_default_hierarchied_memory();
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
    auto mem1 = FuncMemory::create_default_hierarchied_memory();
    auto mem2 = FuncMemory::create_default_hierarchied_memory();

    ElfLoader( valid_elf_file).load_to( mem1.get(), -0x400000);
    mem1->duplicate_to( mem2);

    CHECK( mem1->dump() == mem2->dump());
    check_coherency( mem1.get(), mem2.get(), dataSectAddr - 0x400000);
}

TEST_CASE( "Func_memory: Plain Memory")
{
    auto mem1 = FuncMemory::create_default_hierarchied_memory();
    auto mem2 = FuncMemory::create_plain_memory( 24);

    ElfLoader( valid_elf_file).load_to( mem1.get(), -0x400000);
    mem1->duplicate_to( mem2);

    CHECK( mem1->dump() == mem2->dump());
    check_coherency( mem1.get(), mem2.get(), dataSectAddr - 0x400000);
}

TEST_CASE( "Func_memory: Duplicate Plain Memory")
{
    auto mem1 = FuncMemory::create_plain_memory( 24);
    auto mem2 = FuncMemory::create_default_hierarchied_memory();

    ElfLoader( valid_elf_file).load_to( mem1.get(), -0x400000);
    mem1->duplicate_to( mem2);

    CHECK( mem1->dump() == mem2->dump());
    check_coherency( mem1.get(), mem2.get(), dataSectAddr - 0x400000);
}

TEST_CASE( "Func_memory: memset")
{
    auto mem = FuncMemory::create_plain_memory( 24);

    mem->memset( 0x1000, std::byte{'a'}, 16);
    mem->memset( 0x1000, std::byte{'b'}, 8);

    CHECK( mem->read<uint8, std::endian::little>( 0x1000) == 'b');
    CHECK( mem->read<uint8, std::endian::little>( 0x1008) == 'a');
}

TEST_CASE( "Func_memory: ZeroMemory")
{
    auto zm = ReadableMemory::create_zero_memory();
    CHECK( zm->read<uint32, std::endian::big>(0x12355) == 0 );
    CHECK( zm->dump() == "empty memory\n");
}

TEST_CASE( "Func_memory: String length in zero memory")
{
    CHECK( ReadableMemory::create_zero_memory()->strlen(0x10) == 0);
}

TEST_CASE( "Zero_memory: Duplicate")
{
    auto mem1 = FuncMemory::create_plain_memory( 24);
    auto mem2 = FuncMemory::create_plain_memory( 24);

    ElfLoader( valid_elf_file).load_to( mem1.get(), -0x400000);
    mem1->duplicate_to( mem2);
    ReadableMemory::create_zero_memory()->duplicate_to( mem1); // nop

    check_coherency( mem1.get(), mem2.get(), 0);
}

TEST_CASE( "Func_memory: String length, no zero bytes")
{
    auto mem = FuncMemory::create_hierarchied_memory( 1, 0, 0);
    mem->write<uint16, std::endian::big>( 0xABCD, 0x0);
    CHECK( mem->strlen( 0x0) == 2);
}

TEST_CASE( "Func_memory: String length")
{
    const std::string hw("Hello World!");
    auto mem = FuncMemory::create_default_hierarchied_memory();
    mem->memcpy_host_to_guest( 0x10, byte_cast( hw.c_str()), hw.size());
    CHECK( mem->strlen( 0x10) == 12);
    CHECK( mem->strlen( 0x12) == 10);
    CHECK( mem->read_string( 0x10) == "Hello World!");
    CHECK( mem->read_string( 0x12) == "llo World!");
    CHECK( mem->read_string_limited( 0x10, 50) == "Hello World!");
    CHECK( mem->read_string_limited( 0x10, 5) == "Hello");
}

TEST_CASE( "Func_memory: String length, empty memory")
{
    auto mem = FuncMemory::create_default_hierarchied_memory();
    CHECK( mem->strlen( 0x10) == 0);
}

TEST_CASE( "Func_memory: String length, plain memory")
{
    const std::string hw("Hello World!");
    auto mem = FuncMemory::create_4M_plain_memory();
    mem->memcpy_host_to_guest( 0x10, byte_cast( hw.c_str()), hw.size());
    CHECK( mem->strlen( 0x10) == 12);
}

TEST_CASE( "Func_memory: Write string")
{
    auto mem = FuncMemory::create_4M_plain_memory();
    mem->write_string( "MIPT-MIPS is cool", 0x20); 
    CHECK( mem->read_string( 0x20) == "MIPT-MIPS is cool");
}

TEST_CASE( "Func_memory: Write string limited")
{
    auto mem = FuncMemory::create_4M_plain_memory();
    mem->write_string_limited( "MIPT-MIPS is cool", 0x20, 9);
    CHECK( mem->read_string( 0x20) == "MIPT-MIPS");
}

class DummyStore : public Datapath<uint64> {
    public:
        explicit DummyStore( Addr a) : Datapath<uint64>( 0, 0)
        {
            set_type( OUT_STORE);
            mem_addr = a;
            mem_size = 8;
            v_src[1] = 0xABCD'EF12'3456'7890ULL;
        }
        static auto get_endian() { return std::endian::little; } 
};

TEST_CASE( "Func_memory: Store to 0")
{
    DummyStore store( 0);
    auto mem = FuncMemory::create_4M_plain_memory();
    CHECK_THROWS_AS( mem->load_store( &store), Exception);
}

TEST_CASE( "Func_memory: Store to 0x100")
{
    DummyStore store( 0x100);
    auto mem = FuncMemory::create_4M_plain_memory();
    mem->load_store( &store);
    CHECK( mem->read<uint64, std::endian::little>( 0x100) == store.get_v_src( 1));
}

TEST_CASE( "Func_memory Replicant: read and write")
{
    auto mem1 = FuncMemory::create_4M_plain_memory();
    auto mem2 = FuncMemory::create_4M_plain_memory();
    auto mem3 = FuncMemory::create_4M_plain_memory();
    FuncMemoryReplicant mem12( mem1);
    mem12.add_replica( mem2);

    mem12.write_string( "Hello World", 0x20);
    CHECK( mem1->read_string( 0x20) == "Hello World");
    CHECK( mem2->read_string( 0x20) == "Hello World");
    CHECK( mem12.read_string( 0x20) == "Hello World");

    mem12.duplicate_to( mem3);
    CHECK( mem3->read_string( 0x20) == "Hello World");
    CHECK( mem12.dump() == mem1->dump());
}
