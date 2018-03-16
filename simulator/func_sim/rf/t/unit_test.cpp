/**
 * Unit tests for rf
 * @author Alexander Misevich
 * Copyright 2018 MIPT-MIPS
 */

// generic C
#include <cassert>
#include <cstdlib>
#include <cmath>

// Google Test library
#include <gtest/gtest.h>

// MIPS-MIPS modules
#include <mips/mips.h>
#include "../rf.h"

#define GTEST_ASSERT_NO_DEATH(statement) \
    ASSERT_EXIT({{ statement } ::exit(EXIT_SUCCESS); }, ::testing::ExitedWithCode(0), "")

static_assert(MIPSRegister::MAX_REG >= 32);

// Testing methods of the class
TEST( Rf, empty_rf)
{
    // Create array of 35 Regs
    ASSERT_FALSE( RF<MIPS>().array.empty());    
}

TEST( RF, read_write_rf)
{
    auto rf = std::make_unique<RF<MIPS>>();
    
    // Fill array using write() and check correctness using read()
    for( size_t i = 0; i < 32; ++i)
    {   
        rf->write( MIPSRegister(i), i);
        
        // Try to write something in zero register
        rf->write( MIPSRegister::zero, i);
        
        // Checks
        ASSERT_EQ( rf->read( MIPSRegister(i)), i);
        ASSERT_FALSE( rf->read( MIPSRegister::zero));
    }

    // Additional checks for mips_hi_lo 
    rf->write( MIPSRegister::mips_hi_lo, pow(2, 32));
    ASSERT_EQ( rf->read( MIPSRegister::mips_hi), 1u);
    ASSERT_FALSE( rf->read( MIPSRegister::mips_lo));
}

TEST( RF, read_sources_write_dst_rf)
{
    auto rf = std::make_unique<RF<MIPS>>();

    // Fill Reg 25(it's src2) with some value
    rf->write( MIPSRegister(25), 1);

    // Create the instr( for example, "add")
    auto instr = new MIPSInstr( 0x01398820);
    // Use read_sources( "add") method( initialize src1 and src2)
    rf->read_sources( instr);
    // Execute instruction( to change v_dst field)
    instr->execute();
    // Check
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_FALSE( instr->get_v_dst() == NO_VAL64);

    // Same 
    instr = new MIPSInstr( 0x01398821);
    rf->read_sources( instr);
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_FALSE( instr->get_v_dst() == NO_VAL64);
    instr = new MIPSInstr( 0x01398824);
    rf->read_sources( instr);
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_FALSE( instr->get_v_dst() == NO_VAL64);
    instr = new MIPSInstr( 0x0139880a);
    rf->read_sources( instr);
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_FALSE( instr->get_v_dst() == NO_VAL64);
    instr = new MIPSInstr( 0x0139880b);
    rf->read_sources( instr);
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_FALSE( instr->get_v_dst() == NO_VAL64);
    instr = new MIPSInstr( 0x71398802);
    rf->read_sources( instr);
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_FALSE( instr->get_v_dst() == NO_VAL64);
    instr = new MIPSInstr( 0x01398827);
    rf->read_sources( instr);
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_FALSE( instr->get_v_dst() == NO_VAL64);
    instr = new MIPSInstr( 0x01398825);
    rf->read_sources( instr);
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_FALSE( instr->get_v_dst() == NO_VAL64);
    instr = new MIPSInstr( 0x03298804);
    rf->read_sources( instr);
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_FALSE( instr->get_v_dst() == NO_VAL64);
    instr = new MIPSInstr( 0x03298806);
    rf->read_sources( instr);
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_FALSE( instr->get_v_dst() == NO_VAL64);
    instr = new MIPSInstr( 0x01398822);
    rf->read_sources( instr);
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_FALSE( instr->get_v_dst() == NO_VAL64);
    instr = new MIPSInstr( 0x01398823);
    rf->read_sources( instr);
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_FALSE( instr->get_v_dst() == NO_VAL64);
    instr = new MIPSInstr( 0x01398826);
    rf->read_sources( instr);
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_FALSE( instr->get_v_dst() == NO_VAL64);
    instr = new MIPSInstr( 0x0139882a);
    rf->read_sources( instr);
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_FALSE( instr->get_v_dst() == NO_VAL64);
    instr = new MIPSInstr( 0x0139882b);
    rf->read_sources( instr);
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_FALSE( instr->get_v_dst() == NO_VAL64);
}
    
int main( int argc, char* argv[])
{
    ::testing::InitGoogleTest( &argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}

