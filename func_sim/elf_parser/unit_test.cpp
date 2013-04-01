// generic C
#include <cassert>
#include <cstdlib>

// Google Test library
#include <gtest/gtest.h>

// uArchSim modules
#include <elf_parser.h>

static const char * valid_elf_file = "./mips_bin_exmpl.out";
static const char * valid_section_name = ".data";

//
// Check that all incorect input params of the constructor
// are properly handled.
//
TEST( Elf_parser_init, Process_Wrong_Args_Of_Constr)
{   
    ASSERT_NO_THROW( ElfSection section( valid_elf_file, valid_section_name));

    // test behavior when the file name does not exist
    const char * wrong_file_name = "./1234567890/qwertyuiop";
    // must exit and return EXIT_FAILURE
    ASSERT_EXIT( ElfSection section( wrong_file_name, valid_section_name),
                 ::testing::ExitedWithCode( EXIT_FAILURE), "ERROR.*");
    
    // test behavior when the file name is valid, but not in ELF format
    const char * not_elf_file = "./elf_parser.h";
    // must exit and return EXIT_FAILURE
    ASSERT_EXIT( ElfSection section( not_elf_file, valid_section_name),
                 ::testing::ExitedWithCode( EXIT_FAILURE), "ERROR.*");
    
    // test behavior when thegiven section does not exist
    const char * wrong_section_name = "./1234567890/qwertyuiop";
    // must exit and return EXIT_FAILURE
    ASSERT_EXIT( ElfSection section( valid_elf_file, wrong_section_name),
                 ::testing::ExitedWithCode( EXIT_FAILURE), "ERROR.*");
}

TEST( Elf_parser, IsInside_Method_Test)
{
    ElfSection sect( valid_elf_file, valid_section_name);

    for( short i = 1; i < 8; ++i)
        ASSERT_EQ( sect.isInside( sect.startAddr() + i, i), true);
    
    for( short i = 1; i < 8; ++i)
        ASSERT_EQ( sect.isInside( sect.startAddr() + 1000 * i), false);
        
    ASSERT_EQ( sect.isInside( sect.startAddr() - 1), false);

    // check hadling the situation when 0 number of bytes is requested
    ASSERT_EXIT( sect.isInside( sect.startAddr(), 0),
                 ::testing::KilledBySignal( SIGABRT), ".*");
}

TEST( Elf_parser, StartAddr_Method_Test)
{
    ElfSection sect( valid_elf_file, valid_section_name);

    ASSERT_EQ( sect.startAddr(), 0x4100c0);
}

TEST( Elf_parser, Read_Method_Test)
{
    ElfSection sect( valid_elf_file, valid_section_name);

    // read 4 bytes from the section start addr
    uint64 right_ret = 0x03020100;
    ASSERT_EQ( sect.read( sect.startAddr()), right_ret);
    
    // read 3 bytes from the section start addr + 1
    right_ret = 0x030201;
    ASSERT_EQ( sect.read( sect.startAddr() + 1, 3), right_ret);
    
    // read 2 bytes from the section start addr + 2
    right_ret = 0x0302;
    ASSERT_EQ( sect.read( sect.startAddr() + 2, 2), right_ret);
    
    // read 1 bytes from the section start addr + 3
    right_ret = 0x03;
    ASSERT_EQ( sect.read( sect.startAddr() + 3, 1), right_ret);

    // check hadling the situation when the addr is not inside the section
    ASSERT_EXIT( sect.read( sect.startAddr() - 1),
                 ::testing::KilledBySignal( SIGABRT), ".*");

    // check hadling the situation when 0 number of bytes is requested
    ASSERT_EXIT( sect.read( sect.startAddr(), 0),
                 ::testing::KilledBySignal( SIGABRT), ".*");
}

TEST( Elf_parser, Write_Method_Test)
{
    ElfSection sect( valid_elf_file, valid_section_name);

    // write 1 into the byte pointed by the section start addr
    sect.write( 1, sect.startAddr(), sizeof(uint8));
    uint64 right_ret = 0x03020101; // before write it was 0x03020100
    ASSERT_EQ( sect.read( sect.startAddr()), right_ret);
    
    // write 0x7777 into the two bytes pointed by the section start addr + 1
    sect.write( 0x7777, sect.startAddr() + 1, sizeof(uint16));
    right_ret = 0x03777701; // before write it was 0x03020101
    ASSERT_EQ( sect.read( sect.startAddr()), right_ret);
    
    // write 0x00000000 into the four bytes pointed by the section start addr
    sect.write( 0x00000000, sect.startAddr(), sizeof(uint32));
    right_ret = 0x00000000; // before write it was 0x00000000
    ASSERT_EQ( sect.read( sect.startAddr()), right_ret);

    // check hadling the situation when the addr is not inside the section
    ASSERT_EXIT( sect.write( 1, sect.startAddr() - 1),
                 ::testing::KilledBySignal( SIGABRT), ".*");

    // check hadling the situation when 0 number of bytes is written
    ASSERT_EXIT( sect.write( 1, sect.startAddr(), 0),
                 ::testing::KilledBySignal( SIGABRT), ".*");
}

int main( int argc, char* argv[])
{
  ::testing::InitGoogleTest( &argc, argv);
  return RUN_ALL_TESTS();
}
