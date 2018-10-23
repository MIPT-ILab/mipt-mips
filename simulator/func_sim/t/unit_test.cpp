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
    sim.set_memory( m.get());
    CHECK_THROWS_AS( sim.run_no_limit(), BearingLost);
}

// TODO: remove that class, use true FuncSim interfaces instead
template <typename ISA>
struct FuncSimAndMemory : FuncSim<ISA>
{
    std::unique_ptr<FuncMemory> mem;

    FuncSimAndMemory() : FuncSim<ISA>(), mem( FuncMemory::create_hierarchied_memory())
    {
       this->set_memory( mem.get());
    }

    void init_trace( const std::string& tr) {
        ElfLoader elf( tr);
        elf.load_to( mem.get());
        this->set_pc( elf.get_startPC());
    }

    auto run_trace( const std::string& tr, uint64 steps) {
        init_trace( tr);
        return FuncSim<ISA>::run( steps);
    }

    auto run_trace_no_limit( const std::string& tr) {
        init_trace( tr);
        return FuncSim<ISA>::run_no_limit();
    }
};

TEST_CASE( "FuncSim: get lost without pc")
{
    auto m = FuncMemory::create_hierarchied_memory();
    FuncSim<MIPS32> sim( false);
    sim.set_memory( m.get());
    ElfLoader( valid_elf_file).load_to( m.get());
    CHECK_THROWS_AS( sim.run_no_limit(), BearingLost);
}

TEST_CASE( "Process_Wrong_Args_Of_Constr: Func_Sim_init_and_load")
{
    // Call constructor and init
    CHECK_NOTHROW( FuncSimAndMemory<MIPS32>().init_trace( valid_elf_file) );
}

TEST_CASE( "Make_A_Step: Func_Sim")
{
    FuncSimAndMemory<MIPS32> simulator;
    simulator.init_trace( valid_elf_file);
    CHECK( simulator.step().string_dump().find("lui $at, 0x41\t [ $at = 0x410000 ]") != std::string::npos);
}

TEST_CASE( "FuncSim: make a step with checker")
{
    FuncSimAndMemory<MIPS32> simulator;
    simulator.init_trace( valid_elf_file);
    simulator.init_checker();
    CHECK( simulator.step().string_dump().find("lui $at, 0x41\t [ $at = 0x410000 ]") != std::string::npos);
}

TEST_CASE( "Run one instruction: Func_Sim")
{
    CHECK( FuncSimAndMemory<MIPS32>().run_trace( smc_code, 1) == Trap::NO_TRAP);
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
    FuncSim<MIPS32> simMIPS32;
    FuncSim<MIPS64> simMIPS64;

    CHECK( simMIPS32.sizeof_register() == bytewidth<uint32>);
    CHECK( simMIPS64.sizeof_register() == bytewidth<uint64>);
}

TEST_CASE( "Run_SMC_trace: Func_Sim")
{
    CHECK_NOTHROW( FuncSimAndMemory<MIPS32>().run_trace_no_limit( smc_code));
}

TEST_CASE( "Torture_Test: Func_Sim")
{
    // MIPS 32 Little-endian
    CHECK_NOTHROW( FuncSimAndMemory<MIPS32>().run_trace_no_limit( TEST_PATH "/tt.core.universal.out") );
    CHECK_NOTHROW( FuncSimAndMemory<MIPS32>().run_trace_no_limit( TEST_PATH "/tt.core32.out") );
    CHECK_NOTHROW( FuncSimAndMemory<MIPS32>().run_trace_no_limit( TEST_PATH "/tt.core32.le.out") );

    // MIPS 64 Little-Endian
    CHECK_NOTHROW( FuncSimAndMemory<MIPS64>().run_trace_no_limit( TEST_PATH "/tt.core.universal.out") );
    CHECK_NOTHROW( FuncSimAndMemory<MIPS64>().run_trace_no_limit( TEST_PATH "/tt.core64.out") );
    CHECK_NOTHROW( FuncSimAndMemory<MIPS64>().run_trace_no_limit( TEST_PATH "/tt.core64.le.out") );
}
