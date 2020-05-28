/* MIPS Instruction unit tests
 * @author: Pavel Kryukov, Vsevolod Pukhov, Egor Bova
 * Copyright (C) MIPT-MIPS 2017-2019
 */

#include "mips32_test.h"

#include <catch.hpp>

TEST_CASE( "MIPS32_instr_init: Process_Wrong_Args_Of_Constr")
{
    CHECK_NOTHROW( MIPS32Instr(0x0).execute());
}

TEST_CASE( "MIPS32_instr: invalid instruction in ctor from string")
{
    MIPS32Instr instr( "invalid_instruction_12345678");
    instr.execute();
    CHECK( instr.trap_type() == Trap::UNKNOWN_INSTRUCTION );
}

TEST_CASE( "MIPS32_instr: MIPS64_instr in MIPS32_instr ctor")
{
    MIPS32Instr instr( "dsllv");
    instr.execute();
    CHECK( instr.trap_type() == Trap::UNKNOWN_INSTRUCTION );
}

TEST_CASE( "MIPS32_disasm BE-LE")
{
    CHECK(MIPS32Instr(0x0139882C).get_disasm() == MIPS32BEInstr(0x0139882C).get_disasm());
}

TEST_CASE("Sequence id print")
{
    MIPS32Instr instr( 0xb531fb2e);
    instr.set_sequence_id( 50);
    std::ostringstream oss;
    oss << std::hex << instr;
    CHECK( oss.str() == "{50}	sdr $s1, 0xfb2e($t1)	 [ ]" );
}

// ********* Converted SPIM TT tests with some additions **********
// 55 done

////////////////////////////////////////////////////////////////////////////////

TEST_CASE ( "MIPS32_instr: add two zeroes")
{
    CHECK(MIPS32Instr(0x01398820).get_disasm() == "add $s1, $t1, $t9");
    
    MIPS32Instr instr( "add");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE ( "MIPS32_instr: add 0 and 1")
{
    MIPS32Instr instr( "add");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE ( "MIPS32_instr: add 1 and -1")
{
    MIPS32Instr instr( "add");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}


TEST_CASE ( "MIPS32_instr: add overflow")
{
    MIPS32Instr instr( "add");
    instr.set_v_src( 0x7fffffff, 0);
    instr.set_v_src( 0x7fffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == NO_VAL32);
    CHECK( instr.trap_type() != Trap::NO_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: addi two zeroes")
{
    CHECK(MIPS32Instr(0x213104d2).get_disasm() == "addi $s1, $t1, 1234");
    CHECK(MIPS32Instr(0x2131fb2e).get_disasm() == "addi $s1, $t1, -1234");
    
    MIPS32Instr instr( "addi");
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: addi 0 and 1")
{
    MIPS32Instr instr( "addi", 1);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS32_instr: addi 1 and -1")
{
    MIPS32Instr instr( "addi", 0xffff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: addi overflow")
{
    MIPS32Instr instr( "addi", 1);
    instr.set_v_src( 0x7fffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == NO_VAL32);
    CHECK( instr.trap_type() != Trap::NO_TRAP);
}

////////////////////////////////////////////////////////////////////////////////
    
TEST_CASE( "MIPS32_instr: addiu two zeroes")
{
    CHECK(MIPS32Instr(0x253104d2).get_disasm() == "addiu $s1, $t1, 1234");
    CHECK(MIPS32Instr(0x2531fb2e).get_disasm() == "addiu $s1, $t1, -1234");
    
    MIPS32Instr instr( "addiu");
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: addiu 0 and 1")
{
    MIPS32Instr instr( "addiu", 1);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS32_instr: addiu 1 and -1")
{
    MIPS32Instr instr( "addiu", 0xffff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: addiu overflow")
{
    MIPS32Instr instr( "addiu", 2);
    instr.set_v_src( 0x7fffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x80000001);
    CHECK( instr.has_trap() == false);
}

////////////////////////////////////////////////////////////////////////////////

TEST_CASE ( "MIPS32_instr: addu two zeroes")
{
    CHECK(MIPS32Instr(0x01398821).get_disasm() == "addu $s1, $t1, $t9");
    
    MIPS32Instr instr( "addu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE ( "MIPS32_instr: addu 0 and 1")
{
    MIPS32Instr instr( "addu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE ( "MIPS32_instr: addu 1 and -1")
{
    MIPS32Instr instr( "addu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE ( "MIPS32_instr: addu overflow")
{
    MIPS32Instr instr( "addu");
    instr.set_v_src( 0x7fffffff, 0);
    instr.set_v_src( 0x7fffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xfffffffe);
    CHECK( instr.has_trap() == false);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE ( "MIPS32_instr: and two zeroes")
{
    CHECK(MIPS32Instr(0x01398824).get_disasm() == "and $s1, $t1, $t9");
    
    MIPS32Instr instr( "and");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE ( "MIPS32_instr: and 1 and 1")
{
    MIPS32Instr instr( "and");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE ( "MIPS32_instr: and 1 and -1")
{
    MIPS32Instr instr( "and");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE ( "MIPS32_instr: andi two zeroes")
{
    CHECK(MIPS32Instr(0x313104d2).get_disasm() == "andi $s1, $t1, 0x4d2");
    
    MIPS32Instr instr( "andi");
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE ( "MIPS32_instr: andi 1 and 1")
{
    MIPS32Instr instr( "andi", 1);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE ( "MIPS32_instr: andi 1 and -1")
{
    MIPS32Instr instr( "andi", 0xffff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE ( "MIPS32_instr: andi -1 and -1")
{
    MIPS32Instr instr( "andi", 0xffff);
    instr.set_v_src( 0xffffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x0000ffff);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: clo zero")
{
    CHECK(MIPS32Instr(0x71208821).get_disasm() == "clo $s1, $t1");
    
    MIPS32Instr instr( "clo");
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: clo -1")
{
    MIPS32Instr instr( "clo");
    instr.set_v_src( 0xffffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 32);
}

TEST_CASE( "MIPS32_instr: clo 0xf0000000")
{
    MIPS32Instr instr( "clo");
    instr.set_v_src( 0xf0000000, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 4);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: clz zero")
{
    CHECK(MIPS32Instr(0x71208820).get_disasm() == "clz $s1, $t1");
    
    MIPS32Instr instr( "clz");
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 32);
}

TEST_CASE( "MIPS32_instr: clz -1")
{
    MIPS32Instr instr( "clz");
    instr.set_v_src( 0xffffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: clz 0x0fff0000")
{
    MIPS32Instr instr( "clz");
    instr.set_v_src( 0x0fff0000, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 4);
}

////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: lui with 0x4d2")
{
    CHECK(MIPS32Instr(0x3c1104d2).get_disasm() == "lui $s1, 0x4d2");

    MIPS32Instr instr( "lui", 0x4d2);
    instr.execute();
    CHECK( instr.get_v_dst( 0)  == 0x4d20000);
}

TEST_CASE( "MIPS32_instr: lui with 1")
{
    MIPS32Instr instr( "lui", 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0)  == 0x10000);
}

TEST_CASE( "MIPS32_instr: lui with 0")
{
    MIPS32Instr instr( "lui", 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0)  == 0);
}

TEST_CASE( "MIPS32_instr: lui with 0xfee1")
{
    MIPS32Instr instr( "lui", 0xfee1);
    instr.execute();
    CHECK( instr.get_v_dst( 0)  == 0xfee10000);
}

////////////////////////////////////////////////////////////////////////////////

static void check_move( std::string_view name)
{
    MIPS32Instr instr( name);
    instr.set_v_src( 0x12345678, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x12345678);
}

TEST_CASE( "MIPS32_instr: mfc0 0x12345678")
{
    CHECK(MIPS32Instr(0x40046800).get_disasm() == "mfc0 $a0, $Cause");
    check_move( "mfc0");
}

////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: mfhi 0xdeadbeef")
{
    CHECK(MIPS32Instr(0x00008810).get_disasm() == "mfhi $s1");
    check_move( "mfhi");
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: mflo 0xfee1dead")
{
    CHECK(MIPS32Instr(0x00008812).get_disasm() == "mflo $s1");
    check_move( "mflo");
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: movn 3 if 0 isn't equal to 0")
{
    CHECK(MIPS32Instr(0x0139880b).get_disasm() == "movn $s1, $t1, $t9");
    
    MIPS32Instr instr( "movn");
    instr.set_v_src( 3, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_mask() == 0);
}
    
TEST_CASE( "MIPS32_instr: movn 0 if 0 isn't equal to 0")
{
    MIPS32Instr instr( "movn");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_mask() == 0);
}
    
TEST_CASE( "MIPS32_instr: movn 1 if 1 isn't equal to 1")
{
    MIPS32Instr instr( "movn");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
    CHECK( instr.get_mask() == all_ones<uint32>());
}

TEST_CASE( "MIPS32_instr: movn 3 if 2 isn't equal to 0")
{
    MIPS32Instr instr( "movn");
    instr.set_v_src( 3, 0);
    instr.set_v_src( 2, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 3);
    CHECK( instr.get_mask() == all_ones<uint32>());
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: movz 3 if 2 is equal to 0")
{
    CHECK(MIPS32Instr(0x0139880a).get_disasm() == "movz $s1, $t1, $t9");

    MIPS32Instr instr( "movz");
    instr.set_v_src( 3, 0);
    instr.set_v_src( 2, 1);
    instr.execute();
    CHECK( instr.get_mask() == 0);
}

TEST_CASE( "MIPS32_instr: movz 1 if 1 is equal to 1")
{
    MIPS32Instr instr( "movz");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_mask() == 0);
}

TEST_CASE( "MIPS32_instr: movz 0 if 0 is equal to 0")
{
    MIPS32Instr instr( "movz");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
    CHECK( instr.get_mask() == all_ones<uint32>());
}

TEST_CASE( "MIPS32_instr: movz 3 if 0 is equal to 0")
{
    MIPS32Instr instr( "movz");
    instr.set_v_src( 3, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 3);
    CHECK( instr.get_mask() == all_ones<uint32>());
}

TEST_CASE( "MIPS32_instr: mtc0 0x12345678")
{
    CHECK(MIPS32Instr(0x408d2000).get_disasm() == "mtc0 $Context1, $t5");
    check_move( "mtc0");
}

TEST_CASE( "MIPS32_instr: mthi 0x12345678")
{
    CHECK(MIPS32Instr(0x02200011).get_disasm() == "mthi $s1");
    check_move( "mthi");
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: mtlo 0xfeedc0de")
{
    CHECK(MIPS32Instr(0x02200013).get_disasm() == "mtlo $s1");
    check_move( "mtlo");
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: nop")
{
    CHECK(MIPS32Instr(0x00000000).get_disasm() == "nop");
    CHECK(MIPS32Instr     ("nop").get_disasm() == "nop");
    MIPS32Instr( "nop").execute();
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: nor 0 and 0")
{
    CHECK(MIPS32Instr(0x01398827).get_disasm() == "nor $s1, $t1, $t9");

    MIPS32Instr instr( "nor");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xffffffff);
}

TEST_CASE( "MIPS32_instr: nor 1 and 1")
{
    MIPS32Instr instr( "nor");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xfffffffe);
}

TEST_CASE( "MIPS32_instr: nor 1 and -1")
{
    MIPS32Instr instr( "nor");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: or 0 and 0")
{
    CHECK(MIPS32Instr(0x01398825).get_disasm() == "or $s1, $t1, $t9");
    
    MIPS32Instr instr( "or");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: or 1 and 1")
{
    MIPS32Instr instr( "or");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS32_instr: or 1 and -1")
{
    MIPS32Instr instr( "or");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xffffffff);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: ori 0 and 0")
{
    CHECK(MIPS32Instr(0x353104d2).get_disasm() == "ori $s1, $t1, 0x4d2");
    
    MIPS32Instr instr( "ori", 0);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: ori 1 and 1")
{
    MIPS32Instr instr( "ori", 1);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS32_instr: ori 1 and -1")
{
    MIPS32Instr instr( "ori", 0x0000ffff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x0000ffff);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE ( "MIPS32_instr: sll 0x00098cc0 by 0")
{
    CHECK(MIPS32Instr(0x00098cc0).get_disasm() == "sll $s1, $t1, 19");
    
    MIPS32Instr instr( "sll", 0);
    instr.set_v_src( 0x00098cc0, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x00098cc0);
}

TEST_CASE ( "MIPS32_instr: sll 51 by 1")
{
    MIPS32Instr instr( "sll", 1);
    instr.set_v_src( 51, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 102);
}

TEST_CASE ( "MIPS32_instr: sll 0xaabbccdd by 8")
{
    MIPS32Instr instr( "sll", 8);
    instr.set_v_src( 0xaabbccdd, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xbbccdd00);
}

TEST_CASE ( "MIPS32_instr: sll 1 by 31")
{
    MIPS32Instr instr( "sll", 31);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x80000000);
}

TEST_CASE( "MIPS32Instr: slt 0 < 0")
{
    CHECK(MIPS32Instr(0x0139882a).get_disasm() == "slt $s1, $t1, $t9");

    MIPS32Instr instr( "slt");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: slt 1 < 0")
{
    MIPS32Instr instr( "slt");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: slt 0 < 1")
{
    MIPS32Instr instr( "slt");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS32_instr: slt -1 < 0")
{
    MIPS32Instr instr( "slt");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS32_instr: slt 0 < -1")
{
    MIPS32Instr instr( "slt");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: slt 1 < -1")
{
    MIPS32Instr instr( "slt");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: slt -1 < 1")
{
    MIPS32Instr instr( "slt");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: sllv 1 by 0")
{
    CHECK(MIPS32Instr(0x03298804).get_disasm() == "sllv $s1, $t1, $t9");
    
    MIPS32Instr instr( "sllv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS32_instr: sllv 1 by 1")
{
    MIPS32Instr instr( "sllv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 2);
}

TEST_CASE( "MIPS32_instr: sllv 1 by 32 + 8 (shift-variable overflow)")
{
    MIPS32Instr instr( "sllv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 32 + 8, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x100);
}

TEST_CASE( "MIPS32_instr: sllv 1 by 32 (shift-variable overflow)")
{
    MIPS32Instr instr( "sllv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 32, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS32_instr: sllv by 64 (shift-variable overflow)")
{
    MIPS32Instr instr( "sllv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 64, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS32_instr: slti 0 < 0")
{
    CHECK(MIPS32Instr(0x293104d2).get_disasm() == "slti $s1, $t1, 1234");
    CHECK(MIPS32Instr(0x2931fb2e).get_disasm() == "slti $s1, $t1, -1234");

    MIPS32Instr instr( "slti", 0);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: slti 1 < 0")
{
    MIPS32Instr instr( "slti", 0);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: slti 0 < 1")
{
    MIPS32Instr instr( "slti", 1);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS32_instr: slti -1 < 0")
{
    MIPS32Instr instr( "slti", 0);
    instr.set_v_src( 0xffffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS32_instr: slti 0 < -1")
{
    MIPS32Instr instr( "slti", 0xffff);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: slti 1 < -1")
{
    MIPS32Instr instr( "slti", 0xffff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: slti -1 < 1")
{
    MIPS32Instr instr( "slti", 1);
    instr.set_v_src( 0xffffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: sltiu 0 < 0")
{
    CHECK(MIPS32Instr(0x2d3104d2).get_disasm() == "sltiu $s1, $t1, 1234");
    CHECK(MIPS32Instr(0x2d31fb2e).get_disasm() == "sltiu $s1, $t1, -1234");
    
    MIPS32Instr instr( "sltiu", 0);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: sltiu 1 < 0")
{
    MIPS32Instr instr( "sltiu", 0);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: sltiu 0 < 1")
{
    MIPS32Instr instr( "sltiu", 1);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS32_instr: sltiu -1 < 0")
{
    MIPS32Instr instr( "sltiu", 0);
    instr.set_v_src( 0xffffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: sltiu 0 < -1")
{
    MIPS32Instr instr( "sltiu", 0xffff);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS32_instr: sltiu -1 < 1")
{
    MIPS32Instr instr( "sltiu", 1);
    instr.set_v_src( 0xffffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: sltiu 1 < -1")
{
    MIPS32Instr instr( "sltiu", 0xffff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: sltu 0 < 0")
{
    CHECK(MIPS32Instr(0x0139882b).get_disasm() == "sltu $s1, $t1, $t9");
    
    MIPS32Instr instr( "sltu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: sltu 1 < 0")
{
    MIPS32Instr instr( "sltu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: sltu 0 < 1")
{
    MIPS32Instr instr( "sltu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS32_instr: sltu -1 < 0")
{
    MIPS32Instr instr( "sltu");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: sltu 0 < -1")
{
    MIPS32Instr instr( "sltu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS32_instr: sltu -1 < 1")
{
    MIPS32Instr instr( "sltu");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: sra 0xabcd1234 by 0")
{
    CHECK(MIPS32Instr(0x00098cc3).get_disasm() == "sra $s1, $t1, 19");
    
    MIPS32Instr instr( "sra", 0);
    instr.set_v_src( 0xabcd1234, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xabcd1234);
}

TEST_CASE( "MIPS32_instr: sra 49 by 1")
{
    MIPS32Instr instr( "sra", 1);
    instr.set_v_src( 49, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 24);
}

TEST_CASE( "MIPS32_instr: sra 0x1000 by 4")
{
    MIPS32Instr instr( "sra", 4);
    instr.set_v_src( 0x1000, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x100);
}

TEST_CASE( "MIPS32_instr: sra 0xffa00000 by 8")
{
    MIPS32Instr instr( "sra", 8);
    instr.set_v_src( 0xffa00000, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xffffa000);
}

TEST_CASE( "MIPS32_instr: srav 0x321 by 0")
{
    CHECK(MIPS32Instr(0x03298807).get_disasm() == "srav $s1, $t1, $t9");
    
    MIPS32Instr instr( "srav");
    instr.set_v_src( 0x321, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x321);
}

TEST_CASE( "MIPS32_instr: srav 14 by 0xffffff03 shift-variable overflow")
{
    MIPS32Instr instr( "srav");
    instr.set_v_src( 14, 0);
    instr.set_v_src( 0xffffff03, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS32_instr: srav 0x333311 by 4")
{
    MIPS32Instr instr( "srav");
    instr.set_v_src( 0x333311, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x33331);
}

TEST_CASE( "MIPS32_instr: srav 0xaabb0000 by 4")
{
    MIPS32Instr instr( "srav");
    instr.set_v_src( 0xaabb0000, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xfaabb000);
}

TEST_CASE ( "MIPS32_instr: srl 0xdeadbeef by 0")
{
    CHECK(MIPS32Instr(0x00098cc2).get_disasm() == "srl $s1, $t1, 19");
    
    MIPS32Instr instr( "srl", 0);
    instr.set_v_src( 0xdeadbeef, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xdeadbeef);
}

TEST_CASE ( "MIPS32_instr: srl 0xabcd1234 by 5")
{
    MIPS32Instr instr( "srl", 5);
    instr.set_v_src( 0xabcd1234, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x055e6891);
}

TEST_CASE ( "MIPS32_instr: srl 0xc0dec0de by 4")
{
    MIPS32Instr instr( "srl", 4);
    instr.set_v_src( 0xc0dec0de, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x0c0dec0d);
}

TEST_CASE ( "MIPS32_instr: srl 0x80000000 by 16")
{
    MIPS32Instr instr( "srl", 16);
    instr.set_v_src( 0x80000000, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x00008000);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: srlv 0xa1 by 0")
{
    CHECK(MIPS32Instr(0x03298806).get_disasm() == "srlv $s1, $t1, $t9");
    
    MIPS32Instr instr( "srlv");
    instr.set_v_src( 0xa1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xa1);
}

TEST_CASE( "MIPS32_instr: srlv 153 by 3")
{
    MIPS32Instr instr( "srlv");
    instr.set_v_src( 153, 0);
    instr.set_v_src( 3, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 19);
}

TEST_CASE( "MIPS32_instr: srlv 0xfeed by 8")
{
    MIPS32Instr instr( "srlv");
    instr.set_v_src( 0xfeed, 0);
    instr.set_v_src( 8, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x00fe);
}

TEST_CASE( "MIPS32_instr: srlv 0xaaa00000 by 4")
{
    MIPS32Instr instr( "srlv");
    instr.set_v_src( 0xaaa00000, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x0aaa0000);
}

TEST_CASE( "MIPS32_instr: srlv 1 by 32 (shift-variable overflow)")
{
    MIPS32Instr instr( "srlv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 32, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS32_instr: sub 1 from 1")
{
    CHECK(MIPS32Instr(0x01398822).get_disasm() == "sub $s1, $t1, $t9");
    MIPS32Instr instr( "sub");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK(instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: sub 1 from 10")
{
    MIPS32Instr instr( "sub");
    instr.set_v_src( 10, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK(instr.get_v_dst( 0) == 9);
}

TEST_CASE( "MIPS32_instr: sub 0 from 1")
{
    MIPS32Instr instr( "sub");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK(instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS32_instr: sub overflow")
{
    MIPS32Instr instr( "sub");
    instr.set_v_src( 0x80000000, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK(instr.get_v_dst( 0) == NO_VAL32);
    CHECK(instr.trap_type() == Trap::INTEGER_OVERFLOW);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: subu 1 from 1")
{
    CHECK(MIPS32Instr(0x01398823).get_disasm() == "subu $s1, $t1, $t9");
    MIPS32Instr instr( "subu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK(instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: subu 1 from 10")
{ 
    MIPS32Instr instr( "subu");
    instr.set_v_src( 10, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK(instr.get_v_dst( 0) == 9);
}

TEST_CASE( "MIPS32_instr: subu 0 from 1")
{   
    MIPS32Instr instr( "subu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK(instr.get_v_dst( 0) == 1);
}

////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: xor 0 and 0")
{
    CHECK(MIPS32Instr(0x01398826).get_disasm() == "xor $s1, $t1, $t9");

    MIPS32Instr instr( "xor");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: xor -1 and -1")
{
    MIPS32Instr instr( "xor");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: xor 1 and -1")
{
    MIPS32Instr instr( "xor");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xfffffffe);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: xori 0 with 0")
{
    CHECK(MIPS32Instr(0x393104d2).get_disasm() == "xori $s1, $t1, 0x4d2");
    MIPS32Instr instr( "xori", 0);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK(instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS32_instr: xori 0 with 1")
{
    MIPS32Instr instr( "xori", 1);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK(instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS32_instr: xori 1 with 0")
{
    MIPS32Instr instr( "xori", 0);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK(instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS32_instr: xori 0xa with 0x5")
{   
    MIPS32Instr instr( "xori", 5);
    instr.set_v_src( 0xa, 0);
    instr.execute();
    CHECK(instr.get_v_dst( 0) == 0xf);
}
////////////////////////////////////////////////////////////////////////////////
