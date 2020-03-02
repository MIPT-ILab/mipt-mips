/**
 * trap_types.cpp - Trap type abstraction
 * @author Vsevolod Pukhov
 * Copyright 2019 MIPT-MIPS
 */

#include <catch.hpp>
#include <func_sim/traps/trap.h>
#include <riscv.opcode.gen.h>
#include <sstream>

TEST_CASE( "Trap: invalid trap")
{
    Trap trap( Trap::INVALID_TRAP);
    CHECK_THROWS_AS( trap.to_riscv_format(), InvalidTrapConversion);
    CHECK_THROWS_AS( trap.to_gdb_format(), InvalidTrapConversion);
    CHECK_THROWS_AS( trap.to_mips_format(), InvalidTrapConversion);
}

TEST_CASE( "Trap: check conversion")
{
    CHECK( Trap( Trap::SYSCALL).to_riscv_format() == CAUSE_USER_ECALL);
    CHECK( Trap( Trap::UNKNOWN_INSTRUCTION).to_gdb_format() == 4);
    CHECK( Trap( Trap::FP_DIV_BY_ZERO).to_mips_format() == 15);
    CHECK( Trap( Trap::NO_TRAP).to_mips_format() == 0);
}

TEST_CASE( "Trap: check initialization")
{
    CHECK( Trap::from_riscv_format( CAUSE_MISALIGNED_FETCH) == Trap::UNALIGNED_FETCH);
    CHECK( Trap::from_gdb_format( 12) == Trap::UNSUPPORTED_SYSCALL);
    CHECK( Trap::from_mips_format( 16) == Trap::FP_OVERFLOW);
}

TEST_CASE( "Trap: check bad initialization")
{
    CHECK_THROWS_AS( Trap::from_riscv_format( 255), InvalidTrapConversion);
    CHECK_THROWS_AS( Trap::from_gdb_format( 255), InvalidTrapConversion);
    CHECK_THROWS_AS( Trap::from_mips_format( 255), InvalidTrapConversion);
}

TEST_CASE( "Trap: print")
{
    Trap trap( Trap::UNKNOWN_INSTRUCTION);
    std::ostringstream oss;
    oss << trap;
    CHECK( oss.str() == "UNKNOWN_INSTRUCTION");
}
