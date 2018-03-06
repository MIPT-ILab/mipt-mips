// generic C
#include <cassert>
#include <cstdlib>

// Google Test library
#include <gtest/gtest.h>

// MIPS-MIPS modules
#include "../mips_register.h"

#define GTEST_ASSERT_NO_DEATH(statement) \
    ASSERT_EXIT({{ statement } ::exit(EXIT_SUCCESS); }, ::testing::ExitedWithCode(0), "")

TEST( MIPS_registers, Process_Correct_Args_Of_Constr)
{
    // Call a constructor
    GTEST_ASSERT_NO_DEATH( MIPSRegister reg(1); );
}

// Testing methods of the class
TEST( MIPS_registers, TEST_methods)
{
    for (size_t i = 0; i < MIPSRegister::MAX_REG; i++)
    {
        if(i == 0)
            ASSERT_TRUE(MIPSRegister(i).is_zero());
        else 
            ASSERT_FALSE(MIPSRegister(i).is_zero());
        if(i == 31)
            ASSERT_TRUE(MIPSRegister(i).is_mips_ra());
        else 
            ASSERT_FALSE(MIPSRegister(i).is_mips_ra());
        if(i < 32)
        {
            ASSERT_EQ(MIPSRegister(i).to_size_t(), i);
            ASSERT_FALSE(MIPSRegister(i).is_mips_hi());
            ASSERT_FALSE(MIPSRegister(i).is_mips_lo());
            ASSERT_FALSE(MIPSRegister(i).is_mips_hi_lo());
        }
    }
}

TEST( MIPS_Registers, Output_zero)
{
    std::ostringstream output;
    output << MIPSRegister::zero;
    ASSERT_EQ( output.str(), "zero");
}

TEST( MIPS_Registers, Output_hi)
{
    std::ostringstream output;
    output << MIPSRegister::mips_hi;
    ASSERT_EQ( output.str(), "hi");
}

TEST( MIPS_Registers, Output_lo)
{
    std::ostringstream output;
    output << MIPSRegister::mips_lo;
    ASSERT_EQ( output.str(), "lo");
}

TEST( MIPS_Registers, Output_hi_lo)
{
    std::ostringstream output;
    output << MIPSRegister::mips_hi_lo;
    ASSERT_EQ( output.str(), "hi_lo");
}

int main( int argc, char* argv[])
{
    ::testing::InitGoogleTest( &argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}

