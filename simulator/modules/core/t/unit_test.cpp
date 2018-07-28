/**
 * Test for Performance Simulation
 * Copyright 2018 MIPT-MIPS
 */

#include "../perf_sim.h"

// Catch2
#include <catch.hpp>

// Module
#include <mips/mips.h>
#include <modules/writeback/writeback.h>

static const std::string valid_elf_file = TEST_PATH "/tt.core.out";
static const std::string smc_code = TEST_PATH "/smc.out";

TEST_CASE( "Perf_Sim_init: Process_Correct_Args_Of_Constr")
{
    // Just call a constructor
    PerfSim<MIPS32>( false);
}

TEST_CASE( "Perf_Sim_init: Make_A_Step")
{
    // Call constructor and run one instr
    PerfSim<MIPS32>( false).run( valid_elf_file, 1);
}

TEST_CASE( "Perf_Sim_init: Process_Wrong_Args")
{
    // Do bad init
    CHECK_THROWS_AS( PerfSim<MIPS32>( false).run( "./1234567890/qwertyuop", 1), InvalidElfFile);
}

TEST_CASE( "Perf_Sim: Run_Full_Trace")
{
    PerfSim<MIPS32> MIPS32( false);
    MIPS32.run_no_limit( valid_elf_file);
}

TEST_CASE( "Perf_Sim: Run_SMC_Trace")
{
    PerfSim<MIPS32> MIPS32( false);
    CHECK_THROWS_AS( MIPS32.run_no_limit( smc_code), CheckerMismatch);
}

