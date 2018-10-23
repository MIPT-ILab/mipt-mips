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

TEST_CASE( "PerfSim: create empty memory and get lost")
{
    auto m = FuncMemory::create_hierarchied_memory();
    PerfSim<MIPS32> sim( false);
    sim.set_memory( m.get());
    CHECK_THROWS_AS( sim.run_no_limit(), Deadlock);
}

// TODO: remove that class, use true FuncSim interfaces instead
template <typename ISA>
struct PerfSimAndMemory : PerfSim<ISA>
{
    std::unique_ptr<FuncMemory> mem;

    PerfSimAndMemory(bool log) : PerfSim<ISA>(log), mem( FuncMemory::create_hierarchied_memory())
    {
       this->set_memory( mem.get());
    }

    void init_trace( const std::string& tr) {
        ElfLoader elf( tr);
        elf.load_to( mem.get());
        this->init_checker();
        this->set_pc( elf.get_startPC());
    }

    auto run_trace( const std::string& tr, uint64 steps) {
        init_trace( tr);
        return PerfSim<ISA>::run( steps);
    }

    auto run_trace_no_limit( const std::string& tr) {
        init_trace( tr);
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

TEST_CASE( "Perf_Sim: Register R/W")
{
    PerfSim<MIPS32> sim( false);

    /* Signed */
    sim.write_cpu_register( 1, static_cast<uint64>( -1337));
    CHECK( static_cast<int32>( sim.read_cpu_register( 1)) == -1337 );
    /* Unsigned */
    sim.write_cpu_register( 1, static_cast<uint64>( MAX_VAL32));
    CHECK( sim.read_cpu_register( 1) == MAX_VAL32 );
}

TEST_CASE( "Perf_Sim: Register size")
{
    CHECK( PerfSim<MIPS32>( false).sizeof_register() == bytewidth<uint32>);
    CHECK( PerfSim<MIPS64>( false).sizeof_register() == bytewidth<uint64>);
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

TEST_CASE( "Perf_Sim: Run_SMC_Trace_WithoutChecker")
{
    PerfSim<MIPS32> sim( false);
    auto mem = FuncMemory::create_hierarchied_memory();
    sim.set_memory( mem.get());
    ElfLoader elf( smc_code);
    elf.load_to( mem.get());
    sim.set_pc( elf.get_startPC());
    CHECK( sim.run_no_limit( ) == Trap::NO_TRAP);
}

TEST_CASE( "Perf_Sim: Run_SMC_Trace_WithChecker")
{
    CHECK_THROWS_AS( PerfSimAndMemory<MIPS32>( false).run_trace_no_limit( smc_code), CheckerMismatch);
}

