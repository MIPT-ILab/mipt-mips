/**
 * Test for Performance Simulation
 * Copyright 2018 MIPT-MIPS
 */

#include "../perf_sim.h"

// Catch2
#include <catch.hpp>

// Module
#include <memory/elf/elf_loader.h>
#include <modules/writeback/writeback.h>

TEST_CASE( "Perf_Sim_init: Process_Correct_Args_Of_Constr")
{
    // Just call a constructor
    auto sim = CycleAccurateSimulator::create_simulator( "mips32", false);
    auto mem = FuncMemory::create_hierarchied_memory();
    CHECK_NOTHROW(sim->set_memory( mem));
}

TEST_CASE( "Perf_Sim_init: push a nop")
{
    auto sim = CycleAccurateSimulator::create_simulator( "mips32", false);
    auto mem = FuncMemory::create_hierarchied_memory();
    sim->set_memory( mem);
    sim->init_checker();
    sim->set_pc( 0x10);

    CHECK_NOTHROW( sim->get_pc() == 0x10);
    CHECK_NOTHROW( sim->run( 1) );
    CHECK_NOTHROW( sim->get_pc() == 0x14);
}

static void run_empty_memory( const std::string& isa)
{
    auto sim = CycleAccurateSimulator::create_simulator( isa, false);
    sim->set_memory( FuncMemory::create_hierarchied_memory());
    return sim->run_no_limit();
}

TEST_CASE( "Perf_Sim: deadlock on empty memory")
{
    CHECK_THROWS_AS( run_empty_memory("mips32"), Deadlock);
    CHECK_THROWS_AS( run_empty_memory("riscv32"), Deadlock);
    CHECK_THROWS_AS( run_empty_memory("riscv128"), Deadlock);
}

TEST_CASE( "Perf_Sim: unsigned register R/W")
{
    auto sim = CycleAccurateSimulator::create_simulator( "mips32", false);
    sim->write_cpu_register( 1, uint64{ MAX_VAL32});
    CHECK( sim->read_cpu_register( 1) == MAX_VAL32 );
}

TEST_CASE( "Perf_Sim: signed register R/W")
{
    auto sim = CycleAccurateSimulator::create_simulator( "mips32", false);
    sim->write_cpu_register( 1, narrow_cast<uint64>( -1337));
    CHECK( narrow_cast<int32>( sim->read_cpu_register( 1)) == -1337 );
}

TEST_CASE( "Perf_Sim: GDB Register R/W")
{
    auto sim = CycleAccurateSimulator::create_simulator( "mips32", false);
    sim->write_gdb_register( 1, uint64{ MAX_VAL32});
    CHECK( sim->read_gdb_register( 1) == MAX_VAL32 );
    CHECK( sim->read_gdb_register( 0) == 0 );

    sim->write_gdb_register( 37, 100500);
    CHECK( sim->read_gdb_register( 37) == 100500);
    CHECK( sim->get_pc() == 100500);
}

TEST_CASE( "Perf_Sim: Register size")
{
    CHECK( CycleAccurateSimulator::create_simulator( "mips32", false)->sizeof_register() == bytewidth<uint32>);
    CHECK( CycleAccurateSimulator::create_simulator( "mips64", false)->sizeof_register() == bytewidth<uint64>);
}

TEST_CASE( "Torture_Test: Perf_Sim , MARS 64, Core 64")
{
    auto sim = CycleAccurateSimulator::create_simulator( "mars64", false);
    auto mem = FuncMemory::create_hierarchied_memory();
    sim->set_memory( mem);
    ElfLoader elf( TEST_PATH "/tt.core64.out");
    elf.load_to( mem.get());
    sim->init_checker();
    sim->set_pc( elf.get_startPC());
    CHECK_NOTHROW( sim->run_no_limit() );
}

static auto get_smc_loaded_simulator(bool init_checker)
{
    auto sim = CycleAccurateSimulator::create_simulator( "mars", false);
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
