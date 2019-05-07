/**
 * trap_types.cpp - Trap type abstraction
 * @author Vsevolod Pukhov
 * Copyright 2019 MIPT-MIPS
 */

#include "../trap.h"
#include <catch.hpp>
#include <riscv.opcode.gen.h>
#include <sstream>

TEST_CASE( "Trap: check conversion to RISC-V ")
{
    Trap trap( Trap::SYSCALL);
    CHECK( trap.to_riscv_format() == CAUSE_USER_ECALL);
}

TEST_CASE( "Trap: check conversion to GDB ")
{
    Trap trap( Trap::UNKNOWN_INSTRUCTION);
    CHECK( trap.to_gdb_format() == 4);
}

TEST_CASE( "Trap: check conversion to MIPS ")
{
    Trap trap( Trap::FP_DIV_BY_ZERO);
    CHECK( trap.to_mips_format() == 15);
}

TEST_CASE( "Trap: check bad conversion to MIPS ")
{
    Trap trap( Trap::NO_TRAP);
    CHECK_THROWS_AS( trap.to_mips_format(), std::out_of_range);
}

TEST_CASE( "Trap: check RISC-V initialization")
{
    Trap trap( Trap::NO_TRAP);
    trap.set_from_riscv_format( CAUSE_MISALIGNED_FETCH);
    CHECK( trap == Trap( Trap::UNALIGNED_FETCH));
}

TEST_CASE( "Trap: check GDB initialization")
{
    Trap trap( Trap::NO_TRAP);
    trap.set_from_gdb_format( 5);
    CHECK( trap == Trap( Trap::BREAKPOINT));
}

TEST_CASE( "Trap: check MIPS initialization")
{
    Trap trap( Trap::NO_TRAP);
    trap.set_from_mips_format( 16);
    CHECK( trap == Trap( Trap::FP_OVERFLOW));
}

TEST_CASE( "Trap: print")
{
    Trap trap( Trap::UNKNOWN_INSTRUCTION);
    std::ostringstream oss;
    oss << trap;
    CHECK( oss.str() == "UNKNOWN_INSTRUCTION");
}
