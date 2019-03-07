/**
 * Unit tests for rf
 * @author Alexander Misevich
 * Copyright 2018 MIPT-MIPS
 */

#include "../rf.h"

#include <catch.hpp>
#include <mips/mips_register/mips_register.h>
#include <func_sim/operation.h>

using MIPS32Instr = BaseInstruction<uint32, MIPSRegister>;

static_assert(MIPSRegister::MAX_REG >= 32);

TEST_CASE( "RF: read_write_rf")
{
    auto rf = std::make_unique<RF<MIPS32Instr>>();

    // Fill array using write() and check correctness using read()
    for( uint8 i = 0; i < 32; ++i)
    {
        rf->write( MIPSRegister::from_cpu_index(i), i);

        // Try to write something in zero register
        rf->write( MIPSRegister::zero(), i);

        // Checks
        CHECK( rf->read( MIPSRegister::from_cpu_index(i)) == i);
        CHECK( rf->read( MIPSRegister::zero()) == 0u);
    }

    for( uint8 i = 1; i < 32; ++i)
    {
        rf->write( MIPSRegister::from_cpu_index(i), 0u);
        rf->write( MIPSRegister::from_cpu_index(i), 0x12345678u, 0xFFu);
        CHECK( rf->read( MIPSRegister::from_cpu_index(i)) == 0x78u); // only one byte must go

        rf->write( MIPSRegister::from_cpu_index(i), 0u);
        rf->write( MIPSRegister::from_cpu_index(i), 0x12345678u, 0xFF00u);
        CHECK( rf->read( MIPSRegister::from_cpu_index(i)) == 0x5600u);

        rf->write( MIPSRegister::from_cpu_index(i), 0u);
        rf->write( MIPSRegister::from_cpu_index(i), 0x12345678u, 0xFFFFu);
        CHECK( rf->read( MIPSRegister::from_cpu_index(i)) == 0x5678u);

        rf->write( MIPSRegister::from_cpu_index(i), 0u);
        rf->write( MIPSRegister::from_cpu_index(i), 0x12345678u, 0xFFFF00u);
        CHECK( rf->read( MIPSRegister::from_cpu_index(i)) == 0x345600u);

        rf->write( MIPSRegister::from_cpu_index(i), 0u);
        rf->write( MIPSRegister::from_cpu_index(i), 0x12345678u, 0xFFFFFF00u);
        CHECK( rf->read( MIPSRegister::from_cpu_index(i)) == 0x12345600u);

        rf->write( MIPSRegister::from_cpu_index(i), 0u);
        rf->write( MIPSRegister::from_cpu_index(i), 0x12345678u, 0xFF0000u);
        CHECK( rf->read( MIPSRegister::from_cpu_index(i)) == 0x340000u);

        rf->write( MIPSRegister::from_cpu_index(i), 0x1u);
        rf->write( MIPSRegister::from_cpu_index(i), 0x12345678u, 0xFF0000u);
        CHECK( rf->read( MIPSRegister::from_cpu_index(i)) == 0x340001u);

        rf->write( MIPSRegister::from_cpu_index(i), 0x9876u);
        rf->write( MIPSRegister::from_cpu_index(i), 0x12345678u, 0xFF0000u);
        CHECK( rf->read( MIPSRegister::from_cpu_index(i)) == 0x349876u);

        rf->write( MIPSRegister::from_cpu_index(i), 0x5500u);
        rf->write( MIPSRegister::from_cpu_index(i), 0x12345678u, 0xFFu);
        CHECK( rf->read( MIPSRegister::from_cpu_index(i)) == 0x5578u);

        rf->write( MIPSRegister::from_cpu_index(i), 0x558700u);
        rf->write( MIPSRegister::from_cpu_index(i), 0x12345678u, 0xFF0000u);
        CHECK( rf->read( MIPSRegister::from_cpu_index(i)) == 0x348700u);
    }

    // Additional checks for mips_hi_lo
    // Write 1 to HI and 0 to LO
    rf->write( MIPSRegister::mips_hi(), 1u);
    rf->write( MIPSRegister::mips_lo(), 0u);

    CHECK( rf->read( MIPSRegister::mips_hi()) == 1u);
    CHECK( rf->read( MIPSRegister::mips_lo()) == 0u);

    // Check accumulating writes
    rf->write( MIPSRegister::mips_hi(), 0u, all_ones<uint32>(), -1 /* subtract */);
    rf->write( MIPSRegister::mips_lo(), 1u, all_ones<uint32>(), -1 /* subtract */);
    CHECK( rf->read( MIPSRegister::mips_hi()) == 0u);
    CHECK( rf->read( MIPSRegister::mips_lo()) == MAX_VAL32);

    // Check accumulating writes
    rf->write( MIPSRegister::mips_hi(), 0u, all_ones<uint32>(), +1 /* add */);
    rf->write( MIPSRegister::mips_lo(), 1u, all_ones<uint32>(), +1 /* add */);
    CHECK( rf->read( MIPSRegister::mips_hi()) == 1u);
    CHECK( rf->read( MIPSRegister::mips_lo()) == 0u);
}

