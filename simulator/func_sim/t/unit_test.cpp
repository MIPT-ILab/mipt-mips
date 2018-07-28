/*
 * Unit testing for extremely simple simulator
 * Copyright 2018 MIPT-MIPS
 */

#include "../func_sim.h"

// Catch2
#include <catch.hpp>

// Module
#include <mips/mips.h>

static const std::string valid_elf_file = TEST_PATH "/tt.core32.out";
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
    FuncSim<MIPS32> simulator;
    simulator.init( valid_elf_file);
    CHECK( simulator.step().Dump().find("lui $at, 0x41\t [ $at = 0x410000 ]") != std::string::npos);
}

TEST_CASE( "Run_Full_Trace_MIPS32: Func_Sim")
{
    FuncSim<MIPS32>().run_no_limit( TEST_PATH "/tt.core32.out");
}

TEST_CASE( "Run_Full_Trace_MIPS32_universal: Func_Sim")
{
    FuncSim<MIPS32>().run_no_limit( TEST_PATH "/tt.core.universal.out");
}

TEST_CASE( "Run_Full_Trace_MIPS64: Func_Sim")
{
    FuncSim<MIPS64>().run_no_limit( TEST_PATH "/tt.core64.out");
}

TEST_CASE( "Run_Full_Trace_MIPS64_universal: Func_Sim")
{
    FuncSim<MIPS64>().run_no_limit( TEST_PATH "/tt.core.universal.out");
}

TEST_CASE( "Run_SMC_trace: Func_Sim")
{
    CHECK_THROWS_AS( FuncSim<MIPS32>().run_no_limit( smc_code), BearingLost);
}

