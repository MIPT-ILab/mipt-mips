/**
 * Unit tests for rf
 * @author Alexander Misevich
 * Copyright 2018 MIPT-MIPS
 */

#include <catch.hpp>
#include <func_sim/operation.h>
#include <func_sim/rf/rf.h>
#include <mips/mips_register/mips_register.h>

using MIPS32Instr = BaseInstruction<uint32, MIPSRegister>;

static_assert(MIPSRegister::MAX_REG >= 32);

static auto get_filled_rf()
{
    auto rf = std::make_unique<RF<MIPS32Instr>>();

    for( uint8 i = 0; i < 32; ++i)
    {
        rf->write( MIPSRegister::from_cpu_index(i), i);

        // Try to write something in zero register
        rf->write( MIPSRegister::zero(), i);
    }

    return rf;
}

TEST_CASE( "RF: read_write_rf")
{
    auto rf = get_filled_rf();

    CHECK( rf->read( MIPSRegister::zero()) == 0U);
    for ( uint8 i = 1; i < 32; ++i)
        CHECK( rf->read( MIPSRegister::from_cpu_index(i)) == i);
}

TEST_CASE( "RF mixed test")
{
    auto rf = get_filled_rf();

    for( uint8 i = 1; i < 32; ++i)
    {
        rf->write( MIPSRegister::from_cpu_index(i), 0U);
        rf->write( MIPSRegister::from_cpu_index(i), 0x12345678U, 0xFFU);
        CHECK( rf->read( MIPSRegister::from_cpu_index(i)) == 0x78U); // only one byte must go

        rf->write( MIPSRegister::from_cpu_index(i), 0U);
        rf->write( MIPSRegister::from_cpu_index(i), 0x12345678U, 0xFF00U);
        CHECK( rf->read( MIPSRegister::from_cpu_index(i)) == 0x5600U);

        rf->write( MIPSRegister::from_cpu_index(i), 0U);
        rf->write( MIPSRegister::from_cpu_index(i), 0x12345678U, 0xFFFFU);
        CHECK( rf->read( MIPSRegister::from_cpu_index(i)) == 0x5678U);

        rf->write( MIPSRegister::from_cpu_index(i), 0U);
        rf->write( MIPSRegister::from_cpu_index(i), 0x12345678U, 0xFFFF00U);
        CHECK( rf->read( MIPSRegister::from_cpu_index(i)) == 0x345600U);

        rf->write( MIPSRegister::from_cpu_index(i), 0U);
        rf->write( MIPSRegister::from_cpu_index(i), 0x12345678U, 0xFFFFFF00U);
        CHECK( rf->read( MIPSRegister::from_cpu_index(i)) == 0x12345600U);

        rf->write( MIPSRegister::from_cpu_index(i), 0U);
        rf->write( MIPSRegister::from_cpu_index(i), 0x12345678U, 0xFF0000U);
        CHECK( rf->read( MIPSRegister::from_cpu_index(i)) == 0x340000U);

        rf->write( MIPSRegister::from_cpu_index(i), 0x1U);
        rf->write( MIPSRegister::from_cpu_index(i), 0x12345678U, 0xFF0000U);
        CHECK( rf->read( MIPSRegister::from_cpu_index(i)) == 0x340001U);

        rf->write( MIPSRegister::from_cpu_index(i), 0x9876U);
        rf->write( MIPSRegister::from_cpu_index(i), 0x12345678U, 0xFF0000U);
        CHECK( rf->read( MIPSRegister::from_cpu_index(i)) == 0x349876U);

        rf->write( MIPSRegister::from_cpu_index(i), 0x5500U);
        rf->write( MIPSRegister::from_cpu_index(i), 0x12345678U, 0xFFU);
        CHECK( rf->read( MIPSRegister::from_cpu_index(i)) == 0x5578U);

        rf->write( MIPSRegister::from_cpu_index(i), 0x558700U);
        rf->write( MIPSRegister::from_cpu_index(i), 0x12345678U, 0xFF0000U);
        CHECK( rf->read( MIPSRegister::from_cpu_index(i)) == 0x348700U);
    }
}

TEST_CASE( "MIPS RF: hi/lo")
{
    auto rf = std::make_unique<RF<MIPS32Instr>>();

    rf->write( MIPSRegister::mips_hi(), 1U);
    rf->write( MIPSRegister::mips_lo(), 0U);

    CHECK( rf->read( MIPSRegister::mips_hi()) == 1U);
    CHECK( rf->read( MIPSRegister::mips_lo()) == 0U);
}

TEST_CASE( "MIPS RF: accumulating subtraction")
{
    auto rf = std::make_unique<RF<MIPS32Instr>>();
    rf->write( MIPSRegister::mips_hi(), 1U);
    rf->write( MIPSRegister::mips_lo(), 0U);

    rf->write( MIPSRegister::mips_hi(), 0U, all_ones<uint32>(), -1 /* subtract */);
    rf->write( MIPSRegister::mips_lo(), 1U, all_ones<uint32>(), -1 /* subtract */);
    CHECK( rf->read( MIPSRegister::mips_hi()) == 0U);
    CHECK( rf->read( MIPSRegister::mips_lo()) == MAX_VAL32);
}

TEST_CASE( "MIPS RF: accumulating addition")
{
    auto rf = std::make_unique<RF<MIPS32Instr>>();
    rf->write( MIPSRegister::mips_hi(), 0U);
    rf->write( MIPSRegister::mips_lo(), MAX_VAL32);

    rf->write( MIPSRegister::mips_hi(), 0U, all_ones<uint32>(), +1 /* add */);
    rf->write( MIPSRegister::mips_lo(), 1U, all_ones<uint32>(), +1 /* add */);
    CHECK( rf->read( MIPSRegister::mips_hi()) == 1U);
    CHECK( rf->read( MIPSRegister::mips_lo()) == 0U);
}
