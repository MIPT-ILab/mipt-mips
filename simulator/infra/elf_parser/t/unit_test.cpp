// generic C
#include <cassert>
#include <cstdlib>

// Catch2
#include <catch.hpp>

// uArchSim modules
#include "../elf_parser.h"

static const std::string valid_elf_file = TEST_PATH "/tt.core.out";
//static const std::string valid_section_name = ".data";

//
// Check that all incorect input params of the constructor
// are properly handled.
//
TEST_CASE( "Elf_parser_init: Process_Args_Of_Constr")
{
    CHECK_NOTHROW( ElfSection::getAllElfSections( valid_elf_file));
}

TEST_CASE( "Elf_parser_init: Process_Wrong_Args_Of_Constr")
{
    // must return empty list
    CHECK_THROWS_AS( ElfSection::getAllElfSections( std::string("./1234567890/qwertyuiop")), InvalidElfFile);
}

