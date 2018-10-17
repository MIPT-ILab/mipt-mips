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

// TODO: remove that class, use true FuncSim interfaces instead
template <typename ISA>
struct FuncSimAndMemory : FuncSim<ISA>
{
    std::unique_ptr<FuncMemory> mem;

    FuncSimAndMemory() : FuncSim<ISA>(), mem( FuncMemory::create_hierarchied_memory())
    {
       this->set_memory( mem.get());
    }

    void init( const std::string& tr) {
        ::load_elf_file( mem.get(), tr);
        FuncSim<ISA>::init();
    }

    auto run_trace( const std::string& tr, uint64 steps) {
        ::load_elf_file( mem.get(), tr);
        return FuncSim<ISA>::run( steps);
    }

    auto run_trace_no_limit( const std::string& tr) {
        ::load_elf_file( mem.get(), tr);
        return FuncSim<ISA>::run_no_limit();
    }
};

TEST_CASE( "Process_Wrong_Args_Of_Constr: Func_Sim_init")
{
    // Just call a constructor
    CHECK_NOTHROW( FuncSimAndMemory<MIPS32>() );

    // Call constructor and init
    CHECK_NOTHROW( FuncSimAndMemory<MIPS32>().init( valid_elf_file) );

    // Do bad init
    CHECK_THROWS_AS( FuncSimAndMemory<MIPS32>().init( "./1234567890/qwertyuop"), InvalidElfFile);
}

TEST_CASE( "Make_A_Step: Func_Sim")
{
    FuncSimAndMemory<MIPS32> simulator;
    simulator.init( valid_elf_file);
    CHECK( simulator.step().string_dump().find("lui $at, 0x41\t [ $at = 0x410000 ]") != std::string::npos);
}

TEST_CASE( "FuncSim: make a step with checker")
{
    FuncSimAndMemory<MIPS32> simulator;
    simulator.init( valid_elf_file);
    simulator.init_checker();
    CHECK( simulator.step().string_dump().find("lui $at, 0x41\t [ $at = 0x410000 ]") != std::string::npos);
}

TEST_CASE( "Run one instruction: Func_Sim")
{
    CHECK( FuncSimAndMemory<MIPS32>().run_trace( smc_code, 1) == Trap::NO_TRAP);
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
