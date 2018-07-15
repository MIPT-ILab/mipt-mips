// generic C
#include <cassert>
#include <cstdlib>

// Catch2
#include <catch.hpp>

// Module
#include <mips/mips.h>
#include "../func_sim.h"

static const std::string valid_elf_file = TEST_PATH "/tt.core.out";
static const std::string smc_code = TEST_PATH "/smc.out";

TEST_CASE( "Process_Wrong_Args_Of_Constr: Func_Sim_init")
{
    // Just call a constructor
    CHECK_NOTHROW( FuncSim<MIPS32>() );

    // Call constructor and init
    CHECK_NOTHROW( FuncSim<MIPS32>().init( valid_elf_file) );

    // Do bad init
    CHECK_THROWS_AS( FuncSim<MIPS32>().init( "./1234567890/qwertyuop"), InvalidElfFile);
}

TEST_CASE( "Make_A_Step: Func_Sim")
{
    FuncSim<MIPS32> MIPS32;
    MIPS32.init( valid_elf_file);
    CHECK( MIPS32.step().Dump().find("lui $at, 0x41\t [ $at = 0x410000 ]") != std::string::npos);
}

TEST_CASE( "Run_Full_Trace: Func_Sim")
{
    FuncSim<MIPS32> MIPS32;
    MIPS32.run_no_limit( valid_elf_file);
}

TEST_CASE( "Run_SMC_trace: Func_Sim")
{
    FuncSim<MIPS32> MIPS32;
    CHECK_THROWS_AS( MIPS32.run_no_limit( smc_code), BearingLost);
}

