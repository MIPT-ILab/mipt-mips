// generic C
#include <cassert>
#include <cstdlib>

// Google Test library
#include <gtest/gtest.h>

// uArchSim modules
#include <func_memory.h>

static const char * valid_elf_file = "./mips_bin_exmpl.out";

//
// Check that all incorect input params of the constructor
// are properly handled.
//
TEST( Func_memory_init, Process_Wrong_Args_Of_Constr)
{  
    // check memory initialization with default parameters 
    ASSERT_NO_THROW( FuncMemory func_mem( valid_elf_file));
    // check memory initialization with custom parameters 
    ASSERT_NO_THROW( FuncMemory func_mem( valid_elf_file, 64, 15, 32));

    // test behavior when the file name does not exist
    const char * wrong_file_name = "./1234567890/qwertyuiop";
    // must exit and return EXIT_FAILURE
    ASSERT_EXIT( FuncMemory func_mem( wrong_file_name),
                 ::testing::ExitedWithCode( EXIT_FAILURE), "ERROR.*");
}

TEST( Func_memory, StartPC_Method_Test)
{
    FuncMemory func_mem( valid_elf_file);

    ASSERT_EQ( func_mem.startPC(), 0x4000b0 /*address of the ".text" section*/);
}

TEST( Func_memory, Read_Method_Test)
{
    FuncMemory func_mem( valid_elf_file);

    // the address of the ".data" section
    uint64 dataSectAddr = 0x4100c0;
 
    // read 4 bytes from the func_mem start addr
    uint64 right_ret = 0x03020100;
    ASSERT_EQ( func_mem.read( dataSectAddr), right_ret);
    
    // read 3 bytes from the func_mem start addr + 1
    right_ret = 0x030201;
    ASSERT_EQ( func_mem.read( dataSectAddr + 1, 3), right_ret);
    
    // read 2 bytes from the func_mem start addr + 2
    right_ret = 0x0302;
    ASSERT_EQ( func_mem.read( dataSectAddr + 2, 2), right_ret);
    
    // read 1 bytes from the func_mem start addr + 3
    right_ret = 0x03;
    ASSERT_EQ( func_mem.read( dataSectAddr + 3, 1), right_ret);

    // check hadling the situation when 0 number of bytes is requested
    ASSERT_EXIT( func_mem.read( dataSectAddr, 0),
                 ::testing::KilledBySignal( SIGABRT), ".*");

    // check hadling the situation when read
    // from not initialized or written data
    ASSERT_EXIT( func_mem.read( 0x300000),
                 ::testing::KilledBySignal( SIGABRT), ".*");
}

TEST( Func_memory, Write_Read_Initialized_Mem_Test)
{
    FuncMemory func_mem( valid_elf_file);

    // the address of the ".data" func_memion
    uint64 data_sect_addr = 0x4100c0;
 
    // write 1 into the byte pointed by data_sect_addr
    func_mem.write( 1, data_sect_addr, sizeof( uint8));
    uint64 right_ret = 0x03020101; // before write it was 0x03020100
    ASSERT_EQ( func_mem.read( data_sect_addr), right_ret);
    
    // write 0x7777 into the two bytes pointed by ( data_sect_addr + 1)
    func_mem.write( 0x7777, data_sect_addr + 1, sizeof( uint16));
    right_ret = 0x03777701; // before write it was 0x03020101
    ASSERT_EQ( func_mem.read( data_sect_addr), right_ret);
    
    // write 0x00000000 into the four bytes pointed by data_sect_addr 
    func_mem.write( 0x00000000, data_sect_addr, sizeof( uint32));
    right_ret = 0x00000000; // before write it was 0x03777701
    ASSERT_EQ( func_mem.read( data_sect_addr), right_ret);

    // check hadling the situation when 0 number of bytes is written
    ASSERT_EXIT( func_mem.write( 1, data_sect_addr, 0),
                 ::testing::KilledBySignal( SIGABRT), ".*");
}

TEST( Func_memory, Write_Read_Not_Initialized_Mem_Test)
{
    FuncMemory func_mem( valid_elf_file);

    uint64 write_addr = 0x3FFFFE;
 
    // write 0x03020100 into the four bytes pointed by write_addr
    func_mem.write( 0x03020100, write_addr, sizeof( uint64));
    uint64 right_ret = 0x0100;
    ASSERT_EQ( func_mem.read( write_addr, sizeof( uint16)), right_ret);
 
    right_ret = 0x0201;
    ASSERT_EQ( func_mem.read( write_addr + 1, sizeof( uint16)), right_ret);
 
    right_ret = 0x0302;
    ASSERT_EQ( func_mem.read( write_addr + 2, sizeof( uint16)), right_ret);
}

int main( int argc, char* argv[])
{
    ::testing::InitGoogleTest( &argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}
