// generic C
#include <cassert>
#include <cstdlib>

// Google Test library
#include <gtest/gtest.h>

// Module
#include <mips/mips.h>
#include "../perf_sim.h"

static const std::string valid_elf_file = TEST_PATH "/tt.core.out";
static const std::string smc_code = TEST_PATH "/smc.out";

#define GTEST_ASSERT_NO_DEATH(statement) \
    ASSERT_EXIT({{ statement } ::exit(EXIT_SUCCESS); }, ::testing::ExitedWithCode(0), "")

TEST( Perf_Sim_init, Process_Correct_Args_Of_Constr)
{
    // Just call a constructor
    GTEST_ASSERT_NO_DEATH( PerfSim<MIPS32> MIPS32( false); );
}

TEST( Perf_Sim_init, Make_A_Step)
{
    // Call constructor and run one instr
    GTEST_ASSERT_NO_DEATH( PerfSim<MIPS32>( false).run( valid_elf_file, 1); );
}

TEST( Perf_Sim_init, Process_Wrong_Args)
{
    // Do bad init
    ASSERT_EXIT( PerfSim<MIPS32>( false).run( "./1234567890/qwertyuop", 1),
                 ::testing::ExitedWithCode( EXIT_FAILURE), "ERROR.*");
}

TEST( Perf_Sim, Run_Full_Trace)
{
    PerfSim<MIPS32> MIPS32( false);
    GTEST_ASSERT_NO_DEATH( MIPS32.run_no_limit( valid_elf_file); );
}

TEST( Perf_Sim, Run_SMC_Trace)
{
    PerfSim<MIPS32> MIPS32( false);
    ASSERT_EXIT( MIPS32.run_no_limit( smc_code);,
                 ::testing::ExitedWithCode( EXIT_FAILURE), "Mismatch:.*");
}

int main( int argc, char* argv[])
{
    ::testing::InitGoogleTest( &argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}

