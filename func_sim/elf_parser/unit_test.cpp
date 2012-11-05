// generic C
#include <cassert>
#include <cstdlib>

// Google Test library
#include <gtest/gtest.h>

// uArchSim modules
#include <elf_parser.h>

static const char * valid_elf_file = "./mips_bin_exmpl.out";
static const char * valid_section_name = ".text";

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

TEST( Elf_parser, Read_Method_Test)
{
    ElfSection sect( valid_elf_file, valid_section_name);

    // read 4 bytes from the section start addr
    uint64 right_ret = 0x02324020;
    ASSERT_EQ( sect.read( sect.startAddr()), right_ret);
    
    // read 3 bytes from the section start addr + 1
    right_ret = 0x023240;
    ASSERT_EQ( sect.read( sect.startAddr() + 1, 3), right_ret);
    
    // read 2 bytes from the section start addr + 2
    right_ret = 0x0232;
    ASSERT_EQ( sect.read( sect.startAddr() + 2, 2), right_ret);
    
    // read 1 bytes from the section start addr + 3
    right_ret = 0x02;
    ASSERT_EQ( sect.read( sect.startAddr() + 3, 1), right_ret);

    // check hadling the situation when the addr is not inside the section
    ASSERT_EXIT( sect.read( sect.startAddr() - 1),
                 ::testing::KilledBySignal( SIGABRT), ".*");

    // check hadling the situation when 0 number of bytes is requested
    ASSERT_EXIT( sect.read( sect.startAddr(), 0),
                 ::testing::KilledBySignal( SIGABRT), ".*");
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

    ASSERT_EQ( sect.startAddr(), 0x400090);
}

int main( int argc, char* argv[])
{
  ::testing::InitGoogleTest( &argc, argv);
  return RUN_ALL_TESTS();
}
