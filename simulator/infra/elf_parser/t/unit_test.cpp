// generic C
#include <cassert>
#include <cstdlib>

// Google Test library
#include <gtest/gtest.h>

// uArchSim modules
#include "../elf_parser.h"

static const std::string valid_elf_file = TEST_PATH;
//static const std::string valid_section_name = ".data";

//
// Check that all incorect input params of the constructor
// are properly handled.
//
TEST( Elf_parser_init, Process_Args_Of_Constr)
{
    std::list<ElfSection> sections_array;
    ASSERT_NO_THROW( ElfSection::getAllElfSections( valid_elf_file, &sections_array));
}

TEST( Elf_parser_init, Process_Wrong_Args_Of_Constr)
{
    std::list<ElfSection> sections_array;

    // must exit and return EXIT_FAILURE
    ASSERT_EXIT( ElfSection::getAllElfSections( std::string("./1234567890/qwertyuiop"), &sections_array),
                 ::testing::ExitedWithCode( EXIT_FAILURE), "ERROR.*");
}

int main( int argc, char* argv[])
{
    ::testing::InitGoogleTest( &argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}

