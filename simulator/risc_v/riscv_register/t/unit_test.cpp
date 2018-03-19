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

TEST( RISCV_registers, no_mips_hi)
{
	auto reg = RISCVRegister::mips_hi;
	for( size_t i = 0; i < 32; ++i)
	{
		ASSERT_NE( RISCVRegister(i).to_size_t(), reg.to_size_t());
	}
}

TEST( RISCV_registers, no_mips_lo)
{
	auto reg = RISCVRegister::mips_lo;
	for( size_t i = 0; i < 32; ++i)
	{
		ASSERT_NE( RISCVRegister(i).to_size_t(), reg.to_size_t());
	}
}

TEST( RISCV_registers, no_mips_hi_lo)
{
	auto reg = RISCVRegister::mips_hi_lo;
	for( size_t i = 0; i < 32; ++i)
	{
		ASSERT_NE( RISCVRegister(i).to_size_t(), reg.to_size_t());
	}
}

int main( int argc, char* argv[])
{
    ::testing::InitGoogleTest( &argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}

