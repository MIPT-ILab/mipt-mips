/**
 * Test for Performance Simulation
 * Copyright 2018 MIPT-MIPS
 */

#include "../perf_sim.h"

// Catch2
#include <catch.hpp>

// Module
#include <infra/memory/elf/elf_loader.h>    
#include <mips/mips.h>
#include <modules/writeback/writeback.h>

static const std::string valid_elf_file = TEST_PATH "/tt.core32.out";
static const std::string smc_code = TEST_PATH "/smc.out";

TEST_CASE( "Perf_Sim_init: Process_Correct_Args_Of_Constr")
{
    // Just call a constructor
    CHECK_NOTHROW( PerfSim<MIPS32>( false) );
}

TEST_CASE( "Perf_Sim_init: Make_A_Step")
{
    // Call constructor and run one instr
    CHECK_NOTHROW( PerfSim<MIPS32>( false).run( valid_elf_file, 1) );
}

TEST_CASE( "Perf_Sim_init: Process_Wrong_Args")
{
    // Do bad init
    CHECK_THROWS_AS( PerfSim<MIPS32>( false).run( "./1234567890/qwertyuop", 1), InvalidElfFile);
}

TEST_CASE( "Torture_Test: Perf_Sim")
{
    // MIPS 32 Little-Endian
    CHECK_NOTHROW( PerfSim<MIPS32>( false).run_no_limit( TEST_PATH "/tt.core.universal.out") );
    CHECK_NOTHROW( PerfSim<MIPS32>( false).run_no_limit( TEST_PATH "/tt.core32.out") );
    CHECK_NOTHROW( PerfSim<MIPS32>( false).run_no_limit( TEST_PATH "/tt.core32.le.out") );

    // MIPS 64 Little-Endian
    CHECK_NOTHROW( PerfSim<MIPS64>( false).run_no_limit( TEST_PATH "/tt.core.universal.out") );
    CHECK_NOTHROW( PerfSim<MIPS64>( false).run_no_limit( TEST_PATH "/tt.core64.out") );
    CHECK_NOTHROW( PerfSim<MIPS64>( false).run_no_limit( TEST_PATH "/tt.core64.le.out") );
}

TEST_CASE( "Perf_Sim: Run_SMC_Trace")
{
    CHECK_THROWS_AS( PerfSim<MIPS32>( false).run_no_limit( smc_code), CheckerMismatch);
}

