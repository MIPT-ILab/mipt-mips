/**
 * Unit tests for RISCV register
 * @author Alexander Misevich
 * Copyright 2018 MIPT-MIPS
 */

// Catch2
#include <catch.hpp>

// MIPT-MIPS modules
#include <risc_v/riscv_register/riscv_register.h>

#include <cassert>
#include <cstdlib>

// Testing methods of the class
TEST_CASE( "RISCV_registers: Size_t_converters")
{
    for ( uint8 i = 0; i < 32; ++i)
    {
        CHECK( RISCVRegister::from_cpu_index( i).to_rf_index() == i);
    }
}

TEST_CASE( "RISCV_registers: Equal")
{
    for ( uint8 i = 0; i < 32; ++i)
    {
        CHECK( RISCVRegister::from_cpu_index( i) == RISCVRegister::from_cpu_index( i));
        if (i > 0) {
            CHECK( RISCVRegister::from_cpu_index(i - 1) != RISCVRegister::from_cpu_index( i));
        }
    }
}

TEST_CASE( "RISCV_registers: no_mips")
{
    // Allocate on heap to prevent constexprizing everything
    // so the coverage is reported correctly
    auto reg_hi = std::make_unique<RISCVRegister>(RISCVRegister::mips_hi());
    auto reg_lo = std::make_unique<RISCVRegister>(RISCVRegister::mips_lo());
    for( uint8 i = 0; i < 32; ++i)
    {
        // Ensure that there are no mips regs
        CHECK( RISCVRegister::from_cpu_index( i).to_rf_index() != reg_hi->to_rf_index());
        CHECK( RISCVRegister::from_cpu_index( i).to_rf_index() != reg_lo->to_rf_index());
        CHECK_FALSE( RISCVRegister::from_cpu_index( i).is_mips_hi());
        CHECK_FALSE( RISCVRegister::from_cpu_index( i).is_mips_lo());
    }
}

TEST_CASE( "RISCV_registers: return_address")
{
    auto reg = std::make_unique<RISCVRegister>(RISCVRegister::return_address());
    CHECK( reg->to_rf_index() == 1U);
    CHECK_FALSE( reg->is_zero());
    CHECK_FALSE( reg->is_mips_hi());
    CHECK_FALSE( reg->is_mips_lo());
}

TEST_CASE( "RISCV_registers: Zero")
{
    auto reg = RISCVRegister::zero();
    CHECK( reg.is_zero());
    CHECK_FALSE( reg.is_mips_hi());
    CHECK_FALSE( reg.is_mips_lo());
}

TEST_CASE( "RISCV_registers: CSR")
{
    auto reg = RISCVRegister::from_csr_index(0x305);
    CHECK_FALSE( reg.is_zero());
    CHECK_FALSE( reg.is_mips_hi());
    CHECK_FALSE( reg.is_mips_lo());
    CHECK( reg.dump() == "mtvec");
}

TEST_CASE( "RISCV_registers: invalid CSR")
{
    auto reg = RISCVRegister::from_csr_index(0x8789);
    CHECK_FALSE( reg.is_zero());
    CHECK_FALSE( reg.is_mips_hi());
    CHECK_FALSE( reg.is_mips_lo());
    CHECK_FALSE( reg.is_valid());
}

TEST_CASE( "RISCV_registers: CSR by name")
{
    auto reg = RISCVRegister::from_csr_name( "mscratch");
    CHECK( reg.is_valid());
    CHECK( reg == RISCVRegister::from_csr_index(0x340));
}

TEST_CASE( "RISCV_registers: CSR by invalid name")
{
    auto reg = RISCVRegister::from_csr_name( "balalaika");
    CHECK( !reg.is_valid());
}

TEST_CASE( "RISCV_registers: GDB interface")
{
    CHECK( RISCVRegister::from_gdb_index( 2).dump() == "sp");
    CHECK( RISCVRegister::get_gdb_pc_index() == 37);
}
