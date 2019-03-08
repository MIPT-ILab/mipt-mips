/**
 * Unit tests for RISCV register
 * @author Alexander Misevich
 * Copyright 2018 MIPT-MIPS
 */

// generic C
#include <cassert>
#include <cstdlib>

// Catch2
#include <catch.hpp>

// MIPT-MIPS modules
#include "../riscv_register.h"

static_assert(RISCVRegister::MAX_REG == 32);

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
    CHECK( reg->to_rf_index() == 1u);
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

