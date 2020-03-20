/*
 * RISC-V 32 driver unit-tests
 * Author Eric Konks
 * Copyright 2020 MIPT-V
*/

#include <catch.hpp>
#include <risc_v/risc_v.h>
#include <simulator.h>

static auto get_op_with_trap( Trap trap)
{
    Operation op( 0x100, 0x104);
    op.set_trap( trap);
    return op;
}

TEST_CASE("RISCV32 driver - no trap")
{
    auto sim = Simulator::create_simulator( "riscv32", true);
    auto drv = create_riscv32_driver( sim.get());
    auto expected_pc = sim->get_pc();
    CHECK( drv->handle_trap( get_op_with_trap( Trap( Trap::NO_TRAP))) == Trap::NO_TRAP);
    CHECK( sim->get_pc() == expected_pc);
}

TEST_CASE("RISCV32 driver - breakpoint")
{
    auto sim = Simulator::create_simulator( "riscv32", true);
    auto drv = create_riscv32_driver( sim.get());
    sim->write_csr_register( "stvec", 0x8000);
    CHECK( drv->handle_trap( get_op_with_trap( Trap( Trap::BREAKPOINT))) == Trap::NO_TRAP);
    auto expected_pc = trap_vector_address<Addr>( sim->read_csr_register( "stvec"));
    CHECK( sim->get_pc() == expected_pc);
    auto expected_cause = Trap( Trap::BREAKPOINT).to_riscv_format();
    CHECK( sim->read_csr_register( "scause") == expected_cause);
}
