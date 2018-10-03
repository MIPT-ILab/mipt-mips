/**
 * Unit tests for RISCV register
 * @author Alexander Misevich, Vyacheslav Kompan
 * Copyright 2018 MIPT-MIPS
 */

// generic C
#include <cassert>
#include <cstdlib>

// Catch2
#include <catch.hpp>

// MIPT-MIPS modules
#include "../riscv_register.h"

static_assert(RISCVRegister::MAX_REG >= 32);

// Testing methods of the class
TEST_CASE( "RISCV_registers: Constructor")
{
    for ( size_t i = 0; i < 32; ++i)
    {
        CHECK_NOTHROW( RISCVRegister(i));
    }
    CHECK_THROWS_AS( RISCVRegister(32), RISCVRegister::InvalidRegNum);
}

TEST_CASE( "RISCV_registers: Size_t_converters")
{
    for ( size_t i = 0; i < 32; ++i)
    {
        CHECK( RISCVRegister(i).to_size_t() == i);
    }
}

TEST_CASE( "RISCV_registers: Equal")
{
    for ( size_t i = 0; i < 32; ++i)
    {
        CHECK( RISCVRegister(i) == RISCVRegister(i));
        if (i > 0) {
            CHECK(RISCVRegister(i - 1) != RISCVRegister(i));
        }
    }
}

TEST_CASE( "RISCV_registers: no_mips")
{
    auto reg_hi = RISCVRegister::mips_hi;
    auto reg_lo = RISCVRegister::mips_lo;
    for( size_t i = 0; i < 32; ++i)
    {
        // Ensure that there no mips regs
        CHECK( RISCVRegister(i).to_size_t() != reg_hi.to_size_t());
        CHECK( RISCVRegister(i).to_size_t() != reg_lo.to_size_t());
        CHECK_FALSE( RISCVRegister(i).is_mips_hi());
        CHECK_FALSE( RISCVRegister(i).is_mips_lo());
    }
}

TEST_CASE( "RISCV_registers: return_address")
{
    auto reg = RISCVRegister::return_address;
    CHECK( reg.to_size_t() == 1u);
    CHECK_FALSE( reg.is_zero());
    CHECK_FALSE( reg.is_mips_hi());
    CHECK_FALSE( reg.is_mips_lo());
}

TEST_CASE( "RISCV_registers: Zero")
{
    auto reg = RISCVRegister::zero;
    CHECK( reg.is_zero());
    CHECK_FALSE( reg.is_mips_hi());
    CHECK_FALSE( reg.is_mips_lo());
}

