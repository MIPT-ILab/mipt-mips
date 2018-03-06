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
    for (size_t i = 0; i < MIPSRegister::MAX_REG; ++i)
    {
        ASSERT_EQ(MIPSRegister(i).to_size_t(), i);
        if(i != 0)
            ASSERT_FALSE(MIPSRegister(i).is_zero());
        else 
            ASSERT_TRUE(MIPSRegister(i).is_zero());
	    if(i != 32)
            ASSERT_FALSE(MIPSRegister(i).is_mips_hi());
        else 
            ASSERT_TRUE(MIPSRegister(i).is_mips_hi());
	    if(i != 33)
            ASSERT_FALSE(MIPSRegister(i).is_mips_lo());
        else 
            ASSERT_TRUE(MIPSRegister(i).is_mips_lo());
	    if(i != 34)
            ASSERT_FALSE(MIPSRegister(i).is_mips_hi_lo());
        else 
            ASSERT_TRUE(MIPSRegister(i).is_mips_hi_lo());
    }
}

int main( int argc, char* argv[])
{
    ::testing::InitGoogleTest( &argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}

