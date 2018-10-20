/**
 * Unit tests for MIPS register
 * @author Alexander Misevich
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
TEST_CASE( "MIPS_registers: ID_converters")
{
    for ( size_t i = 0; i < 32; ++i)
        CHECK( MIPSRegister::from_cpu_index(i).to_rf_index() == i);
}

TEST_CASE( "MIPS_registers: GDB_ID_converter")
{
    for ( size_t i = 0; i < 32; ++i)
        CHECK( MIPSRegister::from_gdb_index(i) == MIPSRegister::from_cpu_index(i));
    CHECK( MIPSRegister::from_gdb_index(32) == MIPSRegister::from_cp0_index(12));  // SR
    CHECK( MIPSRegister::from_gdb_index(33) == MIPSRegister::mips_lo);
    CHECK( MIPSRegister::from_gdb_index(34) == MIPSRegister::mips_hi);
    CHECK( MIPSRegister::from_gdb_index(35) == MIPSRegister::from_cp0_index( 8));  // Bad
    CHECK( MIPSRegister::from_gdb_index(36) == MIPSRegister::from_cp0_index( 13)); // Cause
}

TEST_CASE( "MIPS_registers: Equal")
{
    for ( size_t i = 0; i < 32; ++i)
    {
        CHECK(MIPSRegister::from_cpu_index(i) == MIPSRegister::from_cpu_index(i));
        if (i > 0) {
            CHECK(MIPSRegister::from_cpu_index(i - 1) != MIPSRegister::from_cpu_index(i));
        }
    }
}

TEST_CASE( "MIPS_registers: Hi_Lo_impossible")
{
    for ( size_t i = 0; i < 32; ++i)
    {
        auto reg = MIPSRegister::from_cpu_index(i);
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
    CHECK(reg.to_rf_index() == 0);
}

TEST_CASE( "MIPS_registers: Return_address")
{
    auto reg = MIPSRegister::return_address;
    CHECK_FALSE(reg.is_zero());
    CHECK_FALSE(reg.is_mips_hi());
    CHECK_FALSE(reg.is_mips_lo());
    CHECK(reg.to_rf_index() == 31);
}

TEST_CASE( "MIPS_registers: Hi_register")
{
    auto reg = MIPSRegister::mips_hi;
    CHECK_FALSE(reg.is_zero());
    CHECK(reg.is_mips_hi());
    CHECK_FALSE(reg.is_mips_lo());
    CHECK_FALSE(reg.to_rf_index() < 32);
}

TEST_CASE( "MIPS_registers: Lo_register")
{
    auto reg = MIPSRegister::mips_lo;
    CHECK_FALSE(reg.is_zero());
    CHECK_FALSE(reg.is_mips_hi());
    CHECK(reg.is_mips_lo());
    CHECK_FALSE(reg.to_rf_index() < 32);
}

