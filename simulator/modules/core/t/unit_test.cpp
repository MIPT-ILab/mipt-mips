/**
 * Test for Performance Simulation
 * Copyright 2018 MIPT-MIPS
 */

#include "../perf_sim.h"

#include <catch.hpp>

#include <kernel/kernel.h>
#include <modules/writeback/writeback.h>

TEST_CASE( "Perf_Sim_init: Process_Correct_Args_Of_Constr")
{
    // Just call a constructor
    auto sim = Simulator::create_simulator( "mips32", false);
    auto mem = FuncMemory::create_default_hierarchied_memory();
    CHECK_NOTHROW( sim->set_memory( mem));
    CHECK( sim->get_exit_code() == 0);
}

TEST_CASE( "Perf_Sim_init: push a nop")
{
    auto sim = CycleAccurateSimulator::create_simulator( "mips32");
    auto mem = FuncMemory::create_default_hierarchied_memory();
    sim->set_memory( mem);

    auto kernel = Kernel::create_dummy_kernel();
    kernel->set_simulator( sim);
    kernel->connect_memory( mem);
    sim->set_kernel( kernel);

    sim->init_checker();
    sim->set_pc( 0x10);

    CHECK_NOTHROW( sim->get_pc() == 0x10);
    CHECK_NOTHROW( sim->run( 1) );
    CHECK_NOTHROW( sim->get_pc() == 0x14);
    CHECK( sim->get_exit_code() == 0);
}

TEST_CASE( "PerfSim: create empty memory and get lost")
{
    auto m = FuncMemory::create_default_hierarchied_memory();
    auto sim = CycleAccurateSimulator::create_simulator( "mips32");
    sim->set_memory( m);
    CHECK_THROWS_AS( sim->run_no_limit(), Deadlock);
}

TEST_CASE( "Perf_Sim: unsigned register R/W")
{
    auto sim = CycleAccurateSimulator::create_simulator( "mips32");
    sim->write_cpu_register( 1, uint64{ MAX_VAL32});
    CHECK( sim->read_cpu_register( 1) == MAX_VAL32 );
    CHECK( sim->get_exit_code() == 0);
}

TEST_CASE( "Perf_Sim: signed register R/W")
{
    auto sim = CycleAccurateSimulator::create_simulator( "mips32");
    sim->write_cpu_register( 1, narrow_cast<uint64>( -1337));
    CHECK( narrow_cast<int32>( sim->read_cpu_register( 1)) == -1337 );
    CHECK( sim->get_exit_code() == 0);
}

TEST_CASE( "Perf_Sim: GDB Register R/W")
{
    auto sim = CycleAccurateSimulator::create_simulator( "mips32");
    sim->write_gdb_register( 1, uint64{ MAX_VAL32});
    CHECK( sim->read_gdb_register( 1) == MAX_VAL32 );
    CHECK( sim->read_gdb_register( 0) == 0 );

    sim->write_gdb_register( 37, 100500);
    CHECK( sim->read_gdb_register( 37) == 100500);
    CHECK( sim->get_pc() == 100500);
    CHECK( sim->get_exit_code() == 0);
}

TEST_CASE( "Perf_Sim: csr Register R/W")
{
    auto sim = CycleAccurateSimulator::create_simulator( "riscv32");
    sim->write_csr_register( "mscratch", 333);
    CHECK( sim->read_csr_register( "mscratch") == 333 );
    CHECK( sim->get_exit_code() == 0);
}

TEST_CASE( "Perf_Sim: Register size")
{
    CHECK( CycleAccurateSimulator::create_simulator( "mips32")->sizeof_register() == bytewidth<uint32>);
    CHECK( CycleAccurateSimulator::create_simulator( "mips64")->sizeof_register() == bytewidth<uint64>);
}

static auto get_mars32_tt_simulator( bool has_hooks)
{
    auto sim = CycleAccurateSimulator::create_simulator( "mars");
    auto mem = FuncMemory::create_default_hierarchied_memory();
    sim->set_memory( mem);

    auto kernel = Kernel::create_mars_kernel();
    kernel->connect_memory( mem);
    kernel->set_simulator( sim);
    kernel->load_file( TEST_PATH "/mips-tt-no-delayed-branches.bin");
    sim->set_kernel( kernel);
    if ( has_hooks)
        sim->enable_driver_hooks();

    sim->init_checker();
    sim->set_pc( kernel->get_start_pc());
    return sim;
}

TEST_CASE( "Torture_Test: Perf_Sim, MARS 32, Core Universal")
{
    auto sim = get_mars32_tt_simulator( false);
    CHECK( sim->run_no_limit() == Trap::HALT);
    CHECK( sim->get_exit_code() == 0);
}

TEST_CASE( "Torture_Test: Perf_Sim, MARS 32, Core Universal hooked")
{
    auto sim = get_mars32_tt_simulator( true);
    auto trap = sim->run_no_limit();
    CHECK_FALSE( trap == Trap::NO_TRAP);
    CHECK_FALSE( trap == Trap::HALT);
    CHECK( sim->get_exit_code() == 0);
}

static auto get_smc_loaded_simulator( bool init_checker)
{
    auto sim = CycleAccurateSimulator::create_simulator( "mars");
    auto mem = FuncMemory::create_default_hierarchied_memory();
    sim->set_memory( mem);

    auto kernel = Kernel::create_mars_kernel();
    kernel->connect_memory( mem);
    kernel->set_simulator( sim);
    kernel->load_file( TEST_PATH "/mips-smc.bin");
    sim->set_kernel( kernel);

    if ( init_checker)
        sim->init_checker();
    sim->set_pc( kernel->get_start_pc());
    return sim;
}

TEST_CASE( "Perf_Sim: Run_SMC_Trace_WithoutChecker")
{
    CHECK( get_smc_loaded_simulator( false)->run_no_limit() == Trap::HALT);
}

TEST_CASE( "Perf_Sim: Run_SMC_Trace_WithChecker")
{
    CHECK_THROWS_AS( get_smc_loaded_simulator( true)->run_no_limit(), CheckerMismatch);
}

TEST_CASE( "Torture_Test: Perf_Sim, RISC-V 32 simple trace")
{
    auto sim = CycleAccurateSimulator::create_simulator( "riscv32");
    auto mem = FuncMemory::create_default_hierarchied_memory();
    sim->set_memory( mem);
    auto kernel = Kernel::create_dummy_kernel();
    kernel->connect_memory( mem);
    kernel->set_simulator( sim);
    kernel->load_file( TEST_PATH "/rv32ui-p-simple");
    sim->set_kernel( kernel);
    sim->init_checker();
    sim->set_pc( kernel->get_start_pc());
    CHECK( sim->run_no_limit() == Trap::HALT);
    CHECK( sim->get_exit_code() == 0);
}
