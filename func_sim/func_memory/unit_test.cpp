// generic C
#include <cassert>
#include <cstdlib>

// Google Test library
#include <gtest/gtest.h>

// uArchSim modules
#include <func_memory.h>

static const char * valid_elf_file = "./mips_bin_exmpl.out";
static const char * const valid_elf_section_name[] = {".data", ".text", ".reginfo"};
static const unsigned short num_of_elf_sections = 3;

//
// Check that all incorect input params of the constructor
// are properly handled.
//
TEST( Func_memory_init, Process_Wrong_Args_Of_Constr)
{   
    ASSERT_NO_THROW( FuncMemory( valid_elf_file, valid_elf_section_name,
                                 num_of_elf_sections));

    // test behavior when the file name does not exist
    const char * wrong_file_name = "./1234567890/qwertyuiop";
    // must exit and return EXIT_FAILURE
    ASSERT_EXIT( FuncMemory func_mem( wrong_file_name, valid_elf_section_name,
                                      num_of_elf_sections),
                 ::testing::ExitedWithCode( EXIT_FAILURE), "ERROR.*");
    
    // test behavior when the file name is valid, but not in ELF format
    const char * not_elf_file = "./elf_parser.h";
    // must exit and return EXIT_FAILURE
    ASSERT_EXIT( FuncMemory func_mem( not_elf_file, valid_elf_section_name,
                                      num_of_elf_sections),
                 ::testing::ExitedWithCode( EXIT_FAILURE), "ERROR.*");

    // test behavior when thegiven func_mem does not exist
    const char * const wrong_elf_section_name[] =
        {"qwertyuiop_0", "qwertyuiop_1", "qwertyuiop_2"};
    // must exit and return EXIT_FAILURE
    ASSERT_EXIT( FuncMemory func_mem( valid_elf_file, wrong_elf_section_name,
                                      num_of_elf_sections),
                 ::testing::ExitedWithCode( EXIT_FAILURE), "ERROR.*");
}

TEST( Func_memory, StartPC_Method_Test)
{
    FuncMemory func_mem( valid_elf_file, valid_elf_section_name,
                         num_of_elf_sections);

    ASSERT_EQ( func_mem.startPC(), 0x4000b0 /*address of the ".text" section*/);
}

TEST( Elf_parser, Read_Method_Test)
{
    FuncMemory func_mem( valid_elf_file, valid_elf_section_name,
                         num_of_elf_sections);

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

    // check hadling the situation when the addr is not inside the func_mem
    ASSERT_EXIT( func_mem.read( dataSectAddr - 1),
                 ::testing::KilledBySignal( SIGABRT), ".*");

    // check hadling the situation when 0 number of bytes is requested
    ASSERT_EXIT( func_mem.read( dataSectAddr, 0),
                 ::testing::KilledBySignal( SIGABRT), ".*");
}

TEST( Elf_parser, Write_Method_Test)
{
    FuncMemory func_mem( valid_elf_file, valid_elf_section_name,
                         num_of_elf_sections);

    // the address of the ".data" func_memion
    uint64 dataSectAddr = 0x4100c0;
 
    // write 1 into the byte pointed by the func_mem start addr
    func_mem.write( 1, dataSectAddr, sizeof(uint8));
    uint64 right_ret = 0x03020101; // before write it was 0x03020100
    ASSERT_EQ( func_mem.read( dataSectAddr), right_ret);
    
    // write 0x7777 into the two bytes pointed by the func_mem start addr + 1
    func_mem.write( 0x7777, dataSectAddr + 1, sizeof(uint16));
    right_ret = 0x03777701; // before write it was 0x03020101
    ASSERT_EQ( func_mem.read( dataSectAddr), right_ret);
    
    // write 0x00000000 into the four bytes pointed by the func_mem start addr
    func_mem.write( 0x00000000, dataSectAddr, sizeof(uint32));
    right_ret = 0x00000000; // before write it was 0x00000000
    ASSERT_EQ( func_mem.read( dataSectAddr), right_ret);

    // check hadling the situation when the addr is not inside the func_mem
    ASSERT_EXIT( func_mem.write( 1, dataSectAddr - 1),
                 ::testing::KilledBySignal( SIGABRT), ".*");

    // check hadling the situation when 0 number of bytes is written
    ASSERT_EXIT( func_mem.write( 1, dataSectAddr, 0),
                 ::testing::KilledBySignal( SIGABRT), ".*");
}

int main( int argc, char* argv[])
{
  ::testing::InitGoogleTest( &argc, argv);
  return RUN_ALL_TESTS();
}
