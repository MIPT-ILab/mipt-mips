// generic C
#include <cassert>
#include <cstdlib>

// Google Test library
#include <gtest/gtest.h>

// Module
#include <mips/mips.h>
#include "../func_sim.h"

static const std::string valid_elf_file = TEST_PATH "/tt.core.out";
static const std::string smc_code = TEST_PATH "/smc.out";

#define GTEST_ASSERT_NO_DEATH(statement) \
    ASSERT_EXIT({{ statement } ::exit(EXIT_SUCCESS); }, ::testing::ExitedWithCode(0), "")

TEST( Func_Sim_init, Process_Wrong_Args_Of_Constr)
{
    // Just call a constructor
    ASSERT_NO_THROW( FuncSim<MIPS> mips );

    // Call constructor and init
    ASSERT_NO_THROW( FuncSim<MIPS>().init( valid_elf_file) );

    // Do bad init
    ASSERT_EXIT( FuncSim<MIPS>().init( "./1234567890/qwertyuop"),
                 ::testing::ExitedWithCode( EXIT_FAILURE), "ERROR.*");
}

TEST( Func_Sim, Make_A_Step)
{
    FuncSim<MIPS> mips;
    mips.init( valid_elf_file);
    ASSERT_EQ( mips.step().Dump(), "0x4000f0: lui $at, 0x41\t [ $at = 0x410000 ]");
}

TEST( Func_Sim, Run_Full_Trace)
{
    FuncSim<MIPS> mips( true);
    GTEST_ASSERT_NO_DEATH( mips.run_no_limit( valid_elf_file); );
}

TEST( Func_Sim, Run_SMC_trace)
{
    FuncSim<MIPS> mips;
    ASSERT_EXIT( mips.run_no_limit( smc_code);,
                 ::testing::ExitedWithCode( EXIT_FAILURE), "Bearings lost:.*");
}

int main( int argc, char* argv[])
{
    ::testing::InitGoogleTest( &argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}

