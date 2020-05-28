/* MIPS Instruction unit tests
 * @author: Pavel Kryukov, Vsevolod Pukhov, Egor Bova
 * Copyright (C) MIPT-MIPS 2017-2019
 */

#include "mips32_test.h"

#include <catch.hpp>

TEST_CASE( "MIPS32_instr: div 1 by 1")
{
    CHECK(MIPS32Instr(0x0229001a).get_disasm() == "div $s1, $t1");
    CHECK(MIPS32Instr(0x0229001a).is_divmult());
    CHECK(MIPS32Instr("div").is_divmult());

    MIPS32Instr instr( "div");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 1);
}

TEST_CASE( "MIPS32_instr: div -1 by 1")
{
    MIPS32Instr instr( "div");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 0xffffffff);
}

TEST_CASE( "MIPS32_instr: div -1 by -1")
{
    MIPS32Instr instr( "div");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 1);
}

TEST_CASE( "MIPS32_instr: div 1 by -1")
{
    MIPS32Instr instr( "div");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 0xffffffff);
}

TEST_CASE( "MIPS32_instr: div 0 by 1")
{
    MIPS32Instr instr( "div");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 0);
}

TEST_CASE( "MIPS32_instr: div 1 by 0")
{
    MIPS32Instr instr( "div");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 0);
}

TEST_CASE( "MIPS32_instr: div 0x80000000 by -1")
{
    MIPS32Instr instr( "div");
    instr.set_v_src( 0x80000000, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 0);
}

TEST_CASE( "MIPS32_instr: div 0x4c4b4000 by 0x1dcd6500")
{
    MIPS32Instr instr( "div");
    instr.set_v_src( 0x4c4b4000, 0);
    instr.set_v_src( 0x1dcd6500, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0x10b07600);
    CHECK( instr.get_v_dst( 0)  == 2);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: divu 1 by 1")
{
    CHECK(MIPS32Instr(0x0229001b).get_disasm() == "divu $s1, $t1");
    CHECK(MIPS32Instr(0x0229001b).is_divmult());
    CHECK(MIPS32Instr("divu").is_divmult());

    MIPS32Instr instr( "divu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 1);
}

TEST_CASE( "MIPS32_instr: divu -1 by 1")
{
    MIPS32Instr instr( "divu");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 0xffffffff);
}

TEST_CASE( "MIPS32_instr: divu -1 by -1")
{
    MIPS32Instr instr( "divu");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 1);
}

TEST_CASE( "MIPS32_instr: divu 1 by -1")
{
    MIPS32Instr instr( "divu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 1);
    CHECK( instr.get_v_dst( 0)  == 0);
}

TEST_CASE( "MIPS32_instr: divu 0 by 1")
{
    MIPS32Instr instr( "divu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 0);
}

TEST_CASE( "MIPS32_instr: divu 1 by 0")
{
    MIPS32Instr instr( "divu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 0);
}

TEST_CASE( "MIPS32_instr: divu 0x80000000 by -1")
{
    MIPS32Instr instr( "divu");
    instr.set_v_src( 0x80000000, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0x80000000);
    CHECK( instr.get_v_dst( 0)  == 0);
}

TEST_CASE( "MIPS32_instr: divu 0x4c4b4000 by 0x1dcd6500")
{
    MIPS32Instr instr( "divu");
    instr.set_v_src( 0x4c4b4000, 0);
    instr.set_v_src( 0x1dcd6500, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0x10b07600);
    CHECK( instr.get_v_dst( 0)  == 2);
}

////////////////////////////////////////////////////////////////////////////////
static void mult_check_0_by_0( std::string_view name)
{
    MIPS32Instr instr( name);
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 0);
}

static void mult_check_1_by_1( std::string_view name)
{
    MIPS32Instr instr( name);
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 1);
}

static void mult_check_minus1_by_minus1( std::string_view name)
{
    MIPS32Instr instr( name);
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 1);
}

static void mult_check_minus1_by_1( std::string_view name)
{
    MIPS32Instr instr( name);
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0xffffffff);
    CHECK( instr.get_v_dst( 0)  == 0xffffffff);
}

static void mult_check_10000_by_10000( std::string_view name)
{
    MIPS32Instr instr( name);
    instr.set_v_src( 0x10000, 0);
    instr.set_v_src( 0x10000, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 1);
    CHECK( instr.get_v_dst( 0)  == 0);
}

static void mult_check_min_int_by_min_int( std::string_view name)
{
    MIPS32Instr instr( name);
    instr.set_v_src( 0x80000000, 0);
    instr.set_v_src( 0x80000000, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0x40000000);
    CHECK( instr.get_v_dst( 0)  == 0);
}

static void mult_unsigned_check_minus1_by_minus1( std::string_view name)
{
    MIPS32Instr instr( name);
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0xfffffffe);
    CHECK( instr.get_v_dst( 0)  == 1);
}

static void mult_unsigned_check_minus1_by_1( std::string_view name)
{
    MIPS32Instr instr( name);
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 0xffffffff);
}

static void mult_unsigned_check_min_int_by_min_int( std::string_view name)
{
    MIPS32Instr instr( name);
    instr.set_v_src( 0x80000000, 0);
    instr.set_v_src( 0x80000000, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0x40000000);
    CHECK( instr.get_v_dst( 0)  == 0);
}

static void mult_unsigned_check_random( std::string_view name)
{
    MIPS32Instr instr( name);
    instr.set_v_src( 0xcecb8f27, 0);
    instr.set_v_src( 0xfd87b5f2, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0xcccccccb);
    CHECK( instr.get_v_dst( 0)  == 0x7134e5de);
}

static void mult_signed_tests( std::string_view name)
{
    mult_check_0_by_0( name);
    mult_check_1_by_1( name);
    mult_check_minus1_by_minus1( name);
    mult_check_minus1_by_1( name);
    mult_check_10000_by_10000( name);
    mult_check_min_int_by_min_int( name);
}

static void mult_unsigned_tests( std::string_view name)
{
    mult_check_0_by_0( name);
    mult_check_1_by_1( name);
    mult_unsigned_check_minus1_by_minus1( name);
    mult_unsigned_check_minus1_by_1( name);
    mult_check_10000_by_10000( name);
    mult_unsigned_check_min_int_by_min_int( name);
    mult_unsigned_check_random( name);
}

TEST_CASE( "MIPS32_instr: madd")
{
    CHECK(MIPS32Instr(0x72290000).get_disasm() == "madd $s1, $t1");
    CHECK(MIPS32Instr(0x72290000).is_divmult());
    CHECK(MIPS32Instr("madd").is_divmult());
    CHECK(MIPS32Instr("madd").get_accumulation_type() == 1);
    mult_signed_tests( "madd");
}

TEST_CASE( "MIPS32_instr: maddu")
{
    CHECK(MIPS32Instr(0x72290001).get_disasm() == "maddu $s1, $t1");
    CHECK(MIPS32Instr(0x72290001).is_divmult());
    CHECK(MIPS32Instr("maddu").is_divmult());
    CHECK(MIPS32Instr("maddu").get_accumulation_type() == 1);
    mult_unsigned_tests( "maddu");
}

TEST_CASE( "MIPS32_instr: msub")
{
    CHECK(MIPS32Instr(0x72290004).get_disasm() == "msub $s1, $t1");
    CHECK(MIPS32Instr(0x72290004).is_divmult());
    CHECK(MIPS32Instr( "msub").is_divmult());
    CHECK(MIPS32Instr( "msub").get_accumulation_type() == -1);
    mult_signed_tests( "msub");
}

TEST_CASE( "MIPS32_instr: msubu")
{
    CHECK(MIPS32Instr(0x72290005).get_disasm() == "msubu $s1, $t1");
    CHECK(MIPS32Instr(0x72290005).is_divmult());
    CHECK(MIPS32Instr( "msubu").is_divmult());
    CHECK(MIPS32Instr( "msubu").get_accumulation_type() == -1);
    mult_unsigned_tests( "msubu");
}

TEST_CASE( "MIPS32_instr: mul 1 by 0")
{
    CHECK(MIPS32Instr(0x71398802).get_disasm() == "mul $s1, $t1, $t9");
    
    MIPS32Instr instr( "mul");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: mul 1 by 1")
{
    MIPS32Instr instr( "mul");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS32_instr: mul 1 by 10")
{
    MIPS32Instr instr( "mul");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 10, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 10);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: mult")
{
    CHECK(MIPS32Instr( 0x02290018).get_disasm() == "mult $s1, $t1");
    CHECK(MIPS32Instr( 0x02290018).is_divmult());
    CHECK(MIPS32Instr( "mult").is_divmult());
    CHECK(MIPS32Instr( "mult").get_accumulation_type() == 0);
    mult_signed_tests( "mult");
}

TEST_CASE( "MIPS32_instr: multu")
{
    CHECK(MIPS32Instr( 0x02290019).get_disasm() == "multu $s1, $t1");
    CHECK(MIPS32Instr( 0x02290019).is_divmult());
    CHECK(MIPS32Instr( "multu").is_divmult());
    CHECK(MIPS32Instr( "multu").get_accumulation_type() == 0);
    mult_unsigned_tests( "multu");
}

TEST_CASE( "MIPS32_instr: mult dump")
{
    MIPS32Instr instr(0x02290018);

    instr.set_v_src( 0x80000000, 0);
    instr.set_v_src( 0x80000000, 1);
    instr.set_sequence_id( 0);
    instr.execute();
    CHECK( instr.string_dump() == "{0}\tmult $s1, $t1\t [ $lo = 0x0, $hi = 0x40000000 ]" );
}
