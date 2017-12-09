// generic C
#include <cassert>
#include <cstdlib>

// Google Test library
#include <gtest/gtest.h>

// Module
#include "../perf_sim.h"

static const std::string valid_elf_file = TEST_PATH;
static const int64 num_steps = 2154;

#define GTEST_ASSERT_NO_DEATH(statement) \
    ASSERT_EXIT({{ statement } ::exit(EXIT_SUCCESS); }, ::testing::ExitedWithCode(0), "")

TEST( Perf_Sim_init, Process_Correct_Args_Of_Constr)
{
    // Just call a constructor
    GTEST_ASSERT_NO_DEATH( PerfMIPS mips( false); );
}

TEST( Perf_Sim_init, Make_A_Step)
{
    // Call constructor and run one instr
    GTEST_ASSERT_NO_DEATH( PerfMIPS( false).run( valid_elf_file, 1); );
}

TEST( Perf_Sim_init, Process_Wrong_Args)
{
    // Do bad init
    ASSERT_EXIT( PerfMIPS( false).run( "./1234567890/qwertyuop", 1),
                 ::testing::ExitedWithCode( EXIT_FAILURE), "ERROR.*");
}

TEST( Perf_Sim, Run_Full_Trace)
{
    PerfMIPS mips( false);
    GTEST_ASSERT_NO_DEATH( mips.run( valid_elf_file, num_steps); );
}

int main( int argc, char* argv[])
{
    ::testing::InitGoogleTest( &argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}

