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

TEST_CASE( "Perf_Sim_init: Process_Correct_Args_Of_Constr")
{
    // Just call a constructor
    PerfSim<MIPS32> sim( false);
    auto mem = FuncMemory::create_hierarchied_memory();
    CHECK_NOTHROW(sim.set_memory( mem));
}

TEST_CASE( "Perf_Sim_init: push a nop")
{
    PerfSim<MIPS32> sim( false);
    auto mem = FuncMemory::create_hierarchied_memory();
    sim.set_memory( mem);
    sim.init_checker();
    sim.set_pc( 0x10);
    CHECK_NOTHROW( sim.run( 1) );
}

TEST_CASE( "PerfSim: create empty memory and get lost")
{
    auto m = FuncMemory::create_hierarchied_memory();
    PerfSim<MIPS32> sim( false);
    sim.set_memory( m);
    CHECK_THROWS_AS( sim.run_no_limit(), Deadlock);
}

TEST_CASE( "Perf_Sim: unsigned register R/W")
{
    PerfSim<MIPS32> sim( false);
    sim.write_cpu_register( 1, uint64{ MAX_VAL32});
    CHECK( sim.read_cpu_register( 1) == MAX_VAL32 );
}

TEST_CASE( "Perf_Sim: signed register R/W")
{
    PerfSim<MIPS32> sim( false);
    sim.write_cpu_register( 1, narrow_cast<uint64>( -1337));
    CHECK( narrow_cast<int32>( sim.read_cpu_register( 1)) == -1337 );
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

static auto get_smc_loaded_simulator(bool init_checker)
{
    auto sim = std::make_shared<PerfSim<MIPS32>>( false);
    auto mem = FuncMemory::create_hierarchied_memory();
    sim->set_memory( mem);
    ElfLoader elf( TEST_PATH "/smc.out");
    elf.load_to( mem.get());
    if ( init_checker)
        sim->init_checker();
    sim->set_pc( elf.get_startPC());
    return sim;
}

TEST_CASE( "Perf_Sim: Run_SMC_Trace_WithoutChecker")
{
    CHECK( get_smc_loaded_simulator( false)->run_no_limit() == Trap::NO_TRAP);
}

TEST_CASE( "Perf_Sim: Run_SMC_Trace_WithChecker")
{
    CHECK_THROWS_AS( get_smc_loaded_simulator( true)->run_no_limit(), CheckerMismatch);
}
