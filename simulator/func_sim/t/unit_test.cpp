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

#include <boost/iostreams/device/null.hpp>
#include <boost/iostreams/stream.hpp>

#include <iostream>

static auto& nullout()
{
    static boost::iostreams::stream<boost::iostreams::null_sink> instance{ boost::iostreams::null_sink{} };
    return instance;
}

TEST_CASE( "Process_Wrong_Args_Of_Constr: Func_Sim_init")
{
    // Just call a constructor
    CHECK_NOTHROW( Simulator::create_functional_simulator("mips32") );
    CHECK_NOTHROW( Simulator::create_functional_simulator("mips32", true) );
    CHECK_THROWS_AS( Simulator::create_functional_simulator("pdp11"), InvalidISA);
}

struct System
{
    std::shared_ptr<Simulator> sim;
    std::shared_ptr<FuncMemory> mem;
    std::shared_ptr<Kernel> kernel;
};

static auto create_funcsim( std::string_view isa, std::string_view test, std::string_view kernel_mode)
{
    auto sim = Simulator::create_functional_simulator( std::string( isa));
    auto mem = FuncMemory::create_default_hierarchied_memory();
    sim->set_memory( mem);
    if ( kernel_mode == "gdb")
        sim->enable_driver_hooks();

    auto kernel = Kernel::create_kernel( kernel_mode == "mars", std::cin, nullout(), nullout());
    kernel->set_simulator( sim);
    kernel->connect_memory( mem);
    kernel->connect_exception_handler();

    sim->set_kernel( kernel);
    if ( !test.empty())
        kernel->load_file( std::string( test));

    sim->set_pc( kernel->get_start_pc());
    sim->disable_checker(); // does nothing

    return System{ sim, mem, kernel};
}

TEST_CASE( "FuncSim: create empty memory and get lost")
{
    CHECK_THROWS_AS(create_funcsim( "mips32", "", "gdb").sim->run( 30), BearingLost);
    CHECK(          create_funcsim( "riscv32", "", "gdb").sim->run( 30) == Trap::UNKNOWN_INSTRUCTION);
    CHECK(          create_funcsim( "riscv128", "", "gdb").sim->run( 30) == Trap::UNKNOWN_INSTRUCTION);
}

TEST_CASE( "FuncSim: get lost without pc")
{
    auto sim = create_funcsim( "mips32", TEST_PATH "/mips/mips-tt-no-delayed-branches.bin", "default").sim;
    sim->set_pc( NO_VAL64);
    
    CHECK_THROWS_AS( sim->run_no_limit(), BearingLost);
    CHECK( sim->get_exit_code() == 0);
}

TEST_CASE( "Process_Wrong_Args_Of_Constr: Func_Sim_init_and_load")
{
    auto system = create_funcsim( "mips32", TEST_PATH "/mips/mips-tt-no-delayed-branches.bin", "gdb");

    CHECK( system.sim->get_exit_code() == 0);
    CHECK( system.sim->get_pc() == system.kernel->get_start_pc());
}

TEST_CASE( "Make_A_Step: Func_Sim")
{
    auto system = create_funcsim( "mips32", TEST_PATH "/mips/mips-tt-no-delayed-branches.bin", "gdb");
    auto trap = system.sim->run( 1);

    CHECK( trap == Trap::BREAKPOINT);
    CHECK( system.sim->get_pc() == system.kernel->get_start_pc() + 4);
    CHECK( system.sim->get_exit_code() == 0);
}

TEST_CASE( "Make_A_Step: Modify_in_flight")
{
    auto system = create_funcsim( "mips32", TEST_PATH "/mips/mips-tt-no-delayed-branches.bin", "gdb");
    system.sim->run( 1);

    system.mem->memset( system.kernel->get_start_pc(), std::byte{}, 4);
    system.sim->set_pc( system.kernel->get_start_pc());
    system.sim->run( 1);

    CHECK( system.sim->get_pc() == system.kernel->get_start_pc() + 4);
    CHECK( system.sim->get_exit_code() == 0); 
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
    auto sim = create_funcsim( "mips32", TEST_PATH "/mips/mips-smc.bin", "gdb").sim;

    CHECK_NOTHROW( sim->run_no_limit() );
    CHECK( sim->get_exit_code() == 0);
}

TEST_CASE( "Torture_Test: MIPS32 calls without kernel")
{
    auto system = create_funcsim( "mips32", TEST_PATH "/mips/mips-tt-no-delayed-branches.bin", "default");

    auto start_pc = system.kernel->get_start_pc();

    CHECK_THROWS_AS( system.sim->run( 10000), BearingLost);
    CHECK( system.sim->get_pc() >= 0x8'0000'0180);
    CHECK( system.sim->read_cpu_register( MIPSRegister::cause().to_rf_index()) != 0);
    auto epc = system.sim->read_cpu_register( MIPSRegister::epc().to_rf_index());
    CHECK( epc < start_pc);
    CHECK( system.sim->get_exit_code() == 0);
}

TEST_CASE( "Torture_Test: Stop on trap")
{
    auto trap = create_funcsim("mips32", TEST_PATH "/mips/mips-tt-no-delayed-branches.bin", "gdb").sim->run( 10000);
    CHECK( trap != Trap::NO_TRAP );
    CHECK( trap != Trap::HALT );
}

TEST_CASE( "Torture_Test: MIPS32 calls ")
{
    CHECK( create_funcsim("mips32", TEST_PATH "/mips/mips-tt-no-delayed-branches.bin", "mars").sim->run( 10000) == Trap::HALT );
}

static bool riscv_tt( std::string_view isa, std::string_view name, std::string_view kernel_mode)
{
    auto sim = create_funcsim( isa, name, kernel_mode).sim;
    auto trap = sim->run_no_limit();
    return trap == Trap::HALT && sim->read_cpu_register( 3) == 1;
}

TEST_CASE( "Torture_Test: integration")
{
    CHECK( create_funcsim("mars",    TEST_PATH "/mips/mips-tt-no-delayed-branches.bin", "mars").sim->run_no_limit() == Trap::HALT );
    CHECK( create_funcsim("mips32",  TEST_PATH "/mips/mips-tt.bin", "mars").sim->run_no_limit() == Trap::HALT );
    CHECK( riscv_tt("riscv32", TEST_PATH "/riscv/rv32ui-p-simple", "default"));
    CHECK( riscv_tt("riscv32", TEST_PATH "/riscv/rv32ui-p-simple", "mars"));
    CHECK( riscv_tt("riscv64", TEST_PATH "/riscv/rv64ui-p-simple", "default"));
    CHECK( riscv_tt("riscv64", TEST_PATH "/riscv/rv64uc-p-rvc", "mars"));
}
