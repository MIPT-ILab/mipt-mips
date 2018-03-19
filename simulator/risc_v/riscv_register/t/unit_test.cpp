/**
 * Unit tests for RISCV register
 * @author Alexander Misevich
 * Copyright 2018 MIPT-MIPS
 */

// generic C
#include <cassert>
#include <cstdlib>

// Google Test library
#include <gtest/gtest.h>

// MIPT-MIPS modules
#include "../riscv_register.h"

#define GTEST_ASSERT_NO_DEATH(statement) \
    ASSERT_EXIT({{ statement } ::exit(EXIT_SUCCESS); }, ::testing::ExitedWithCode(0), "")

static_assert(RISCVRegister::MAX_REG == 32);

TEST( RISCV_registers, Args_Of_Constr)
{
    // Call a constructor
    for ( size_t i = 0; i < 32; ++i)
    {
        GTEST_ASSERT_NO_DEATH( RISCVRegister reg( i); );
    }

    // Wrong parameter
    ASSERT_EXIT( RISCVRegister reg( 32), ::testing::ExitedWithCode( EXIT_FAILURE), "ERROR: Invalid RISCV register id*");
}

// Testing methods of the class
TEST( RISCV_registers, Size_t_converters)
{
    for ( size_t i = 0; i < 32; ++i)
    {
        ASSERT_EQ( RISCVRegister(i).to_size_t(), i);
    }
}

TEST( RISCV_registers, Equal)
{
    for ( size_t i = 0; i < 32; ++i)
    {
        ASSERT_EQ( RISCVRegister(i), RISCVRegister(i));
        if (i > 0) {
            ASSERT_NE(RISCVRegister(i - 1), RISCVRegister(i));
        }
    }
}

TEST( RISCV_registers, no_mips)
{
    auto reg_hi = RISCVRegister::mips_hi;
    auto reg_lo = RISCVRegister::mips_lo;
    auto reg_hi_lo = RISCVRegister::mips_hi_lo;
    for( size_t i = 0; i < 32; ++i)
	{
        // Ensure that there no mips regs
        ASSERT_NE( RISCVRegister(i).to_size_t(), reg_hi.to_size_t());
        ASSERT_NE( RISCVRegister(i).to_size_t(), reg_lo.to_size_t());
        ASSERT_NE( RISCVRegister(i).to_size_t(), reg_hi_lo.to_size_t());
        ASSERT_FALSE( RISCVRegister(i).is_mips_hi());
        ASSERT_FALSE( RISCVRegister(i).is_mips_lo());
        ASSERT_FALSE( RISCVRegister(i).is_mips_hi_lo());
	}
}

TEST( RISCV_registers, return_address)
{
    auto reg = RISCVRegister::return_address;
    ASSERT_EQ( reg.to_size_t(), 1u);
    ASSERT_FALSE( reg.is_zero());
    ASSERT_FALSE( reg.is_mips_hi());
    ASSERT_FALSE( reg.is_mips_lo());
    ASSERT_FALSE( reg.is_mips_hi_lo());
}

TEST( RISCV_registers, Zero)
{
    auto reg = RISCVRegister::zero;
    ASSERT_TRUE( reg.is_zero());
    ASSERT_FALSE( reg.is_mips_hi());
    ASSERT_FALSE( reg.is_mips_lo());
    ASSERT_FALSE( reg.is_mips_hi_lo());
}

int main( int argc, char* argv[])
{
    ::testing::InitGoogleTest( &argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}

