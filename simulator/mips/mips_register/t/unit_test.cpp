/**
 * Unit tests for MIPS register
 * @author Alexander Misevich
 * Copyright 2018 MIPT-MIPS
 */

// generic C
#include <cassert>
#include <cstdlib>

// Google Test library
#include <gtest/gtest.h>

// MIPS-MIPS modules
#include "../mips_register.h"

#define GTEST_ASSERT_NO_DEATH(statement) \
    ASSERT_EXIT({{ statement } ::exit(EXIT_SUCCESS); }, ::testing::ExitedWithCode(0), "")

static_assert(MIPSRegister::MAX_REG >= 32);

TEST( MIPS_registers, Args_Of_Constr)
{
    // Call a constructor
    for ( size_t i = 0; i < 32; ++i)
    {
        GTEST_ASSERT_NO_DEATH( MIPSRegister reg( i); );
    }

    // Wrong parameter
    ASSERT_EXIT( MIPSRegister reg( 32), ::testing::ExitedWithCode( EXIT_FAILURE), "ERROR: Invalid MIPS register id*");
}

// Testing methods of the class
TEST( MIPS_registers, Size_t_converters)
{
    for ( size_t i = 0; i < 32; ++i)
    {
        ASSERT_EQ(MIPSRegister(i).to_size_t(), i);
    }
}

TEST( MIPS_registers, Equal)
{
    for ( size_t i = 0; i < 32; ++i)
    {
        ASSERT_EQ(MIPSRegister(i), MIPSRegister(i));
        if (i > 0) {
            ASSERT_NE(MIPSRegister(i - 1), MIPSRegister(i));
        }
    }
}

TEST( MIPS_registers, Hi_Lo_impossible)
{
    for ( size_t i = 0; i < 32; ++i)
    {
        MIPSRegister reg(i);
        ASSERT_FALSE(reg.is_mips_hi());
        ASSERT_FALSE(reg.is_mips_lo());
        ASSERT_FALSE(reg.is_mips_hi_lo());
        ASSERT_FALSE(reg.is_mips_acc());
    }
}

TEST( MIPS_registers, Zero)
{
    auto reg = MIPSRegister::zero;
    ASSERT_TRUE(reg.is_zero());
    ASSERT_FALSE(reg.is_mips_hi());
    ASSERT_FALSE(reg.is_mips_lo());
    ASSERT_FALSE(reg.is_mips_hi_lo());
    ASSERT_FALSE(reg.is_mips_acc());
    ASSERT_TRUE(reg.to_size_t() == 0);
}

TEST( MIPS_registers, Return_address)
{
    auto reg = MIPSRegister::return_address;
    ASSERT_FALSE(reg.is_zero());
    ASSERT_FALSE(reg.is_mips_hi());
    ASSERT_FALSE(reg.is_mips_lo());
    ASSERT_FALSE(reg.is_mips_hi_lo());
    ASSERT_FALSE(reg.is_mips_acc());
    ASSERT_TRUE(reg.to_size_t() == 31);
}

TEST( MIPS_registers, Hi_register)
{
    auto reg = MIPSRegister::mips_hi;
    ASSERT_FALSE(reg.is_zero());
    ASSERT_TRUE(reg.is_mips_hi());
    ASSERT_FALSE(reg.is_mips_lo());
    ASSERT_FALSE(reg.is_mips_hi_lo());
    ASSERT_FALSE(reg.is_mips_acc());
    ASSERT_FALSE(reg.to_size_t() < 32);
}

TEST( MIPS_registers, Lo_register)
{
    auto reg = MIPSRegister::mips_lo;
    ASSERT_FALSE(reg.is_zero());
    ASSERT_FALSE(reg.is_mips_hi());
    ASSERT_TRUE(reg.is_mips_lo());
    ASSERT_FALSE(reg.is_mips_hi_lo());
    ASSERT_FALSE(reg.is_mips_acc());
    ASSERT_FALSE(reg.to_size_t() < 32);
}

TEST( MIPS_registers, Hi_Lo_register)
{
    auto reg = MIPSRegister::mips_hi_lo;
    ASSERT_FALSE(reg.is_zero());
    ASSERT_FALSE(reg.is_mips_hi());
    ASSERT_FALSE(reg.is_mips_lo());
    ASSERT_TRUE(reg.is_mips_hi_lo());
    ASSERT_FALSE(reg.is_mips_acc());
    ASSERT_FALSE(reg.to_size_t() < 32);
}

TEST( MIPS_registers, Acc_register)
{
    auto reg = MIPSRegister::mips_hi_lo;
    ASSERT_FALSE(reg.is_zero());
    ASSERT_FALSE(reg.is_mips_hi());
    ASSERT_FALSE(reg.is_mips_lo());
    ASSERT_FALSE(reg.is_mips_hi_lo());
    ASSERT_TRUE(reg.is_mips_acc());
    ASSERT_FALSE(reg.to_size_t() < 32);
}

int main( int argc, char* argv[])
{
    ::testing::InitGoogleTest( &argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}
