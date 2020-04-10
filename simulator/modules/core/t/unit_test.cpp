/**
 * Test for Performance Simulation
 * Copyright 2018 MIPT-MIPS
 */

#include <catch.hpp>

#include <kernel/kernel.h>
#include <modules/core/perf_sim.h>
#include <modules/writeback/writeback.h>

static void check_init( const std::string& isa)
{
    // Just call a constructor
    auto sim = Simulator::create_simulator( isa, false);
    auto mem = FuncMemory::create_default_hierarchied_memory();
    CHECK_NOTHROW( sim->set_memory( mem));
    CHECK( sim->get_exit_code() == 0);
    CHECK( sim->max_cpu_register() >= 32);
}

TEST_CASE( "Perf_Sim_init: Process_Correct_Args_Of_Constr")
{
    for ( auto isa : Simulator::get_supported_isa())
        check_init( isa);
}

TEST_CASE( "Perf_Sim_init: push a nop")
{
    auto sim = CycleAccurateSimulator::create_simulator( "mips32");
    auto mem = FuncMemory::create_default_hierarchied_memory();
    sim->set_memory( mem);

    auto kernel = Kernel::create_dummy_kernel();
    kernel->set_simulator( sim);
    kernel->connect_memory( mem);
    kernel->connect_exception_handler();
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

static auto create_mars_sim( const std::string& isa, const std::string& binary_name, std::istream& kernel_in, std::ostream& kernel_out, bool has_hooks)
{
    auto sim = CycleAccurateSimulator::create_simulator( isa);
    auto mem = FuncMemory::create_default_hierarchied_memory();
    sim->set_memory( mem);

    auto kernel = Kernel::create_mars_kernel( kernel_in, kernel_out, std::cerr);
    kernel->set_simulator( sim);
    kernel->connect_memory( mem);
    kernel->connect_exception_handler();
    kernel->load_file( binary_name);
    sim->set_kernel( kernel);
    if ( has_hooks)
        sim->enable_driver_hooks();

    sim->init_checker();
    sim->set_pc( kernel->get_start_pc());
    return sim;
}

TEST_CASE( "Torture_Test: Perf_Sim, MARS 32, Core Universal")
{
    std::istream nullin( nullptr);
    std::ostream nullout( nullptr);
    auto sim = create_mars_sim( "mars", TEST_PATH "/mips/mips-tt-no-delayed-branches.bin", nullin, nullout, false);
    CHECK( sim->run_no_limit() == Trap::HALT);
    CHECK( sim->get_exit_code() == 0);
}

TEST_CASE( "Torture_Test: Perf_Sim, MARS 32, Core Universal hooked")
{
    std::istream nullin( nullptr);
    std::ostream nullout( nullptr);
    auto sim = create_mars_sim( "mars", TEST_PATH "/mips/mips-tt-no-delayed-branches.bin", nullin, nullout, true);
    auto trap = sim->run_no_limit();
    CHECK_FALSE( trap == Trap::NO_TRAP);
    CHECK_FALSE( trap == Trap::HALT);
    CHECK( sim->get_exit_code() == 0);
}

TEST_CASE( "Perf_Sim: Run_SMC_Trace_WithChecker")
{
    std::istream nullin( nullptr);
    std::ostream nullout( nullptr);
    CHECK_THROWS_AS( create_mars_sim( "mars", TEST_PATH "/mips/mips-smc.bin", nullin, nullout, false)->run_no_limit(), CheckerMismatch);
}

TEST_CASE( "Torture_Test: Perf_Sim, RISC-V 32 simple trace")
{
    std::istream nullin( nullptr);
    std::ostream nullout( nullptr);
    auto sim = create_mars_sim( "riscv32", TEST_PATH "/riscv/rv32ui-p-simple", nullin, nullout, false);
    CHECK( sim->run_no_limit() == Trap::HALT);
    CHECK( sim->get_exit_code() == 0);
}

TEST_CASE( "Perf_sim: Syscall flushes pipeline")
{
    std::istringstream input( "4\n8\n");
    std::ostream nullout( nullptr);
    auto sim = create_mars_sim( "riscv32", TEST_PATH "/riscv/rv32-scall", input, nullout, false);
    CHECK( sim->run_no_limit() == Trap::HALT);
}

TEST_CASE( "Torture_Test: Perf_Sim, RISC-V 32 breakpoint")
{
    std::istream nullin( nullptr);
    std::ostringstream oss;
    auto sim = create_mars_sim( "riscv32", TEST_PATH "/riscv/rv32ui-p-ebreak", nullin, oss, false);
    CHECK( sim->run_no_limit() == Trap::HALT);
    CHECK( sim->get_exit_code() == 0);
    CHECK( oss.str() == "  Interrupt 3  occurred\n  Exception 3  occurred\n");
}
