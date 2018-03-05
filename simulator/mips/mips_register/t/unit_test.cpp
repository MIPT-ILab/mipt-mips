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
    GTEST_ASSERT_NO_DEATH( MIPSRegister reg( 34); );
}

// Testing methods of the class
TEST( MIPS_registers, TEST_methods)
{
    ASSERT_TRUE(MIPSRegister(0).is_zero());
    ASSERT_FALSE(MIPSRegister(0).is_mips_hi());
    ASSERT_FALSE(MIPSRegister(0).is_mips_lo());
    ASSERT_FALSE(MIPSRegister(0).is_mips_hi_lo());
    ASSERT_EQ(MIPSRegister(0).to_size_t(), 0);
    ASSERT_FALSE(MIPSRegister(32).is_zero());
    ASSERT_TRUE(MIPSRegister(32).is_mips_hi());
    ASSERT_FALSE(MIPSRegister(32).is_mips_lo());
    ASSERT_FALSE(MIPSRegister(32).is_mips_hi_lo());
    ASSERT_EQ(MIPSRegister(32).to_size_t(), 32);
    ASSERT_FALSE(MIPSRegister(33).is_zero());
    ASSERT_FALSE(MIPSRegister(33).is_mips_hi());
    ASSERT_TRUE(MIPSRegister(33).is_mips_lo());
    ASSERT_FALSE(MIPSRegister(33).is_mips_hi_lo());
    ASSERT_EQ(MIPSRegister(33).to_size_t(), 33);
    ASSERT_FALSE(MIPSRegister(34).is_zero());
    ASSERT_FALSE(MIPSRegister(34).is_mips_hi());
    ASSERT_FALSE(MIPSRegister(34).is_mips_lo());
    ASSERT_TRUE(MIPSRegister(34).is_mips_hi_lo());
    ASSERT_EQ(MIPSRegister(34).to_size_t(), 34);
}


int main( int argc, char* argv[])
{
    ::testing::InitGoogleTest( &argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}

