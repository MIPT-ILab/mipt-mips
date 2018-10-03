/**
 * Unit tests for MIPS register
 * @author Alexander Misevich, Vyacheslav Kompan
 * Copyright 2018 MIPT-MIPS
 */

// generic C
#include <cassert>
#include <cstdlib>

// Catch2
#include <catch.hpp>

// MIPT-MIPS modules
#include "../mips_register.h"

static_assert(MIPSRegister::MAX_REG >= 32);

// Testing methods of the class
TEST_CASE( "MIPS_registers: Constructor")
{
    for ( size_t i = 0; i < 32; ++i)
    {
        CHECK_NOTHROW( MIPSRegister(i));
    }
    CHECK_THROWS_AS( MIPSRegister(32), MIPSRegister::InvalidRegNum);
}

TEST_CASE( "MIPS_registers: Size_t_converters")
{
    for ( size_t i = 0; i < 32; ++i)
        CHECK(MIPSRegister(i).to_size_t() == i);
}

TEST_CASE( "MIPS_registers: Equal")
{
    for ( size_t i = 0; i < 32; ++i)
    {
        CHECK(MIPSRegister(i) == MIPSRegister(i));
        if (i > 0) {
            CHECK(MIPSRegister(i - 1) != MIPSRegister(i));
        }
    }
}

TEST_CASE( "MIPS_registers: Hi_Lo_impossible")
{
    for ( size_t i = 0; i < 32; ++i)
    {
        MIPSRegister reg(i);
        CHECK_FALSE(reg.is_mips_hi());
        CHECK_FALSE(reg.is_mips_lo());
    }
}

TEST_CASE( "MIPS_registers: Zero")
{
    auto reg = MIPSRegister::zero;
    CHECK(reg.is_zero());
    CHECK_FALSE(reg.is_mips_hi());
    CHECK_FALSE(reg.is_mips_lo());
    CHECK(reg.to_size_t() == 0);
}

TEST_CASE( "MIPS_registers: Return_address")
{
    auto reg = MIPSRegister::return_address;
    CHECK_FALSE(reg.is_zero());
    CHECK_FALSE(reg.is_mips_hi());
    CHECK_FALSE(reg.is_mips_lo());
    CHECK(reg.to_size_t() == 31);
}

TEST_CASE( "MIPS_registers: Hi_register")
{
    auto reg = MIPSRegister::mips_hi;
    CHECK_FALSE(reg.is_zero());
    CHECK(reg.is_mips_hi());
    CHECK_FALSE(reg.is_mips_lo());
    CHECK_FALSE(reg.to_size_t() < 32);
}

TEST_CASE( "MIPS_registers: Lo_register")
{
    auto reg = MIPSRegister::mips_lo;
    CHECK_FALSE(reg.is_zero());
    CHECK_FALSE(reg.is_mips_hi());
    CHECK(reg.is_mips_lo());
    CHECK_FALSE(reg.to_size_t() < 32);
}

