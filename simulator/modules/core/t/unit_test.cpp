/**
 * Test for Performance Simulation
 * Copyright 2018 MIPT-MIPS
 */

#include <catch.hpp>

#include <kernel/kernel.h>
#include <modules/core/perf_sim.h>
#include <modules/writeback/writeback.h>

static auto init( const std::string& isa)
{
    // Just call a constructor
    auto sim = Simulator::create_simulator( isa, false);
    auto mem = FuncMemory::create_default_hierarchied_memory();

    auto kernel = Kernel::create_kernel( false, std::cin, std::cout, std::cerr);
    kernel->set_simulator( sim);
    kernel->connect_memory( mem);
    kernel->connect_exception_handler();

    sim->set_kernel( kernel);
    sim->set_memory( mem);

    return sim;
}

// Redirect std::cout to /dev/null
static auto run_silent( const std::shared_ptr<Simulator>& sim)
{
    std::ostream nullout( nullptr);
    OStreamWrapper cout_wrapper( std::cout, nullout);
    return sim->run_no_limit();
}

static auto run_silent( const std::shared_ptr<Simulator>& sim, size_t steps)
{
    std::ostream nullout( nullptr);
    OStreamWrapper cout_wrapper( std::cout, nullout);
    return sim->run( steps);
}

TEST_CASE( "Perf_Sim_init: Process_Correct_Args_Of_Constr")
{
    for ( const auto& isa : Simulator::get_supported_isa()) {
        auto sim = init( isa);          
        CHECK( sim->get_exit_code() == 0);
        CHECK( sim->max_cpu_register() >= 32);
    }
}

TEST_CASE( "Perf_Sim_init: Default configuration")
{
    auto sim = Simulator::create_configured_simulator();
    CHECK( sim->get_isa() == "mars");
}

TEST_CASE( "Perf_Sim_init: Default configuration and isa")
{
    auto sim = Simulator::create_configured_isa_simulator("riscv32");
    CHECK( sim->get_isa() == "riscv32");
}

TEST_CASE( "Perf_Sim_init: Set PC")
{
    auto sim = init( "mips32");
    sim->set_pc( 0x10);

    CHECK( sim->get_pc() == 0x10);
}

TEST_CASE( "Perf_Sim_init: push a nop")
{
    auto sim = init( "mips32");
    sim->set_pc( 0x10);

    run_silent( sim, 1);
    CHECK( sim->get_pc() == 0x18);
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

    auto kernel = Kernel::create_kernel( true, kernel_in, kernel_out, std::cerr);
    kernel->set_simulator( sim);
    kernel->connect_memory( mem);
    kernel->connect_exception_handler();
    kernel->load_file( binary_name);
    sim->set_kernel( kernel);
    if ( has_hooks)
        sim->enable_driver_hooks();

    sim->set_pc( kernel->get_start_pc());
    return sim;
}

TEST_CASE( "Torture_Test: Perf_Sim, MARS 32, Core Universal")
{
    std::istream nullin( nullptr);
    std::ostream nullout( nullptr);
    auto sim = create_mars_sim( "mars", TEST_PATH "/mips/mips-tt-no-delayed-branches.bin", nullin, nullout, false);

    CHECK( run_silent( sim) == Trap::HALT);
    CHECK( sim->get_exit_code() == 0);
}

TEST_CASE( "Torture_Test: Perf_Sim, MARS 32, Core Universal hooked")
{
    std::istream nullin( nullptr);
    std::ostream nullout( nullptr);
    OStreamWrapper cerr_wrapper( std::cout, nullout);
    auto sim = create_mars_sim( "mars", TEST_PATH "/mips/mips-tt-no-delayed-branches.bin", nullin, nullout, true);
    auto trap = run_silent( sim);
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

TEST_CASE( "Perf_Sim: Run_SMC_Trace_WithoutChecker")
{
    std::istream nullin( nullptr);
    std::ostream nullout( nullptr);
    auto sim = create_mars_sim( "mars", TEST_PATH "/mips/mips-smc.bin", nullin, nullout, false);
    sim->disable_checker();
    run_silent( sim);
    CHECK( sim->get_exit_code() == 0);
}


TEST_CASE( "Torture_Test: Perf_Sim, RISC-V 32 simple trace")
{
    std::istream nullin( nullptr);
    std::ostream nullout( nullptr);
    auto sim = create_mars_sim( "riscv32", TEST_PATH "/riscv/rv32ui-p-simple", nullin, nullout, false);
    CHECK( run_silent( sim) == Trap::HALT);
    CHECK( sim->get_exit_code() == 0);
}

TEST_CASE( "Perf_sim: Syscall flushes pipeline")
{
    std::istringstream input( "4\n8\n");
    std::ostream nullout( nullptr);
    auto sim = create_mars_sim( "riscv32", TEST_PATH "/riscv/rv32-scall", input, nullout, false);
    CHECK( run_silent( sim) == Trap::HALT);
}

TEST_CASE( "Torture_Test: Perf_Sim, RISC-V 32 breakpoint")
{
    std::istream nullin( nullptr);
    std::ostringstream oss;
    auto sim = create_mars_sim( "riscv32", TEST_PATH "/riscv/rv32ui-p-ebreak", nullin, oss, false);
    CHECK( run_silent( sim) == Trap::HALT);
    CHECK( sim->get_exit_code() == 0);
    CHECK( oss.str() == "  Interrupt 3  occurred\n  Exception 3  occurred\n");
}
