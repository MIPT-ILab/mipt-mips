/**
 * Unit tests for rf
 * @author Alexander Misevich
 * Copyright 2018 MIPT-MIPS
 */

// generic C
#include <cassert>
#include <cstdlib>

// Google Test library
#include <gtest/gtest.h>

// MIPS-MIPS modules
#include <mips/mips.h>
#include "../rf.h"

#define GTEST_ASSERT_NO_DEATH(statement) \
    ASSERT_EXIT({{ statement } ::exit(EXIT_SUCCESS); }, ::testing::ExitedWithCode(0), "")

class TestRF : public RF<MIPS32>
{
    public:
        TestRF() : RF<MIPS32>() {};
        using RF<MIPS32>::read;
        using RF<MIPS32>::write;
};

static_assert(MIPSRegister::MAX_REG >= 32);

TEST( RF, read_write_rf)
{
    auto rf = std::make_unique<TestRF>();

    // Fill array using write() and check correctness using read()
    for( size_t i = 0; i < 32; ++i)
    {
        rf->write( MIPSRegister(i), i);

        // Try to write something in zero register
        rf->write( MIPSRegister::zero, i);

        // Checks
        ASSERT_EQ( rf->read( MIPSRegister(i)), i);
        ASSERT_EQ( rf->read( MIPSRegister::zero), 0u);
    }

    for( size_t i = 1; i < 32; ++i)
    {
        rf->write( MIPSRegister(i), 0u);
        rf->write( MIPSRegister(i), 0x12345678u, 0xFFu);
        ASSERT_EQ( rf->read( MIPSRegister(i)), 0x78u); // only one byte must go

        rf->write( MIPSRegister(i), 0u);
        rf->write( MIPSRegister(i), 0x12345678u, 0xFF00u);
        ASSERT_EQ( rf->read( MIPSRegister(i)), 0x5600u);

        rf->write( MIPSRegister(i), 0u);
        rf->write( MIPSRegister(i), 0x12345678u, 0xFFFFu);
        ASSERT_EQ( rf->read( MIPSRegister(i)), 0x5678u);

        rf->write( MIPSRegister(i), 0u);
        rf->write( MIPSRegister(i), 0x12345678u, 0xFFFF00u);
        ASSERT_EQ( rf->read( MIPSRegister(i)), 0x345600u);

        rf->write( MIPSRegister(i), 0u);
        rf->write( MIPSRegister(i), 0x12345678u, 0xFFFFFF00u);
        ASSERT_EQ( rf->read( MIPSRegister(i)), 0x12345600u);

        rf->write( MIPSRegister(i), 0u);
        rf->write( MIPSRegister(i), 0x12345678u, 0xFF0000u);
        ASSERT_EQ( rf->read( MIPSRegister(i)), 0x340000u);

        rf->write( MIPSRegister(i), 0x1u);
        rf->write( MIPSRegister(i), 0x12345678u, 0xFF0000u);
        ASSERT_EQ( rf->read( MIPSRegister(i)), 0x340001u);

        rf->write( MIPSRegister(i), 0x9876u);
        rf->write( MIPSRegister(i), 0x12345678u, 0xFF0000u);
        ASSERT_EQ( rf->read( MIPSRegister(i)), 0x349876u);

        rf->write( MIPSRegister(i), 0x5500u);
        rf->write( MIPSRegister(i), 0x12345678u, 0xFFu);
        ASSERT_EQ( rf->read( MIPSRegister(i)), 0x5578u);

        rf->write( MIPSRegister(i), 0x558700u);
        rf->write( MIPSRegister(i), 0x12345678u, 0xFF0000u);
        ASSERT_EQ( rf->read( MIPSRegister(i)), 0x348700u);
    }

    // Additional checks for mips_hi_lo
    // Write 1 to HI and 0 to LO
    rf->write( MIPSRegister::mips_hi, 1u);
    rf->write( MIPSRegister::mips_lo, 0u);

    ASSERT_EQ( rf->read( MIPSRegister::mips_hi), 1u);
    ASSERT_EQ( rf->read( MIPSRegister::mips_lo), 0u);

    // Check accumulating writes
    rf->write( MIPSRegister::mips_hi, 0u, all_ones<uint32>(), -1 /* subtract */);
    rf->write( MIPSRegister::mips_lo, 1u, all_ones<uint32>(), -1 /* subtract */);
    ASSERT_EQ( rf->read( MIPSRegister::mips_hi), 0u);
    ASSERT_EQ( rf->read( MIPSRegister::mips_lo), MAX_VAL32);

    // Check accumulating writes
    rf->write( MIPSRegister::mips_hi, 0u, all_ones<uint32>(), +1 /* add */);
    rf->write( MIPSRegister::mips_lo, 1u, all_ones<uint32>(), +1 /* add */);
    ASSERT_EQ( rf->read( MIPSRegister::mips_hi), 1u);
    ASSERT_EQ( rf->read( MIPSRegister::mips_lo), 0u);
}

TEST( RF, read_sources_write_dst_rf)
{
    auto rf = std::make_unique<TestRF>();

    // Fill Reg 25(it's src2) with some value
    rf->write( MIPSRegister(25), 1);

    // Create the instr( for example, "add")
    auto instr = std::make_unique<MIPS32Instr>( 0x01398820);
    // Use read_sources( "add") method( initialize src1 and src2)
    rf->read_sources( instr.get());
    // Execute instruction( to change v_dst field)
    instr->execute();
    // Check
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_NE( instr->get_v_dst(), NO_VAL64);

    // Same
    instr = std::make_unique<MIPS32Instr>( 0x01398821);
    rf->read_sources( instr.get());
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_NE( instr->get_v_dst(), NO_VAL64);
    instr = std::make_unique<MIPS32Instr>( 0x01398824);
    rf->read_sources( instr.get());
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_NE( instr->get_v_dst(), NO_VAL64);
    instr = std::make_unique<MIPS32Instr>( 0x0139880a);
    rf->read_sources( instr.get());
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_NE( instr->get_v_dst(), NO_VAL64);
    instr = std::make_unique<MIPS32Instr>( 0x0139880b);
    rf->read_sources( instr.get());
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_NE( instr->get_v_dst(), NO_VAL64);
    instr = std::make_unique<MIPS32Instr>( 0x71398802);
    rf->read_sources( instr.get());
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_NE( instr->get_v_dst(), NO_VAL64);
    instr = std::make_unique<MIPS32Instr>( 0x01398827);
    rf->read_sources( instr.get());
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_NE( instr->get_v_dst(), NO_VAL64);
    instr = std::make_unique<MIPS32Instr>( 0x01398825);
    rf->read_sources( instr.get());
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_NE( instr->get_v_dst(), NO_VAL64);
    instr = std::make_unique<MIPS32Instr>( 0x03298804);
    rf->read_sources( instr.get());
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_NE( instr->get_v_dst(), NO_VAL64);
    instr = std::make_unique<MIPS32Instr>( 0x03298806);
    rf->read_sources( instr.get());
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_NE( instr->get_v_dst(), NO_VAL64);
    instr = std::make_unique<MIPS32Instr>( 0x01398822);
    rf->read_sources( instr.get());
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_NE( instr->get_v_dst(), NO_VAL64);
    instr = std::make_unique<MIPS32Instr>( 0x01398823);
    rf->read_sources( instr.get());
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_NE( instr->get_v_dst(), NO_VAL64);
    instr = std::make_unique<MIPS32Instr>( 0x01398826);
    rf->read_sources( instr.get());
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_NE( instr->get_v_dst(), NO_VAL64);
    instr = std::make_unique<MIPS32Instr>( 0x0139882a);
    rf->read_sources( instr.get());
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_NE( instr->get_v_dst(), NO_VAL64);
    instr = std::make_unique<MIPS32Instr>( 0x0139882b);
    rf->read_sources( instr.get());
    instr->execute();
    ASSERT_EQ( instr->get_v_src2(), 1u);
    ASSERT_NE( instr->get_v_dst(), NO_VAL64);
}

