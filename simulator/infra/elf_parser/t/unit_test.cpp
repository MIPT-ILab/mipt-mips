// generic C
#include <cassert>
#include <cstdlib>

// Google Test library
#include <gtest/gtest.h>

// uArchSim modules
#include "../elf_parser.h"

static const std::string valid_elf_file = TEST_PATH "/tt.core.out";
//static const std::string valid_section_name = ".data";

//
// Check that all incorect input params of the constructor
// are properly handled.
//
TEST( Elf_parser_init, Process_Args_Of_Constr)
{
    ASSERT_NO_THROW( ElfSection::getAllElfSections( valid_elf_file));
}

TEST( Elf_parser_init, Process_Wrong_Args_Of_Constr)
{
    // must return empty list
    ASSERT_TRUE( ElfSection::getAllElfSections( std::string("./1234567890/qwertyuiop")).empty());
}

