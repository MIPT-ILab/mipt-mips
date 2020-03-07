/*
 * Unit testing for extremely simple simulator
 * Copyright 2018 MIPT-MIPS
 */

#include <catch.hpp>

#include <func_sim/func_sim.h>
#include <kernel/kernel.h>
#include <memory/memory.h>
#include <mips/mips_register/mips_register.h>
#include <simulator.h>

TEST_CASE( "Process_Wrong_Args_Of_Constr: Func_Sim_init")
{
    // Just call a constructor
    CHECK_NOTHROW( Simulator::create_functional_simulator("mips32") );
    CHECK_NOTHROW( Simulator::create_functional_simulator("mips32", true) );
    CHECK_THROWS_AS( Simulator::create_functional_simulator("pdp11"), InvalidISA);
}

static auto run_over_empty_memory( const std::string& isa)
{
    auto sim = Simulator::create_functional_simulator( isa);
    sim->enable_driver_hooks();
    auto m = FuncMemory::create_default_hierarchied_memory();
    sim->set_memory( m);
    auto kernel = Kernel::create_dummy_kernel();
    kernel->set_simulator( sim);
    kernel->connect_memory( m);
    kernel->connect_exception_handler();
    kernel->load_file( TEST_PATH "/mips-tt-no-delayed-branches.bin");
    sim->set_kernel( kernel);
    return sim->run( 30);
}

TEST_CASE( "FuncSim: create empty memory and get lost")
{
    CHECK_THROWS_AS( run_over_empty_memory("mips32"), BearingLost);
    CHECK( run_over_empty_memory("riscv32") == Trap::UNKNOWN_INSTRUCTION);
    CHECK( run_over_empty_memory("riscv128") == Trap::UNKNOWN_INSTRUCTION);
}

TEST_CASE( "FuncSim: get lost without pc")
{
    auto m   = FuncMemory::create_default_hierarchied_memory();
    auto sim = Simulator::create_functional_simulator("mips32");
    sim->set_memory( m);
    auto kernel = Kernel::create_dummy_kernel();
    kernel->set_simulator( sim);
    kernel->connect_memory( m);
    kernel->connect_exception_handler();
    kernel->load_file( TEST_PATH "/mips-tt-no-delayed-branches.bin");
    sim->set_kernel( kernel);
    CHECK_THROWS_AS( sim->run_no_limit(), BearingLost);
    CHECK( sim->get_exit_code() == 0);
}

TEST_CASE( "Process_Wrong_Args_Of_Constr: Func_Sim_init_and_load")
{
    // Call constructor and init
    auto sim = Simulator::create_functional_simulator("mips32");
    auto mem = FuncMemory::create_default_hierarchied_memory();
    sim->set_memory( mem);
    auto kernel = Kernel::create_dummy_kernel();
    kernel->set_simulator( sim);
    kernel->connect_memory( mem);
    kernel->connect_exception_handler();
    kernel->load_file( TEST_PATH "/mips-tt-no-delayed-branches.bin");
    sim->set_kernel( kernel);
    CHECK_NOTHROW( sim->set_pc( kernel->get_start_pc()) );
    CHECK( sim->get_exit_code() == 0);
}

TEST_CASE( "Make_A_Step: Func_Sim")
{
    auto sim = Simulator::create_functional_simulator("mips32");
    auto mem = FuncMemory::create_default_hierarchied_memory();
    sim->set_memory( mem);
    auto kernel = Kernel::create_dummy_kernel();
    kernel->set_simulator( sim);
    kernel->connect_memory( mem);
    kernel->connect_exception_handler();
    kernel->load_file( TEST_PATH "/mips-tt-no-delayed-branches.bin");
    sim->set_kernel( kernel);

    sim->set_pc( kernel->get_start_pc());
    sim->init_checker();

    CHECK( sim->get_pc() == kernel->get_start_pc());
    sim->run( 1);
    CHECK( sim->get_pc() == kernel->get_start_pc() + 4);
    CHECK( sim->get_exit_code() == 0);
}

TEST_CASE( "Run one instruction: Func_Sim")
{
    auto sim = Simulator::create_functional_simulator("mips32");
    auto mem = FuncMemory::create_default_hierarchied_memory();
    sim->set_memory( mem);

    auto kernel = Kernel::create_dummy_kernel();
    kernel->set_simulator( sim);
    kernel->connect_memory( mem);
    kernel->connect_exception_handler();
    kernel->load_file( TEST_PATH "/mips-tt-no-delayed-branches.bin");
    sim->set_kernel( kernel);

    sim->set_pc( kernel->get_start_pc());

    CHECK( sim->run( 1) == Trap::BREAKPOINT);
    CHECK( sim->get_exit_code() == 0);
}

TEST_CASE( "FuncSim: Register R/W")
{
    auto sim = Simulator::create_functional_simulator("mips32");

    /* Signed */
    sim->write_cpu_register( 1, narrow_cast<uint64>( -1337));
    sim->write_cpu_register( 2, uint64{ MAX_VAL32});

    CHECK( narrow_cast<int32>( sim->read_cpu_register( 1)) == -1337 );
    CHECK( sim->read_cpu_register( 2) == MAX_VAL32 );
}

TEST_CASE( "FuncSim: Register Duplication")
{
    auto sim = Simulator::create_functional_simulator("mips32");
    auto sim2 = Simulator::create_functional_simulator("mips32");

    sim->write_cpu_register( 1, narrow_cast<uint64>( -1337));
    sim->write_cpu_register( 2, uint64{ MAX_VAL32});

    sim->duplicate_all_registers_to( sim2.get());

    CHECK( narrow_cast<int32>( sim->read_cpu_register( 1)) == -1337 );
    CHECK( sim2->read_cpu_register( 2) == MAX_VAL32 );
}

TEST_CASE( "FuncSim: GDB Register R/W")
{
    auto sim = Simulator::create_functional_simulator("mips32");

    sim->write_gdb_register( 1, uint64{ MAX_VAL32});
    CHECK( sim->read_gdb_register( 1) == MAX_VAL32 );
    CHECK( sim->read_gdb_register( 0) == 0 );

    sim->write_gdb_register( 37, 100500);
    CHECK( sim->read_gdb_register( 37) == 100500);
    CHECK( sim->get_pc() == 100500);
}

TEST_CASE( "FuncSim: Register size")
{
    CHECK( Simulator::create_functional_simulator("mips32")->sizeof_register() == bytewidth<uint32>);
    CHECK( Simulator::create_functional_simulator("mips64")->sizeof_register() == bytewidth<uint64>);
}

TEST_CASE( "Run_SMC_trace: Func_Sim")
{
    auto sim = Simulator::create_functional_simulator("mips32");
    auto mem = FuncMemory::create_default_hierarchied_memory();
    sim->set_memory( mem);

    std::ostream nullout( nullptr);
    auto kernel = Kernel::create_mars_kernel( std::cin, nullout, nullout);
    kernel->set_simulator( sim);
    kernel->connect_memory( mem);
    kernel->connect_exception_handler();
    kernel->load_file( TEST_PATH "/mips-tt-no-delayed-branches.bin");
    sim->set_kernel( kernel);

    sim->set_pc( kernel->get_start_pc());

    CHECK_NOTHROW( sim->run_no_limit() );
    CHECK( sim->get_exit_code() == 0);
}

TEST_CASE( "Torture_Test: MIPS32 calls without kernel")
{
    auto sim = Simulator::create_functional_simulator("mips32");
    auto mem = FuncMemory::create_hierarchied_memory( 36, 10, 12);
    sim->set_memory( mem);

    auto kernel = Kernel::create_dummy_kernel();
    kernel->set_simulator( sim);
    kernel->connect_memory( mem);
    kernel->connect_exception_handler();
    kernel->load_file( TEST_PATH "/mips-tt-no-delayed-branches.bin");
    sim->set_kernel( kernel);

    auto start_pc = kernel->get_start_pc();
    sim->set_pc( start_pc);

    CHECK_THROWS_AS( sim->run( 10000), BearingLost);
    CHECK( sim->get_pc() >= 0x8'0000'0180);
    CHECK( sim->read_cpu_register( MIPSRegister::cause().to_rf_index()) != 0);
    auto epc = sim->read_cpu_register( MIPSRegister::epc().to_rf_index());
    CHECK( epc < start_pc);
    CHECK( sim->get_exit_code() == 0);
}

static auto get_simulator_with_test( const std::string& isa, const std::string& test, bool enable_hooks, bool enable_mars)
{
    auto sim = Simulator::create_functional_simulator(isa);
    auto mem = FuncMemory::create_default_hierarchied_memory();
    sim->set_memory( mem);
    if ( enable_hooks)
        sim->enable_driver_hooks();

    std::ostream nullout( nullptr);
    auto kernel = enable_mars
        ? Kernel::create_mars_kernel( std::cin, nullout, nullout)
        : Kernel::create_dummy_kernel();
    kernel->set_simulator( sim);
    kernel->connect_memory( mem);
    kernel->connect_exception_handler();

    sim->set_kernel( kernel);
    kernel->load_file( test);

    sim->set_pc( kernel->get_start_pc());
    return sim;
}

TEST_CASE( "Torture_Test: Stop on trap")
{
    CHECK( get_simulator_with_test("mips32", TEST_PATH "/mips-tt-no-delayed-branches.bin", true, false)->run( 1) == Trap::BREAKPOINT );
    auto trap = get_simulator_with_test("mips32", TEST_PATH "/mips-tt-no-delayed-branches.bin", true, false)->run( 10000);
    CHECK( trap != Trap::NO_TRAP );
    CHECK( trap != Trap::HALT );
}

TEST_CASE( "Torture_Test: MIPS32 calls ")
{
    CHECK( get_simulator_with_test("mips32", TEST_PATH "/mips-tt-no-delayed-branches.bin", false, true)->run( 10000) == Trap::HALT );
}

static bool riscv_tt( const std::string& isa, const std::string& name, bool enable_mars)
{
    auto sim = get_simulator_with_test( isa, name, false, enable_mars);
    auto trap = sim->run_no_limit();
    return trap == Trap::HALT && sim->read_cpu_register( 3) == 1;
}

TEST_CASE( "Torture_Test: integration")
{
    CHECK( get_simulator_with_test("mars",    TEST_PATH "/mips-tt-no-delayed-branches.bin", false, true)->run_no_limit() == Trap::HALT );
    CHECK( get_simulator_with_test("mips32",  TEST_PATH "/mips-tt.bin", false, true)->run_no_limit() == Trap::HALT );
    CHECK( riscv_tt("riscv32", TEST_PATH "/rv32ui-p-simple", false));
    CHECK( riscv_tt("riscv32", TEST_PATH "/rv32ui-p-simple", true));
    CHECK( riscv_tt("riscv64", TEST_PATH "/rv64ui-p-simple", false));
    CHECK( riscv_tt("riscv64", TEST_PATH "/rv64uc-p-rvc", false));
}
