/*
 * Unit testing for extremely simple simulator
 * Copyright 2018 MIPT-MIPS
 */

#include "../func_sim.h"

// Catch2
#include <catch.hpp>

// Module
#include <kernel/kernel.h>
#include <memory/elf/elf_loader.h>
#include <memory/memory.h>
#include <mips/mips.h>

#include <sstream>

static const std::string valid_elf_file = TEST_PATH "/tt.core32.out";
static const std::string smc_code = TEST_PATH "/smc.out";

TEST_CASE( "Process_Wrong_Args_Of_Constr: Func_Sim_init")
{
    // Just call a constructor
    CHECK_NOTHROW( FuncSim<MIPS32>() );
}

TEST_CASE( "FuncSim: create empty memory and get lost")
{
    auto m = FuncMemory::create_hierarchied_memory();
    FuncSim<MIPS32> sim( false);
    sim.set_memory( m);
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

    CHECK( sim.step().string_dump().find("lui $at, 0x41\t [ $at = 0x410000 ]") != std::string::npos);
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

TEST_CASE( "Torture_Test: Func_Sim")
{
    // MIPS 32 Little-endian
    FuncSim<MIPS32> simMIPS32;
    auto mem = FuncMemory::create_hierarchied_memory();
    simMIPS32.set_memory( mem);

    ElfLoader elfCoreUniversal( TEST_PATH "/tt.core.universal.out");
    elfCoreUniversal.load_to( mem.get());
    simMIPS32.set_pc( elfCoreUniversal.get_startPC());
    CHECK_NOTHROW( simMIPS32.run_no_limit() );

    ElfLoader elfCore32( TEST_PATH "/tt.core32.out");
    elfCore32.load_to( mem.get());
    simMIPS32.set_pc( elfCore32.get_startPC());
    CHECK_NOTHROW( simMIPS32.run_no_limit() );

    ElfLoader elfCore32le( TEST_PATH "/tt.core32.le.out");
    elfCore32le.load_to( mem.get());
    simMIPS32.set_pc( elfCore32le.get_startPC());
    CHECK_NOTHROW( simMIPS32.run_no_limit() );

    // MIPS 64 Little-Endian
    FuncSim<MIPS64> simMIPS64;
    simMIPS64.set_memory( mem);

    elfCoreUniversal.load_to( mem.get());
    simMIPS64.set_pc( elfCoreUniversal.get_startPC());
    CHECK_NOTHROW( simMIPS64.run_no_limit() );

    ElfLoader elfCore64( TEST_PATH "/tt.core64.out");
    elfCore64.load_to( mem.get());
    simMIPS64.set_pc( elfCore64.get_startPC());
    CHECK_NOTHROW( simMIPS64.run_no_limit() );

    ElfLoader elfCore64le( TEST_PATH "/tt.core64.le.out");
    elfCore64le.load_to( mem.get());
    simMIPS64.set_pc( elfCore64le.get_startPC());
    CHECK_NOTHROW( simMIPS64.run_no_limit() );
}
