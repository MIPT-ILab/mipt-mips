// generic C
#include <cassert>
#include <cstdlib>

// Google Test library
#include <gtest/gtest.h>

// Module
#include "../perf_sim.h"

static const std::string valid_elf_file = "./all_mips.out";
static const int64 num_steps = 1498;

#define GTEST_ASSERT_NO_DEATH(statement) \
    ASSERT_EXIT({{ statement } ::exit(EXIT_SUCCESS); }, ::testing::ExitedWithCode(0), "")

TEST( Func_Sim_init, Process_Wrong_Args_Of_Constr)
{
    // Just call a constructor
    ASSERT_NO_THROW( MIPS mips );

    // Call constructor and init
    ASSERT_NO_THROW( MIPS().init( valid_elf_file) );
   
    // Do bad init
    ASSERT_EXIT( MIPS().init( "./1234567890/qwertyuop"),
                 ::testing::ExitedWithCode( EXIT_FAILURE), "ERROR.*");
}

TEST( Func_Sim, Make_A_Step)
{
    MIPS mips;
    mips.init( valid_elf_file);
    ASSERT_EQ( mips.step(), "0x4000f0: lui $at, 0x41\t [ $at = 0x410000]");
}

TEST( Func_Sim, Run_Full_Trace)
{
    MIPS mips;
    GTEST_ASSERT_NO_DEATH( mips.run( valid_elf_file, num_steps); );
}

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

