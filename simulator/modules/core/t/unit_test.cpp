/**
 * Test for Performance Simulation
 * Copyright 2018 MIPT-MIPS
 */

#include "../perf_sim.h"

// Catch2
#include <catch.hpp>

// Module
#include <memory/elf/elf_loader.h>
#include <mips/mips.h>
#include <modules/writeback/writeback.h>

static const std::string valid_elf_file = TEST_PATH "/tt.core32.out";
static const std::string smc_code = TEST_PATH "/smc.out";

// TODO: remove that class, use true FuncSim interfaces instead
template <typename ISA>
struct PerfSimAndMemory : PerfSim<ISA>
{
    std::unique_ptr<FuncMemory> mem;

    PerfSimAndMemory(bool log) : PerfSim<ISA>(log), mem( FuncMemory::create_hierarchied_memory())
    {
       this->set_memory( mem.get());
    }

    auto run_trace( const std::string& tr, uint64 steps) {
        ::load_elf_file( mem.get(), tr);
        this->init_checker();
        return PerfSim<ISA>::run( steps);
    }

    auto run_trace_no_limit( const std::string& tr) {
        ::load_elf_file( mem.get(), tr);
        this->init_checker();
        return PerfSim<ISA>::run_no_limit();
    }
};

TEST_CASE( "Perf_Sim_init: Process_Correct_Args_Of_Constr")
{
    // Just call a constructor
    CHECK_NOTHROW( PerfSimAndMemory<MIPS32>( false) );
}

TEST_CASE( "Perf_Sim_init: Make_A_Step")
{
    // Call constructor and run one instr
    CHECK_NOTHROW( PerfSimAndMemory<MIPS32>( false).run_trace( valid_elf_file, 1) );
}

TEST_CASE( "Perf_Sim_init: Process_Wrong_Args")
{
    // Do bad init
    CHECK_THROWS_AS( PerfSimAndMemory<MIPS32>( false).run_trace( "./1234567890/qwertyuop", 1), InvalidElfFile);
}

TEST_CASE( "Torture_Test: Perf_Sim")
{
    // MIPS 32 Little-Endian
    CHECK_NOTHROW( PerfSimAndMemory<MIPS32>( false).run_trace_no_limit( TEST_PATH "/tt.core.universal.out") );
    CHECK_NOTHROW( PerfSimAndMemory<MIPS32>( false).run_trace_no_limit( TEST_PATH "/tt.core32.out") );
    CHECK_NOTHROW( PerfSimAndMemory<MIPS32>( false).run_trace_no_limit( TEST_PATH "/tt.core32.le.out") );

    // MIPS 64 Little-Endian
    CHECK_NOTHROW( PerfSimAndMemory<MIPS64>( false).run_trace_no_limit( TEST_PATH "/tt.core.universal.out") );
    CHECK_NOTHROW( PerfSimAndMemory<MIPS64>( false).run_trace_no_limit( TEST_PATH "/tt.core64.out") );
    CHECK_NOTHROW( PerfSimAndMemory<MIPS64>( false).run_trace_no_limit( TEST_PATH "/tt.core64.le.out") );
}

TEST_CASE( "Perf_Sim: Run_SMC_Trace")
{
    CHECK_THROWS_AS( PerfSimAndMemory<MIPS32>( false).run_trace_no_limit( smc_code), CheckerMismatch);
}

