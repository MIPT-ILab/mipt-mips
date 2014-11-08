// generic C
#include <cassert>
#include <cstdlib>

// Google Test library
#include <gtest/gtest.h>

// uArchSim modules
#include <func_instr.h>

//
// Check that all incorect input params of the constructor
// are properly handled.
//
TEST( Func_instr_init, Process_Wrong_Args_Of_Constr)
{   
    ASSERT_NO_THROW( FuncInstr fi(0x0));

    // must exit and return EXIT_FAILURE
    ASSERT_EXIT( FuncInstr fi2(~0x0ull),
                 ::testing::ExitedWithCode( EXIT_FAILURE), "ERROR.*");
}

TEST( Func_instr_dump, Process_Disasm)
{   
    FuncInstr fi(0x016A4821ull);
    std::string result = fi.Dump("");
    std::string master = "add $t0, $t1, $t2";

    // must exit and return EXIT_FAILURE
    ASSERT_EQ( result, master);
}

int main( int argc, char* argv[])
{
    ::testing::InitGoogleTest( &argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}
