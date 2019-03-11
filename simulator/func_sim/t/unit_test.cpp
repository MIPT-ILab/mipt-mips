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
#include <simulator.h>

#include <sstream>

static const std::string valid_elf_file = TEST_PATH "/tt.core32.out";
static const std::string smc_code = TEST_PATH "/smc.out";

TEST_CASE( "Process_Wrong_Args_Of_Constr: Func_Sim_init")
{
    // Just call a constructor
    CHECK_NOTHROW( Simulator::create_functional_simulator("mips32") );
    CHECK_THROWS_AS( Simulator::create_functional_simulator("pdp11"), InvalidISA);
}

static auto run_over_empty_memory( const std::string& isa)
{
    auto sim = Simulator::create_functional_simulator( isa);
    sim->set_memory( FuncMemory::create_hierarchied_memory());
    return sim->run_no_limit();
}

TEST_CASE( "FuncSim: create empty memory and get lost")
{
    CHECK_THROWS_AS( run_over_empty_memory("mips32"), BearingLost);
    CHECK_THROWS_AS( run_over_empty_memory("riscv32"), UnknownInstruction);
    CHECK_THROWS_AS( run_over_empty_memory("riscv128"), UnknownInstruction);
}

TEST_CASE( "FuncSim: get lost without pc")
{
    auto m   = FuncMemory::create_hierarchied_memory();
    auto sim = Simulator::create_functional_simulator("mips32");
    sim->set_memory( m);
    ElfLoader( valid_elf_file).load_to( m.get());
    CHECK_THROWS_AS( sim->run_no_limit(), BearingLost);
}

TEST_CASE( "Process_Wrong_Args_Of_Constr: Func_Sim_init_and_load")
{
    // Call constructor and init
    auto sim = Simulator::create_functional_simulator("mips32");
    auto mem = FuncMemory::create_hierarchied_memory();
    sim->set_memory( mem);
    ElfLoader elf( valid_elf_file);
    elf.load_to( mem.get());
    CHECK_NOTHROW( sim->set_pc( elf.get_startPC()) );
}

TEST_CASE( "Make_A_Step: Func_Sim")
{
    auto sim = Simulator::create_functional_simulator("mips32");
    auto mem = FuncMemory::create_hierarchied_memory();
    sim->set_memory( mem);
    ElfLoader elf( valid_elf_file);
    elf.load_to( mem.get());
    sim->set_pc( elf.get_startPC());
    sim->init_checker();

    CHECK( sim->get_pc() == elf.get_startPC());
    sim->run( 1);
    CHECK( sim->get_pc() == elf.get_startPC() + 4);
}

TEST_CASE( "FuncSim: make a system-level step")
{
    auto sim = Simulator::create_functional_simulator("mips32");
    auto mem = FuncMemory::create_hierarchied_memory();
    sim->set_memory( mem);
    ElfLoader elf( valid_elf_file);
    elf.load_to( mem.get());
    sim->set_pc( elf.get_startPC());

    CHECK( sim->run_single_step() == Trap::BREAKPOINT);
}

TEST_CASE( "Run one instruction: Func_Sim")
{
    auto sim = Simulator::create_functional_simulator("mips32");
    auto mem = FuncMemory::create_hierarchied_memory();
    sim->set_memory( mem);
    ElfLoader elf( smc_code);
    elf.load_to( mem.get());
    sim->set_pc( elf.get_startPC());

    CHECK( sim->run( 1) == Trap::NO_TRAP);
}

TEST_CASE( "FuncSim: Register R/W")
{
    auto sim = Simulator::create_functional_simulator("mips32");

    /* Signed */
    sim->write_cpu_register( 1, narrow_cast<uint64>( -1337));
    CHECK( narrow_cast<int32>( sim->read_cpu_register( 1)) == -1337 );
    /* Unsigned */
    sim->write_cpu_register( 1, uint64{ MAX_VAL32});
    CHECK( sim->read_cpu_register( 1) == MAX_VAL32 );
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
    auto mem = FuncMemory::create_hierarchied_memory();
    sim->set_memory( mem);
    ElfLoader elf( smc_code);
    elf.load_to( mem.get());
    sim->set_pc( elf.get_startPC());

    CHECK_NOTHROW( sim->run_no_limit() );
}

static auto get_simulator_with_test( const std::string& isa, const std::string& test)
{
    auto sim = Simulator::create_functional_simulator(isa);
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
    CHECK( get_simulator_with_test("mips32", TEST_PATH "/tt.core.universal.out")->run_until_trap( 1) == Trap::NO_TRAP );

    auto trap = get_simulator_with_test("mips32", TEST_PATH "/tt.core.universal.out")->run_until_trap( 10000);
    CHECK( trap != Trap::NO_TRAP );
    CHECK( trap != Trap::HALT );
}

TEST_CASE( "Torture_Test: MARS")
{
    CHECK( get_simulator_with_test("mars", TEST_PATH "/tt.core.universal.out")->run_no_limit() == Trap::HALT );
    CHECK( get_simulator_with_test("mars", TEST_PATH "/tt.core32.le.out")->run_no_limit() == Trap::HALT );
    CHECK( get_simulator_with_test("mars64", TEST_PATH "/tt.core64.le.out")->run_no_limit() == Trap::HALT );
}

TEST_CASE( "Torture_Test: Delayed branches")
{
    CHECK( get_simulator_with_test("mips32", TEST_PATH "/tt.core.universal_reorder.out")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("mips32", TEST_PATH "/tt.core32.le_reorder.out")->run_no_limit() == Trap::HALT  );
}

static bool riscv_torture_test_passes( const std::string& isa, const std::string& test)
{
    auto sim = get_simulator_with_test(isa, test);
    auto res = sim->run( 10000);
    return res == Trap::HALT && sim->read_cpu_register( 3 /* gp */) == 1;
}

TEST_CASE( "Torture_Test: rvc32ui")
{
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-add")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-addi")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-and")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-andi")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-auipc")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-beq")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-bge")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-bgeu")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-blt")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-bltu")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-bne")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-fence_i")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-jal")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-jalr")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-lb")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-lbu")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-lh")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-lhu")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-lui")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-lw")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-or")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-ori")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-sb")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-sh")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-simple")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-sll")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-slli")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-slt")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-slti")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-sltiu")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-sltu")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-sra")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-srai")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-srl")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-srli")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-sub")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-sw")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-xor")  );
    CHECK( riscv_torture_test_passes("riscv32", RISCV_TEST_PATH "/isa/rv32ui-p-xori")  );
}

TEST_CASE( "Torture_Test: rvc64ui")
{
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-add")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-addi")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-addiw")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-addw")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-and")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-andi")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-auipc")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-beq")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-bge")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-bgeu")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-blt")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-bltu")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-bne")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-fence_i")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-jal")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-jalr")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-lb")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-lbu")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-ld")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-lh")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-lhu")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-lui")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-lw")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-lwu")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-or")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-ori")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-sb")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-sd")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-sh")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-simple")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-sll")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-slli")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-slliw")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-sllw")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-slt")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-slti")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-sltiu")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-sltu")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-sra")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-srai")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-sraiw")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-sraw")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-srl")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-srli")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-srliw")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-srlw")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-sub")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-subw")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-sw")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-xor")->run_no_limit() == Trap::HALT  );
    CHECK( get_simulator_with_test("riscv64", RISCV_TEST_PATH "/isa/rv64ui-p-xori")->run_no_limit() == Trap::HALT  );
}
