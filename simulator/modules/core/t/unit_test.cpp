/**
 * Test for Performance Simulation
 * Copyright 2018 MIPT-MIPS
 */

#include "../perf_sim.h"

// Catch2
#include <catch.hpp>

// Module
#include <memory/elf/elf_loader.h>
#include <mips/mips.h>
#include <modules/writeback/writeback.h>

static const std::string valid_elf_file = TEST_PATH "/tt.core32.out";
static const std::string smc_code = TEST_PATH "/smc.out";

TEST_CASE( "PerfSim: create empty memory and get lost")
{
    auto m = FuncMemory::create_hierarchied_memory();
    PerfSim<MIPS32> sim( false);
    sim.set_memory( m);
    CHECK_THROWS_AS( sim.run_no_limit(), Deadlock);
}

TEST_CASE( "Perf_Sim_init: Process_Correct_Args_Of_Constr")
{
    // Just call a constructor
    PerfSim<MIPS32> sim( false);
    auto mem = FuncMemory::create_hierarchied_memory();
    CHECK_NOTHROW(sim.set_memory( mem));
}

TEST_CASE( "Perf_Sim_init: Make_A_Step")
{
    // Call constructor and run one instr
    PerfSim<MIPS32> sim( false);
    auto mem = FuncMemory::create_hierarchied_memory();
    sim.set_memory( mem);
    ElfLoader elf( valid_elf_file);
    elf.load_to( mem.get());
    sim.init_checker();
    sim.set_pc( elf.get_startPC());
    CHECK_NOTHROW( sim.run( 1) );
}

TEST_CASE( "Perf_Sim: Register R/W")
{
    PerfSim<MIPS32> sim( false);

    /* Signed */
    sim.write_cpu_register( 1, narrow_cast<uint64>( -1337));
    CHECK( narrow_cast<int32>( sim.read_cpu_register( 1)) == -1337 );
    /* Unsigned */
    sim.write_cpu_register( 1, uint64{ MAX_VAL32});
    CHECK( sim.read_cpu_register( 1) == MAX_VAL32 );
}

TEST_CASE( "Perf_Sim: Register size")
{
    CHECK( PerfSim<MIPS32>( false).sizeof_register() == bytewidth<uint32>);
    CHECK( PerfSim<MIPS64>( false).sizeof_register() == bytewidth<uint64>);
}

TEST_CASE( "Torture_Test: Perf_Sim , MIPS 64, Core 64")
{
    PerfSim<MIPS64> sim( false);
    auto mem = FuncMemory::create_hierarchied_memory();
    sim.set_memory( mem);
    ElfLoader elf( TEST_PATH "/tt.core64.out");
    elf.load_to( mem.get());
    sim.init_checker();
    sim.set_pc( elf.get_startPC());
    CHECK_NOTHROW( sim.run_no_limit() );
}

TEST_CASE( "Perf_Sim: Run_SMC_Trace_WithoutChecker")
{
    PerfSim<MIPS32> sim( false);
    auto mem = FuncMemory::create_hierarchied_memory();
    sim.set_memory( mem);
    ElfLoader elf( smc_code);
    elf.load_to( mem.get());
    sim.set_pc( elf.get_startPC());
    CHECK( sim.run_no_limit( ) == Trap::NO_TRAP);
}

TEST_CASE( "Perf_Sim: Run_SMC_Trace_WithChecker")
{
    PerfSim<MIPS32> sim( false);
    auto mem = FuncMemory::create_hierarchied_memory();
    sim.set_memory( mem);
    ElfLoader elf( smc_code);
    elf.load_to( mem.get());
    sim.init_checker();
    sim.set_pc( elf.get_startPC());

    CHECK_THROWS_AS( sim.run_no_limit( ), CheckerMismatch);
}
