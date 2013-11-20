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
    vector<ElfSection> sections_array;
    
    ASSERT_NO_THROW( ElfSection::getAllElfSections( valid_elf_file, sections_array));

    // test behavior when the file name does not exist
    const char * wrong_file_name = "./1234567890/qwertyuiop";
    // must exit and return EXIT_FAILURE
    ASSERT_EXIT( ElfSection::getAllElfSections( wrong_file_name, sections_array),
                 ::testing::ExitedWithCode( EXIT_FAILURE), "ERROR.*");
}

int main( int argc, char* argv[])
{
    ::testing::InitGoogleTest( &argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}
