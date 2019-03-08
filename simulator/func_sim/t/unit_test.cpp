/*
 * Unit testing for extremely simple simulator
 * Copyright 2018 MIPT-MIPS
 */

#include "../func_sim.h"

// Catch2
#include <catch.hpp>

// Module
#include <kernel/kernel.h>
#include <kernel/mars/mars_kernel.h>
#include <memory/elf/elf_loader.h>
#include <memory/memory.h>
#include <mips/mips.h>
#include <risc_v/risc_v.h>

#include <sstream>

static const std::string valid_elf_file = TEST_PATH "/tt.core32.out";
static const std::string smc_code = TEST_PATH "/smc.out";

TEST_CASE( "Process_Wrong_Args_Of_Constr: Func_Sim_init")
{
    // Just call a constructor
    CHECK_NOTHROW( FuncSim<MIPS32>() );
}

TEST_CASE( "FuncSim MIPS32: create empty memory and get lost")
{
    FuncSim<MIPS32> sim( false);
    sim.set_memory( FuncMemory::create_hierarchied_memory());
    CHECK_THROWS_AS( sim.run_no_limit(), BearingLost);
}

TEST_CASE( "FuncSim RISC-V: create empty memory and get lost")
{
    FuncSim<RISCV32> sim( false);
    sim.set_memory( FuncMemory::create_hierarchied_memory());
    CHECK_THROWS_AS( sim.run_no_limit(), BearingLost);
}

TEST_CASE( "FuncSim RISC-V 128 bit: create empty memory and get lost")
{
    FuncSim<RISCV128> sim( false);
    sim.set_memory( FuncMemory::create_hierarchied_memory());
    CHECK_THROWS_AS( sim.run_no_limit(), BearingLost);
}

TEST_CASE( "FuncSim: get lost without pc")
{
    auto m = FuncMemory::create_hierarchied_memory();
    FuncSim<MIPS32> sim( false);
    sim.set_memory( m);
    ElfLoader( valid_elf_file).load_to( m.get());
    CHECK_THROWS_AS( sim.run_no_limit(), BearingLost);
}

TEST_CASE( "Process_Wrong_Args_Of_Constr: Func_Sim_init_and_load")
{
    // Call constructor and init
    FuncSim<MIPS32> sim;
    auto mem = FuncMemory::create_hierarchied_memory();
    sim.set_memory( mem);
    ElfLoader elf( valid_elf_file);
    elf.load_to( mem.get());
    CHECK_NOTHROW( sim.set_pc( elf.get_startPC()) );
}

TEST_CASE( "Make_A_Step: Func_Sim")
{
    FuncSim<MIPS32> sim;
    auto mem = FuncMemory::create_hierarchied_memory();
    sim.set_memory( mem);
    ElfLoader elf( valid_elf_file);
    elf.load_to( mem.get());
    sim.set_pc( elf.get_startPC());

    CHECK( sim.get_pc() == elf.get_startPC());
    CHECK( sim.step().string_dump().find("lui $at, 0x41\t [ $at = 0x410000 ]") != std::string::npos);
    CHECK( sim.get_pc() == elf.get_startPC() + 4);
}

TEST_CASE( "FuncSim: make a step with checker")
{
    FuncSim<MIPS32> sim;
    auto mem = FuncMemory::create_hierarchied_memory();
    sim.set_memory( mem);
    ElfLoader elf( valid_elf_file);
    elf.load_to( mem.get());
    sim.set_pc( elf.get_startPC());
    sim.init_checker();

    CHECK( sim.step().string_dump().find("lui $at, 0x41\t [ $at = 0x410000 ]") != std::string::npos);
}

TEST_CASE( "FuncSim: make a system-level step")
{
    FuncSim<MIPS32> sim;
    auto mem = FuncMemory::create_hierarchied_memory();
    sim.set_memory( mem);
    ElfLoader elf( valid_elf_file);
    elf.load_to( mem.get());
    sim.set_pc( elf.get_startPC());

    CHECK( sim.run_single_step() == Trap::BREAKPOINT);
}

TEST_CASE( "Run one instruction: Func_Sim")
{
    FuncSim<MIPS32> sim;
    auto mem = FuncMemory::create_hierarchied_memory();
    sim.set_memory( mem);
    ElfLoader elf( smc_code);
    elf.load_to( mem.get());
    sim.set_pc( elf.get_startPC());

    CHECK( sim.run( 1) == Trap::NO_TRAP);
}

TEST_CASE( "FuncSim: Register R/W")
{
    FuncSim<MIPS32> sim;

    /* Signed */
    sim.write_cpu_register( 1, narrow_cast<uint64>( -1337));
    CHECK( narrow_cast<int32>( sim.read_cpu_register( 1)) == -1337 );
    /* Unsigned */
    sim.write_cpu_register( 1, uint64{ MAX_VAL32});
    CHECK( sim.read_cpu_register( 1) == MAX_VAL32 );
}

TEST_CASE( "FuncSim: GDB Register R/W")
{
    FuncSim<MIPS32> sim;

    sim.write_gdb_register( 1, uint64{ MAX_VAL32});
    CHECK( sim.read_gdb_register( 1) == MAX_VAL32 );
    CHECK( sim.read_gdb_register( 0) == 0 );

    sim.write_gdb_register( 37, 100500);
    CHECK( sim.read_gdb_register( 37) == 100500);
    CHECK( sim.get_pc() == 100500);
}

TEST_CASE( "FuncSim: Register size")
{
    CHECK( FuncSim<MIPS32>().sizeof_register() == bytewidth<uint32>);
    CHECK( FuncSim<MIPS64>().sizeof_register() == bytewidth<uint64>);
}

TEST_CASE( "Run_SMC_trace: Func_Sim")
{
    FuncSim<MIPS32> sim;
    auto mem = FuncMemory::create_hierarchied_memory();
    sim.set_memory( mem);
    ElfLoader elf( smc_code);
    elf.load_to( mem.get());
    sim.set_pc( elf.get_startPC());

    CHECK_NOTHROW( sim.run_no_limit() );
}

template<typename ISA>
auto get_simulator_with_test( const std::string& test)
{
    // MIPS 32 Little-endian
    auto sim = std::make_shared<FuncSim<ISA>>();
    auto mem = FuncMemory::create_hierarchied_memory();
    sim->set_memory( mem);

    ElfLoader elf( test);
    elf.load_to( mem.get());

    auto kernel = create_mars_kernel();
    kernel->set_memory( mem);
    kernel->set_simulator( sim);
    sim->set_kernel( kernel);

    sim->set_pc( elf.get_startPC());
    return sim;
}

TEST_CASE( "Torture_Test: Stop on trap")
{
    CHECK( get_simulator_with_test<MIPS32>( TEST_PATH "/tt.core.universal.out")->run_until_trap( 1) == Trap::NO_TRAP );

    auto trap = get_simulator_with_test<MIPS32>( TEST_PATH "/tt.core.universal.out")->run_until_trap( 10000);
    CHECK( trap != Trap::NO_TRAP );
    CHECK( trap != Trap::HALT );
}

TEST_CASE( "Torture_Test: MARS")
{
    CHECK_NOTHROW( get_simulator_with_test<MARS>( TEST_PATH "/tt.core.universal.out")->run_no_limit() );
    CHECK_NOTHROW( get_simulator_with_test<MARS>( TEST_PATH "/tt.core32.le.out")->run_no_limit() );
    CHECK_NOTHROW( get_simulator_with_test<MARS64>( TEST_PATH "/tt.core64.le.out")->run_no_limit() );
}

TEST_CASE( "Torture_Test: Delayed branches")
{
    CHECK_NOTHROW( get_simulator_with_test<MIPS32>( TEST_PATH "/tt.core.universal_reorder.out")->run_no_limit() );
    CHECK_NOTHROW( get_simulator_with_test<MIPS32>( TEST_PATH "/tt.core32.le_reorder.out")->run_no_limit() );
}
