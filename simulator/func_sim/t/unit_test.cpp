/*
 * Unit testing for extremely simple simulator
 * Copyright 2018 MIPT-MIPS
 */

#include "../func_sim.h"

// Catch2
#include <catch.hpp>

// Module
#include <infra/memory/elf/elf_loader.h>    
#include <mips/mips.h>

#include <sstream>

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
    CHECK( simulator.step().string_dump().find("lui $at, 0x41\t [ $at = 0x410000 ]") != std::string::npos);
}

TEST_CASE( "Run one instruction: Func_Sim")
{
    CHECK( FuncSim<MIPS32>().run( smc_code, 1) == Trap::NO_TRAP);
}

TEST_CASE( "Run_SMC_trace: Func_Sim")
{
    CHECK_NOTHROW( FuncSim<MIPS32>().run_no_limit( smc_code));
}

TEST_CASE( "Torture_Test: Func_Sim")
{
    // MIPS 32 Little-endian
    CHECK_NOTHROW( FuncSim<MIPS32>().run_no_limit( TEST_PATH "/tt.core.universal.out") );
    CHECK_NOTHROW( FuncSim<MIPS32>().run_no_limit( TEST_PATH "/tt.core32.out") );
    CHECK_NOTHROW( FuncSim<MIPS32>().run_no_limit( TEST_PATH "/tt.core32.le.out") );

    // MIPS 64 Little-Endian
    CHECK_NOTHROW( FuncSim<MIPS64>().run_no_limit( TEST_PATH "/tt.core.universal.out") );
    CHECK_NOTHROW( FuncSim<MIPS64>().run_no_limit( TEST_PATH "/tt.core64.out") );
    CHECK_NOTHROW( FuncSim<MIPS64>().run_no_limit( TEST_PATH "/tt.core64.le.out") );
}
