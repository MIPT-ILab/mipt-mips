/*
 * Unit testing for extremely simple simulator
 * Copyright 2018 MIPT-MIPS
 */

#include "../func_sim.h"

// Catch2
#include <catch.hpp>

// Module
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

TEST_CASE( "FuncSim: Register R/W")
{
    FuncSim<MIPS32> sim;

    /* Signed */
    sim.write_cpu_register( 1, static_cast<uint64>( -1337));
    CHECK( static_cast<int32>( sim.read_cpu_register( 1)) == -1337 );
    /* Unsigned */
    sim.write_cpu_register( 1, static_cast<uint64>( MAX_VAL32));
    CHECK( sim.read_cpu_register( 1) == MAX_VAL32 );
}

TEST_CASE( "FuncSim: Register size")
{
    CHECK( FuncSim<MIPS32>().sizeof_register() == bytewidth<uint32>);
    CHECK( FuncSim<MIPS64>().sizeof_register() == bytewidth<uint64>);
}
