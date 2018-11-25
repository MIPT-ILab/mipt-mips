// Catch2
#include <catch.hpp>

// uArchSim modules
#include "../mips_instr.h"

TEST_CASE( "MIPS32_instr_init: Process_Wrong_Args_Of_Constr")
{
    CHECK_NOTHROW( MIPS32Instr(0x0).execute());
}

TEST_CASE( "MIPS32_instr: invalid instruction in ctor from string")
{
    MIPS32Instr instr( "invalid_instruction_12345678");
    CHECK_THROWS_AS( instr.execute(), UnknownMIPSInstruction);
}

TEST_CASE( "MIPS32_instr: MIPS64_instr in MIPS32_instr ctor")
{
    MIPS32Instr instr( "dsllv");
    CHECK_THROWS_AS( instr.execute(), UnknownMIPSInstruction);
}

TEST_CASE( "MIPS32_instr: Divmult")
{
    CHECK(MIPS32Instr(0x02290018).is_divmult());
    CHECK(MIPS32Instr(0x0229001c).is_divmult());
    CHECK(MIPS32Instr(0x02290019).is_divmult());
    CHECK(MIPS32Instr(0x0229001d).is_divmult());
    CHECK(MIPS32Instr(0x72290000).is_divmult());
    CHECK(MIPS32Instr(0x72290001).is_divmult());
    CHECK(MIPS32Instr(0x72290004).is_divmult());
    CHECK(MIPS32Instr(0x72290005).is_divmult());
    CHECK(MIPS32Instr("mult").is_divmult());
    CHECK(MIPS32Instr("multu").is_divmult());
    CHECK(MIPS32Instr("div").is_divmult());
    CHECK(MIPS32Instr("divu").is_divmult());
    CHECK(MIPS32Instr("dmult").is_divmult());
    CHECK(MIPS32Instr("dmultu").is_divmult());
    CHECK(MIPS32Instr("ddiv").is_divmult());
    CHECK(MIPS32Instr("ddivu").is_divmult());
}

TEST_CASE( "MIPS32_instr_disasm: Process_Disasm_R")
{
    CHECK(MIPS32Instr(0x0139882C).get_disasm() == "dadd $s1, $t1, $t9");
    CHECK(MIPS32Instr(0x0139882D).get_disasm() == "daddu $s1, $t1, $t9");
    CHECK(MIPS32Instr(0x0229001a).get_disasm() == "div $s1, $t1");
    CHECK(MIPS32Instr(0x0229001e).get_disasm() == "ddiv $s1, $t1");
    CHECK(MIPS32Instr(0x0229001b).get_disasm() == "divu $s1, $t1");
    CHECK(MIPS32Instr(0x0229001f).get_disasm() == "ddivu $s1, $t1");
    CHECK(MIPS32Instr(0x0229001c).get_disasm() == "dmult $s1, $t1");
    CHECK(MIPS32Instr(0x0229001d).get_disasm() == "dmultu $s1, $t1");
    CHECK(MIPS32Instr(0x72290000).get_disasm() == "madd $s1, $t1");
    CHECK(MIPS32Instr(0x72290001).get_disasm() == "maddu $s1, $t1");
    CHECK(MIPS32Instr(0x72290004).get_disasm() == "msub $s1, $t1");
    CHECK(MIPS32Instr(0x72290005).get_disasm() == "msubu $s1, $t1");
    CHECK(MIPS32Instr(0x00098cf8).get_disasm() == "dsll $s1, $t1, 19");
    CHECK(MIPS32Instr(0x00098cfc).get_disasm() == "dsll32 $s1, $t1, 19");
    CHECK(MIPS32Instr(0x00098cfb).get_disasm() == "dsra $s1, $t1, 19");
    CHECK(MIPS32Instr(0x00098cff).get_disasm() == "dsra32 $s1, $t1, 19");
    CHECK(MIPS32Instr(0x00098cfa).get_disasm() == "dsrl $s1, $t1, 19");
    CHECK(MIPS32Instr(0x00098cfe).get_disasm() == "dsrl32 $s1, $t1, 19");
    CHECK(MIPS32Instr(0x01398822).get_disasm() == "sub $s1, $t1, $t9");
    CHECK(MIPS32Instr(0x0139882e).get_disasm() == "dsub $s1, $t1, $t9");
    CHECK(MIPS32Instr(0x01398823).get_disasm() == "subu $s1, $t1, $t9");
    CHECK(MIPS32Instr(0x0139882f).get_disasm() == "dsubu $s1, $t1, $t9");
    CHECK(MIPS32Instr(0x0139882a).get_disasm() == "slt $s1, $t1, $t9");
}

TEST_CASE( "MIPS32_instr_disasm: Process_Disasm_IJ")
{
    CHECK(MIPS32Instr(0x613104d2).get_disasm() == "daddi $s1, $t1, 0x4d2");
    CHECK(MIPS32Instr(0x6131fb2e).get_disasm() == "daddi $s1, $t1, 0xfb2e");
    CHECK(MIPS32Instr(0x653104d2).get_disasm() == "daddiu $s1, $t1, 0x4d2");
    CHECK(MIPS32Instr(0x6531fb2e).get_disasm() == "daddiu $s1, $t1, 0xfb2e");
    CHECK(MIPS32Instr(0x293104d2).get_disasm() == "slti $s1, $t1, 0x4d2");
    CHECK(MIPS32Instr(0x2931fb2e).get_disasm() == "slti $s1, $t1, 0xfb2e");
    CHECK(MIPS32Instr(0x393104d2).get_disasm() == "xori $s1, $t1, 0x4d2");
    CHECK(MIPS32Instr(0x3c1104d2).get_disasm() == "lui $s1, 0x4d2");
}

TEST_CASE( "MIPS32_instr_disasm: Process_Disasm_Load_Store")
{
    CHECK(MIPS32Instr(0x813104d2).get_disasm() == "lb $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0x8131fb2e).get_disasm() == "lb $s1, 0xfb2e($t1)");
    CHECK(MIPS32Instr(0x913104d2).get_disasm() == "lbu $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0x9131fb2e).get_disasm() == "lbu $s1, 0xfb2e($t1)");
    CHECK(MIPS32Instr(0x853104d2).get_disasm() == "lh $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0x8531fb2e).get_disasm() == "lh $s1, 0xfb2e($t1)");
    CHECK(MIPS32Instr(0x953104d2).get_disasm() == "lhu $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0x9531fb2e).get_disasm() == "lhu $s1, 0xfb2e($t1)");
    CHECK(MIPS32Instr(0x8d3104d2).get_disasm() == "lw $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0x8d31fb2e).get_disasm() == "lw $s1, 0xfb2e($t1)");
    CHECK(MIPS32Instr(0x893104d2).get_disasm() == "lwl $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0x8931fb2e).get_disasm() == "lwl $s1, 0xfb2e($t1)");
    CHECK(MIPS32Instr(0x993104d2).get_disasm() == "lwr $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0x9931fb2e).get_disasm() == "lwr $s1, 0xfb2e($t1)");
    CHECK(MIPS32Instr(0xc13104d2).get_disasm() == "ll $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xc131fb2e).get_disasm() == "ll $s1, 0xfb2e($t1)");
    CHECK(MIPS32Instr(0xdd3104d2).get_disasm() == "ld $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xdd31fb2e).get_disasm() == "ld $s1, 0xfb2e($t1)");
    CHECK(MIPS32Instr(0x693104d2).get_disasm() == "ldl $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0x6931fb2e).get_disasm() == "ldl $s1, 0xfb2e($t1)");
    CHECK(MIPS32Instr(0x6d3104d2).get_disasm() == "ldr $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0x6d31fb2e).get_disasm() == "ldr $s1, 0xfb2e($t1)");
    CHECK(MIPS32Instr(0xa13104d2).get_disasm() == "sb $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xa131fb2e).get_disasm() == "sb $s1, 0xfb2e($t1)");
    CHECK(MIPS32Instr(0xa53104d2).get_disasm() == "sh $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xa531fb2e).get_disasm() == "sh $s1, 0xfb2e($t1)");
    CHECK(MIPS32Instr(0xad3104d2).get_disasm() == "sw $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xad31fb2e).get_disasm() == "sw $s1, 0xfb2e($t1)");
    CHECK(MIPS32Instr(0xa93104d2).get_disasm() == "swl $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xa931fb2e).get_disasm() == "swl $s1, 0xfb2e($t1)");
    CHECK(MIPS32Instr(0xb93104d2).get_disasm() == "swr $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xb931fb2e).get_disasm() == "swr $s1, 0xfb2e($t1)");
    CHECK(MIPS32Instr(0xe13104d2).get_disasm() == "sc $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xe131fb2e).get_disasm() == "sc $s1, 0xfb2e($t1)");
    CHECK(MIPS32Instr(0xfd3104d2).get_disasm() == "sd $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xfd31fb2e).get_disasm() == "sd $s1, 0xfb2e($t1)");
    CHECK(MIPS32Instr(0xb13104d2).get_disasm() == "sdl $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xb131fb2e).get_disasm() == "sdl $s1, 0xfb2e($t1)");
    CHECK(MIPS32Instr(0xb53104d2).get_disasm() == "sdr $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xb531fb2e).get_disasm() == "sdr $s1, 0xfb2e($t1)");
}

TEST_CASE( "MIPS32_instr_disasm: Process_Disasm_Branches")
{
    CHECK(MIPS32Instr(0x0621fffc).get_disasm() == "bgez $s1, -4");
    CHECK(MIPS32Instr(0x0621000c).get_disasm() == "bgez $s1, 12");
    CHECK(MIPS32Instr(0x0631fffa).get_disasm() == "bgezal $s1, -6");
    CHECK(MIPS32Instr(0x0631000a).get_disasm() == "bgezal $s1, 10");
    CHECK(MIPS32Instr(0x1e20fff9).get_disasm() == "bgtz $s1, -7");
    CHECK(MIPS32Instr(0x1e200008).get_disasm() == "bgtz $s1, 8");
    CHECK(MIPS32Instr(0x1a20fff7).get_disasm() == "blez $s1, -9");
    CHECK(MIPS32Instr(0x1a200006).get_disasm() == "blez $s1, 6");
    CHECK(MIPS32Instr(0x0630fff5).get_disasm() == "bltzal $s1, -11");
    CHECK(MIPS32Instr(0x06300004).get_disasm() == "bltzal $s1, 4");
    CHECK(MIPS32Instr(0x0620fff3).get_disasm() == "bltz $s1, -13");
    CHECK(MIPS32Instr(0x06200002).get_disasm() == "bltz $s1, 2");
    CHECK(MIPS32Instr(0x0622fff3).get_disasm() == "bltzl $s1, -13");
    CHECK(MIPS32Instr(0x06220003).get_disasm() == "bltzl $s1, 3");
    CHECK(MIPS32Instr(0x0623fffd).get_disasm() == "bgezl $s1, -3");
    CHECK(MIPS32Instr(0x0623000d).get_disasm() == "bgezl $s1, 13");
    CHECK(MIPS32Instr(0x0632fff6).get_disasm() == "bltzall $s1, -10");
    CHECK(MIPS32Instr(0x06320003).get_disasm() == "bltzall $s1, 3");
    CHECK(MIPS32Instr(0x0633fffb).get_disasm() == "bgezall $s1, -5");
    CHECK(MIPS32Instr(0x0633000a).get_disasm() == "bgezall $s1, 10");
}


// ********* Converted SPIM TT tests with some additions **********
// 55 done

////////////////////////////////////////////////////////////////////////////////

TEST_CASE ( "MIPS32_instr: add test_1")
{
    CHECK(MIPS32Instr(0x01398820).get_disasm() == "add $s1, $t1, $t9");
    
    MIPS32Instr instr( "add");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE ( "MIPS32_instr: add test_2")
{
    MIPS32Instr instr( "add");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE ( "MIPS32_instr: add test_3")
{
    MIPS32Instr instr( "add");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

/*     Overflow exception isn't implemented (#130)
TEST_CASE ( "MIPS32_instr: add test_4")
{
    MIPS32Instr instr( "add");
    instr.set_v_dst( 0xfee1dead);
    instr.set_v_src( 0x7fffffff, 0);
    instr.set_v_src( 0x7fffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfee1dead);
    CHECK( instr.trap_type() != Trap::NO_TRAP);
} */
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: addi test_1")
{
    CHECK(MIPS32Instr(0x213104d2).get_disasm() == "addi $s1, $t1, 0x4d2");
    CHECK(MIPS32Instr(0x2131fb2e).get_disasm() == "addi $s1, $t1, 0xfb2e");
    
    MIPS32Instr instr( "addi");
    instr.set_v_src( 0, 0);
    instr.set_v_imm( 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: addi test_2")
{
    MIPS32Instr instr( "addi");
    instr.set_v_src( 0, 0);
    instr.set_v_imm( 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: addi test_3")
{
    MIPS32Instr instr( "addi");
    instr.set_v_src( 1, 0);
    instr.set_v_imm( 0xffff);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}
    
/*      Overflow exception isn't implemented (#130)
TEST_CASE( "MIPS32_instr: addi test_4")
{
    MIPS32Instr instr( "addi");
    instr.set_v_dst( 0xfee1dead);
    instr.set_v_src( 0x7fffffff, 0);
    instr.set_v_imm( 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfee1dead);
    CHECK( instr.trap_type() != Trap::NO_TRAP);
}*/

////////////////////////////////////////////////////////////////////////////////
    
TEST_CASE( "MIPS32_instr: addiu test_1")
{
    CHECK(MIPS32Instr(0x253104d2).get_disasm() == "addiu $s1, $t1, 0x4d2");
    CHECK(MIPS32Instr(0x2531fb2e).get_disasm() == "addiu $s1, $t1, 0xfb2e");
    
    MIPS32Instr instr( "addiu");
    instr.set_v_src( 0, 0);
    instr.set_v_imm( 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: addiu test_2")
{
    MIPS32Instr instr( "addiu");
    instr.set_v_src( 0, 0);
    instr.set_v_imm( 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: addiu test_3")
{
    MIPS32Instr instr( "addiu");
    instr.set_v_src( 1, 0);
    instr.set_v_imm( 0xffff);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: addiu test_4")
{
    MIPS32Instr instr( "addiu");
    instr.set_v_src( 0x7fffffff, 0);
    instr.set_v_imm( 2);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x80000001);
    CHECK( instr.has_trap() == false);
}


TEST_CASE( "MIPS64_instr: addiu test_1")
{
    CHECK(MIPS64Instr(0x253104d2).get_disasm() == "addiu $s1, $t1, 0x4d2");
    CHECK(MIPS64Instr(0x2531fb2e).get_disasm() == "addiu $s1, $t1, 0xfb2e");
    
    MIPS64Instr instr( "addiu");
    instr.set_v_src( 0, 0);
    instr.set_v_imm( 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS64_instr: addiu test_2")
{
    MIPS64Instr instr( "addiu");
    instr.set_v_src( 0, 0);
    instr.set_v_imm( 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS64_instr: addiu test_3")
{
    MIPS64Instr instr( "addiu");
    instr.set_v_src( 1, 0);
    instr.set_v_imm( 0xffff);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}
    
TEST_CASE( "MIPS64_instr: addiu test_4")
{
    MIPS64Instr instr( "addiu");
    instr.set_v_src( 0x7fffffff, 0);
    instr.set_v_imm( 2);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x80000001);
    CHECK( instr.has_trap() == false);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE ( "MIPS32_instr: addu test_1")
{
    CHECK(MIPS32Instr(0x01398821).get_disasm() == "addu $s1, $t1, $t9");
    
    MIPS32Instr instr( "addu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE ( "MIPS32_instr: addu test_2")
{
    MIPS32Instr instr( "addu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE ( "MIPS32_instr: addu test_3")
{
    MIPS32Instr instr( "addu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE ( "MIPS32_instr: addu test_4")
{
    MIPS32Instr instr( "addu");
    instr.set_v_src( 0x7fffffff, 0);
    instr.set_v_src( 0x7fffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfffffffe);
    CHECK( instr.has_trap() == false);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE ( "MIPS32_instr: and test_1")
{
    CHECK(MIPS32Instr(0x01398824).get_disasm() == "and $s1, $t1, $t9");
    
    MIPS32Instr instr( "and");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE ( "MIPS32_instr: and test_2")
{
    MIPS32Instr instr( "and");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE ( "MIPS32_instr: and test_3")
{
    MIPS32Instr instr( "and");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE ( "MIPS32_instr: andi test_1")
{
    CHECK(MIPS32Instr(0x313104d2).get_disasm() == "andi $s1, $t1, 0x4d2");
    
    MIPS32Instr instr( "andi");
    instr.set_v_src( 0, 0);
    instr.set_v_imm( 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE ( "MIPS32_instr: andi test_2")
{
    MIPS32Instr instr( "andi");
    instr.set_v_src( 1, 0);
    instr.set_v_imm( 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE ( "MIPS32_instr: andi test_3")
{
    MIPS32Instr instr( "andi");
    instr.set_v_src( 1, 0);
    instr.set_v_imm( 0xffff);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE ( "MIPS32_instr: andi test_4")
{
    MIPS32Instr instr( "andi");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_imm( 0xffff);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x0000ffff);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: beq test_1")
{
    CHECK(MIPS32Instr(0x1229ffff).get_disasm() == "beq $s1, $t1, -1");
    CHECK(MIPS32Instr(0x1229000e).get_disasm() == "beq $s1, $t1, 14");
    
    MIPS32Instr instr( "beq");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.set_v_imm( 1);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: beq test_2")
{
    MIPS32Instr instr( "beq");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.set_v_imm( 1);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: beq test_3")
{
    MIPS32Instr instr( "beq");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4);
}

TEST_CASE( "MIPS32_instr: beq test_4")
{
    MIPS32Instr instr( "beq");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.set_v_imm( 1024);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 + 1024 * 4);
}

TEST_CASE( "MIPS32_instr: beq test_5")
{
    MIPS32Instr instr( "beq");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.set_v_imm( 0xffff - 1024 + 1);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 - 1024 * 4);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: bne test_1")
{
    CHECK(MIPS32Instr(0x1629fff1).get_disasm() == "bne $s1, $t1, -15");
    CHECK(MIPS32Instr(0x16290000).get_disasm() == "bne $s1, $t1, 0");
    
    MIPS32Instr instr( "bne");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.set_v_imm( 1024);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4);
}

TEST_CASE( "MIPS32_instr: bne test_2")
{
    MIPS32Instr instr( "bne");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.set_v_imm( 1);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4);
}
    
TEST_CASE( "MIPS32_instr: bne test_3")
{
    MIPS32Instr instr( "bne");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 1, 1);
    instr.set_v_imm( 1024);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 + 1024 * 4);
}
    
TEST_CASE( "MIPS32_instr: bne test_4")
{
    MIPS32Instr instr( "bne");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.set_v_imm( 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: break test_1")
{
    CHECK(MIPS32Instr(0x0000000d).get_disasm() == "break");
    
    MIPS32Instr instr( "break");
    instr.execute();
    CHECK( instr.trap_type() == Trap::BREAKPOINT);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: clo test_1")
{
    CHECK(MIPS32Instr(0x71208821).get_disasm() == "clo $s1, $t1");
    
    MIPS32Instr instr( "clo");
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: clo test_2")
{
    MIPS32Instr instr( "clo");
    instr.set_v_src( 0xffffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 32);
}

TEST_CASE( "MIPS32_instr: clo test_3")
{
    MIPS32Instr instr( "clo");
    instr.set_v_src( 0xf0000000, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 4);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: clz test_1")
{
    CHECK(MIPS32Instr(0x71208820).get_disasm() == "clz $s1, $t1");
    
    MIPS32Instr instr( "clz");
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 32);
}

TEST_CASE( "MIPS32_instr: clz test_2")
{
    MIPS32Instr instr( "clz");
    instr.set_v_src( 0xffffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: clz test_3")
{
    MIPS32Instr instr( "clz");
    instr.set_v_src( 0x0fff0000, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 4);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dclo test_1")
{
    CHECK(MIPS64Instr(0x71208825).get_disasm() == "dclo $s1, $t1");
    
    MIPS64Instr instr( "dclo");
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS64_instr: dclo test_2")
{
    MIPS64Instr instr( "dclo");
    instr.set_v_src( 0xffffffffffffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 64);
}

TEST_CASE( "MIPS64_instr: dclo test_3")
{
    MIPS64Instr instr( "dclo");
    instr.set_v_src( 0xffe002200011000a, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 11);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dclz test_1")
{
    CHECK(MIPS64Instr(0x71208824).get_disasm() == "dclz $s1, $t1");
    
    MIPS64Instr instr( "dclz");
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 64);
}

TEST_CASE( "MIPS64_instr: dclz test_2")
{
    MIPS64Instr instr( "dclz");
    instr.set_v_src( 0xffffffffffffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS64_instr: dclz test_3")
{
    MIPS64Instr instr( "dclz");
    instr.set_v_src( 0x02ffaa00cc720000, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 6);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dsllv test_1")
{
    CHECK(MIPS64Instr(0x03298814).get_disasm() == "dsllv $s1, $t1, $t9");
    
    MIPS64Instr instr( "dsllv");
    instr.set_v_src( 0xaaaaaaaafee1dead, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xaaaaaaaafee1dead);
}

TEST_CASE( "MIPS64_instr: dsllv test_2")
{
    MIPS64Instr instr( "dsllv");
    
    instr.set_v_src( 2, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 4);
}

TEST_CASE( "MIPS64_instr: dsllv test_3")
{
    MIPS64Instr instr( "dsllv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 32, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x100000000);
}

/* (#709)
 * Bug: This test fails here: CHECK( 0 == 1)
 * Only in case of compilation with:
 *   build = msvc, platform = x86, CMAKEFILE = Visual Studio 15
 * Some other tests marked with sign (#709) also fail.
*/

// Bug-test (#709)
/* ***********************************
TEST_CASE( "MIPS64_instr: dsllv test_4")
{
    MIPS64Instr instr( "dsllv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 64, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}
// ***********************************/

// Bug-test (#709)
/* ***********************************
TEST_CASE( "MIPS64_instr: dsllv test_5")
{
    MIPS64Instr instr( "dsllv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 128, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}
/ ***********************************/
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dsrav test_1")
{
    CHECK(MIPS64Instr(0x03298817).get_disasm() == "dsrav $s1, $t1, $t9");
    
    MIPS64Instr instr( "dsrav");
    instr.set_v_src( 0xfeedabcd, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfeedabcd);
}

TEST_CASE( "MIPS64_instr: dsrav test_2")
{
    MIPS64Instr instr( "dsrav");
    instr.set_v_src( 0xab, 0);
    instr.set_v_src( 0xff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS64_instr: dsrav test_3")
{
    MIPS64Instr instr( "dsrav");
    instr.set_v_src( 0x123400000000, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x012340000000);
}

TEST_CASE( "MIPS64_instr: dsrav test_4")
{
    MIPS64Instr instr( "dsrav");
    instr.set_v_src( 0xffab000000000000, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfffab00000000000);
}

// Bug-test (#709)
/* ***********************************
TEST_CASE( "MIPS64_instr: dsrav test_5")
{
    MIPS64Instr instr( "dsrav");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 64, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}
// ***********************************/
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dsrlv test_1")
{
    CHECK(MIPS64Instr(0x03298816).get_disasm() == "dsrlv $s1, $t1, $t9");
    
    MIPS64Instr instr( "dsrlv");
    instr.set_v_src( 0xdeadbeef, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xdeadbeef);
}

TEST_CASE( "MIPS64_instr: dsrlv test_2")
{
    MIPS64Instr instr( "dsrlv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS64_instr: dsrlv test_3")
{
    MIPS64Instr instr( "dsrlv");
    instr.set_v_src( 0x01a00000, 0);
    instr.set_v_src( 8, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x0001a000);
}

TEST_CASE( "MIPS64_instr: dsrlv test_4")
{
    MIPS64Instr instr( "dsrlv");
    instr.set_v_src( 0x8765432000000011, 0);
    instr.set_v_src( 16, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x0000876543200000);
}

// Bug-test (#709)
/* ***********************************
TEST_CASE( "MIPS64_instr: dsrlv test_5")
{
    MIPS64Instr instr( "dsrlv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 64, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}
// ***********************************/
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: j test_1")
{
    CHECK(MIPS32Instr(0x0bfffb2e).get_disasm() == "j 0x3fffb2e");
    CHECK(MIPS32Instr(0x080004d2).get_disasm() == "j 0x4d2");
    
    MIPS32Instr instr( "j");
    instr.set_v_imm( 4);
    instr.execute();
    CHECK( instr.get_new_PC() == 16);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: jal test_1")
{
    CHECK(MIPS32Instr(0x0ffffb2e).get_disasm() == "jal 0x3fffb2e");
    CHECK(MIPS32Instr(0x0c0004d2).get_disasm() == "jal 0x4d2");
    
    MIPS32Instr instr( "jal");
    instr.set_v_imm( 0xfff);
    instr.execute();
    CHECK( instr.get_new_PC() == 0xfff * 4);
    CHECK( instr.get_v_dst() == instr.get_PC() + 4);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: jalr test_1")
{
    CHECK(MIPS32Instr(0x01208809).get_disasm() == "jalr $s1, $t1");

    MIPS32Instr instr( "jalr");
    instr.set_v_src( 1024, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == 1024);
    CHECK( instr.get_v_dst() == instr.get_PC() + 4);
}

TEST_CASE( "MIPS32_instr: jalr test_2")
{
    MIPS32Instr instr( "jalr");
    instr.set_v_src( 2, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == 4);
    CHECK( instr.get_v_dst() == instr.get_PC() + 4);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: jr test_1")
{
    CHECK(MIPS32Instr(0x02200008).get_disasm() == "jr $s1");
    
    MIPS32Instr instr( "jr");
    instr.set_v_src( 0xfffffff4, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == 0xfffffff4);
}

TEST_CASE( "MIPS32_instr: jr test_2")
{
    MIPS32Instr instr( "jr");
    instr.set_v_src( 2, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == 4);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: mfhi test_1")
{
    CHECK(MIPS32Instr(0x00008810).get_disasm() == "mfhi $s1");
    
    MIPS32Instr instr( "mfhi");
    instr.set_v_dst( 0);
    instr.set_v_src( 0xdeadbeef, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xdeadbeef);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: mflo test_1")
{
    CHECK(MIPS32Instr(0x00008812).get_disasm() == "mflo $s1");
    
    MIPS32Instr instr( "mflo");
    instr.set_v_dst( 0);
    instr.set_v_src( 0xfee1dead, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfee1dead);
}
////////////////////////////////////////////////////////////////////////////////

/*      Not implemented on the register file level
TEST_CASE( "MIPS32_instr: movn test_1")
{
    CHECK(MIPS32Instr(0x0139880b).get_disasm() == "movn $s1, $t1, $t9");
    
    MIPS32Instr instr( "movn");
    instr.set_v_src( 3, 0);
    instr.set_v_src( 0, 1);
    instr.set_v_dst( 4);
    instr.execute();
    CHECK( instr.get_v_dst() == 4);
}

TEST_CASE( "MIPS32_instr: movn test_2")
{
    MIPS32Instr instr( "movn");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.set_v_dst( 4);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}
    
TEST_CASE( "MIPS32_instr: movn test_3")
{
    MIPS32Instr instr( "movn");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.set_v_dst( 4);
    instr.execute();
    CHECK( instr.get_v_dst() == 4);
}

TEST_CASE( "MIPS32_instr: movn test_4")
{
    MIPS32Instr instr( "movn");
    instr.set_v_src( 3, 0);
    instr.set_v_src( 2, 1);
    instr.set_v_dst( 4);
    instr.execute();
    CHECK( instr.get_v_dst() == 3);
} */


// Current version uses mask
TEST_CASE( "MIPS32_instr: movn test_1 (mask)")
{
    CHECK(MIPS32Instr(0x0139880b).get_disasm() == "movn $s1, $t1, $t9");
    
    MIPS32Instr instr( "movn");
    instr.set_v_src( 3, 0);
    instr.set_v_src( 0, 1);
    instr.set_v_dst( 4);
    instr.execute();
    CHECK( instr.get_mask() == 0);
}
    
TEST_CASE( "MIPS32_instr: movn test_2 (mask)")
{
    MIPS32Instr instr( "movn");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.set_v_dst( 4);
    instr.execute();
    CHECK( instr.get_mask() == 0);
}
    
TEST_CASE( "MIPS32_instr: movn test_3 (mask)")
{
    MIPS32Instr instr( "movn");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.set_v_dst( 4);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
    CHECK( instr.get_mask() == all_ones<uint32>());
}

TEST_CASE( "MIPS32_instr: movn test_4 (mask)")
{
    MIPS32Instr instr( "movn");
    instr.set_v_src( 3, 0);
    instr.set_v_src( 2, 1);
    instr.set_v_dst( 4);
    instr.execute();
    CHECK( instr.get_v_dst() == 3);
    CHECK( instr.get_mask() == all_ones<uint32>());
}
////////////////////////////////////////////////////////////////////////////////

/*      Not implemented on the register file level
TEST_CASE( "MIPS32_instr: movz test_1")
{
    CHECK(MIPS32Instr(0x0139880a).get_disasm() == "movz $s1, $t1, $t9");

    MIPS32Instr instr( "movz");
    instr.set_v_src( 3, 0);
    instr.set_v_src( 2, 1);
    instr.set_v_dst( 4);
    instr.execute();
    CHECK( instr.get_v_dst() == 4);
}

TEST_CASE( "MIPS32_instr: movz test_2")
{
    MIPS32Instr instr( "movz");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.set_v_dst( 4);
    instr.execute();
    CHECK( instr.get_v_dst() == 4);
}

TEST_CASE( "MIPS32_instr: movz test_3")
{
    MIPS32Instr instr( "movz");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.set_v_dst( 4);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: movz test_4")
{
    MIPS32Instr instr( "movz");
    instr.set_v_src( 3, 0);
    instr.set_v_src( 0, 1);
    instr.set_v_dst( 4);
    instr.execute();
    CHECK( instr.get_v_dst() == 3);
} */

// Current version uses mask
TEST_CASE( "MIPS32_instr: movz test_1 (mask)")
{
    CHECK(MIPS32Instr(0x0139880a).get_disasm() == "movz $s1, $t1, $t9");

    MIPS32Instr instr( "movz");
    instr.set_v_src( 3, 0);
    instr.set_v_src( 2, 1);
    instr.set_v_dst( 4);
    instr.execute();
    CHECK( instr.get_mask() == 0);
}

TEST_CASE( "MIPS32_instr: movz test_2 (mask)")
{
    MIPS32Instr instr( "movz");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.set_v_dst( 4);
    instr.execute();
    CHECK( instr.get_mask() == 0);
}

TEST_CASE( "MIPS32_instr: movz test_3 (mask)")
{
    MIPS32Instr instr( "movz");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.set_v_dst( 4);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
    CHECK( instr.get_mask() == all_ones<uint32>());
}

TEST_CASE( "MIPS32_instr: movz test_4 (mask)")
{
    MIPS32Instr instr( "movz");
    instr.set_v_src( 3, 0);
    instr.set_v_src( 0, 1);
    instr.set_v_dst( 4);
    instr.execute();
    CHECK( instr.get_v_dst() == 3);
    CHECK( instr.get_mask() == all_ones<uint32>());
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: mthi test_1")
{
    CHECK(MIPS32Instr(0x02200011).get_disasm() == "mthi $s1");
    
    MIPS32Instr instr( "mthi");
    instr.set_v_dst( 0);
    instr.set_v_src( 0x12345678, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x12345678);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: mtlo test_1")
{
    CHECK(MIPS32Instr(0x02200013).get_disasm() == "mtlo $s1");
    
    MIPS32Instr instr( "mtlo");
    instr.set_v_dst( 0);
    instr.set_v_src( 0xfeedc0de, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfeedc0de);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: mul test_1")
{
    CHECK(MIPS32Instr(0x71398802).get_disasm() == "mul $s1, $t1, $t9");
    
    MIPS32Instr instr( "mul");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: mul test_2")
{
    MIPS32Instr instr( "mul");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: mul test_3")
{
    MIPS32Instr instr( "mul");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 10, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 10);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: mult test_1")
{
    CHECK(MIPS32Instr(0x02290018).get_disasm() == "mult $s1, $t1");

    MIPS32Instr instr( "mult");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS32_instr: mult test_2")
{
    MIPS32Instr instr( "mult");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 1);
}

TEST_CASE( "MIPS32_instr: mult test_3")
{
    MIPS32Instr instr( "mult");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 1);
}

TEST_CASE( "MIPS32_instr: mult test_4")
{
    MIPS32Instr instr( "mult");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0xffffffff);
    CHECK( instr.get_v_dst()  == 0xffffffff);
}

TEST_CASE( "MIPS32_instr: mult test_5")
{
    MIPS32Instr instr( "mult");
    instr.set_v_src( 0x10000, 0);
    instr.set_v_src( 0x10000, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 1);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS32_instr: mult test_6")
{
    MIPS32Instr instr( "mult");
    instr.set_v_src( 0x80000000, 0);
    instr.set_v_src( 0x80000000, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0x40000000);
    CHECK( instr.get_v_dst()  == 0);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: multu test_1")
{
    CHECK(MIPS32Instr(0x02290019).get_disasm() == "multu $s1, $t1");

    MIPS32Instr instr( "multu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS32_instr: multu test_2")
{
    MIPS32Instr instr( "multu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 1);
}

TEST_CASE( "MIPS32_instr: multu test_3")
{
    MIPS32Instr instr( "multu");    
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0xfffffffe);
    CHECK( instr.get_v_dst()  == 1);
}

TEST_CASE( "MIPS32_instr: multu test_4")
{
    MIPS32Instr instr( "multu");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS32_instr: multu test_5")
{
    MIPS32Instr instr( "multu");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0xffffffff);
}

TEST_CASE( "MIPS32_instr: multu test_6")
{
    MIPS32Instr instr( "multu");
    instr.set_v_src( 0x10000, 0);
    instr.set_v_src( 0x10000, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 1);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS32_instr: multu test_7")
{
    MIPS32Instr instr( "multu");
    instr.set_v_src( 0x80000000, 0);
    instr.set_v_src( 0x80000000, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0x40000000);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS32_instr: multu test_8")
{
    MIPS32Instr instr( "multu");
    instr.set_v_src( 0xcecb8f27, 0);
    instr.set_v_src( 0xfd87b5f2, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0xcccccccb);
    CHECK( instr.get_v_dst()  == 0x7134e5de);
}


TEST_CASE( "MIPS64_instr: multu test_1")
{
    CHECK(MIPS64Instr(0x02290019).get_disasm() == "multu $s1, $t1");
    
    MIPS64Instr instr( "multu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS64_instr: multu test_2")
{
    MIPS64Instr instr( "multu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 1);
}

TEST_CASE( "MIPS64_instr: multu test_3")
{
    MIPS64Instr instr( "multu");
    instr.set_v_src( 0xffffffffffffffff, 0);
    instr.set_v_src( 0xffffffffffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0xfffffffe);
    CHECK( instr.get_v_dst()  == 1);
}

TEST_CASE( "MIPS64_instr: multu test_4")
{
    MIPS64Instr instr( "multu");
    instr.set_v_src( 0xffffffffffffffff, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS64_instr: multu test_5")
{
    MIPS64Instr instr( "multu");
    instr.set_v_src( 0xffffffffffffffff, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0xffffffff);
}

TEST_CASE( "MIPS64_instr: multu test_6")
{
    MIPS64Instr instr( "multu");
    instr.set_v_src( 0x10000, 0);
    instr.set_v_src( 0x10000, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 1);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS64_instr: multu test_7")
{
    MIPS64Instr instr( "multu");
    instr.set_v_src( 0x80000000, 0);
    instr.set_v_src( 0x80000000, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0x40000000);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS64_instr: multu test_8")
{
    MIPS64Instr instr( "multu");
    instr.set_v_src( 0xcecb8f27, 0);
    instr.set_v_src( 0xfd87b5f2, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0xcccccccb);
    CHECK( instr.get_v_dst()  == 0x7134e5de);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: nop test_1")
{
    CHECK(MIPS32Instr(0x00000000).get_disasm() == "nop");
    CHECK(MIPS32Instr     ("nop").get_disasm() == "nop");
    
    MIPS32Instr instr( "nop");
    instr.execute();
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: nor test_1")
{
    CHECK(MIPS32Instr(0x01398827).get_disasm() == "nor $s1, $t1, $t9");

    MIPS32Instr instr( "nor");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffffffff);
}

TEST_CASE( "MIPS32_instr: nor test_2")
{
    MIPS32Instr instr( "nor");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfffffffe);
}

TEST_CASE( "MIPS32_instr: nor test_3")
{
    MIPS32Instr instr( "nor");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}


TEST_CASE( "MIPS64_instr: nor test_1")
{
    CHECK(MIPS64Instr(0x01398827).get_disasm() == "nor $s1, $t1, $t9");

    MIPS64Instr instr( "nor");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffffffffffffffff);
}

TEST_CASE( "MIPS64_instr: nor test_2")
{
    MIPS64Instr instr( "nor");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfffffffffffffffe);
}

TEST_CASE( "MIPS64_instr: nor test_3")
{
    MIPS64Instr instr( "nor");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffffffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: or test_1")
{
    CHECK(MIPS32Instr(0x01398825).get_disasm() == "or $s1, $t1, $t9");
    
    MIPS32Instr instr( "or");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: or test_2")
{
    MIPS32Instr instr( "or");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: or test_3")
{
    MIPS32Instr instr( "or");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffffffff);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: ori test_1")
{
    CHECK(MIPS32Instr(0x353104d2).get_disasm() == "ori $s1, $t1, 0x4d2");
    
    MIPS32Instr instr( "ori");
    instr.set_v_src( 0, 0);
    instr.set_v_imm( 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: ori test_2")
{
    MIPS32Instr instr( "ori");
    instr.set_v_src( 1, 0);
    instr.set_v_imm( 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: ori test_3")
{
    MIPS32Instr instr( "ori");
    instr.set_v_src( 1, 0);
    instr.set_v_imm( 0x0000ffff);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x0000ffff);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE ( "MIPS32_instr: sll test_1")
{
    CHECK(MIPS32Instr(0x00098cc0).get_disasm() == "sll $s1, $t1, 19");
    
    MIPS32Instr instr( "sll");
    instr.set_v_src( 0x00098cc0, 0);
    instr.set_v_imm( 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x00098cc0);
}

TEST_CASE ( "MIPS32_instr: sll test_2")
{
    MIPS32Instr instr( "sll");
    instr.set_v_src( 51, 0);
    instr.set_v_imm( 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 102);
}

TEST_CASE ( "MIPS32_instr: sll test_3")
{
    MIPS32Instr instr( "sll");
    instr.set_v_src( 0xaabbccdd, 0);
    instr.set_v_imm( 8);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xbbccdd00);
}

TEST_CASE ( "MIPS32_instr: sll test_4")
{
    MIPS32Instr instr( "sll");
    instr.set_v_src( 1, 0);
    instr.set_v_imm( 31);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x80000000);
}


TEST_CASE ( "MIPS64_instr: sll test_1")
{
    CHECK(MIPS64Instr(0x00098cc0).get_disasm() == "sll $s1, $t1, 19");

    MIPS64Instr instr( "sll");
    instr.set_v_src( 100, 0);
    instr.set_v_imm( 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 100);
}

TEST_CASE ( "MIPS64_instr: sll test_2")
{
    MIPS64Instr instr( "sll");
    instr.set_v_src( 1, 0);
    instr.set_v_imm( 33);
    instr.execute();
    CHECK( instr.get_v_dst() == 2);
}

TEST_CASE ( "MIPS64_instr: sll test_3")
{
    MIPS64Instr instr( "sll");
    instr.set_v_src( 1, 0);
    instr.set_v_imm( 16);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x10000);
}

TEST_CASE ( "MIPS64_instr: sll test_4")
{
    MIPS64Instr instr( "sll");
    instr.set_v_src( 1, 0);
    instr.set_v_imm( 31);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x80000000);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: sllv test_1")
{
    CHECK(MIPS32Instr(0x03298804).get_disasm() == "sllv $s1, $t1, $t9");
    
    MIPS32Instr instr( "sllv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: sllv test_2")
{
    MIPS32Instr instr( "sllv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 2);
}

TEST_CASE( "MIPS32_instr: sllv test_3")
{
    MIPS32Instr instr( "sllv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 64 + 8, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x100);
}

TEST_CASE( "MIPS32_instr: sllv test_4")
{
    MIPS32Instr instr( "sllv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 32, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

// Bug-test (#709)
/* ***********************************
TEST_CASE( "MIPS32_instr: sllv test_5")
{
    MIPS32Instr instr( "sllv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 64, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}
// ***********************************/

// Bug-test (#709)
/* ***********************************
TEST_CASE( "MIPS64_instr: sllv test_1")
{
    MIPS64Instr instr( "sllv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 64, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}
// ***********************************/
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: sltiu test_1")
{
    CHECK(MIPS32Instr(0x2d3104d2).get_disasm() == "sltiu $s1, $t1, 0x4d2");
    CHECK(MIPS32Instr(0x2d31fb2e).get_disasm() == "sltiu $s1, $t1, 0xfb2e");
    
    MIPS32Instr instr( "sltiu");
    instr.set_v_src( 0, 0);
    instr.set_v_imm( 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: sltiu test_2")
{
    MIPS32Instr instr( "sltiu");
    instr.set_v_src( 1, 0);
    instr.set_v_imm( 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: sltiu test_3")
{
    MIPS32Instr instr( "sltiu");
    instr.set_v_src( 0, 0);
    instr.set_v_imm( 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: sltiu test_4")
{
    MIPS32Instr instr( "sltiu");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_imm( 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: sltiu test_5")
{
    MIPS32Instr instr( "sltiu");
    instr.set_v_src( 0, 0);
    instr.set_v_imm( 0xffff);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: sltiu test_6")
{
    MIPS32Instr instr( "sltiu");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_imm( 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: sltiu test_7")
{
    MIPS32Instr instr( "sltiu");
    instr.set_v_src( 1, 0);
    instr.set_v_imm( 0xffff);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: sltu test_1")
{
    CHECK(MIPS32Instr(0x0139882b).get_disasm() == "sltu $s1, $t1, $t9");
    
    MIPS32Instr instr( "sltu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: sltu test_2")
{
    MIPS32Instr instr( "sltu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: sltu test_3")
{
    MIPS32Instr instr( "sltu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: sltu test_4")
{
    MIPS32Instr instr( "sltu");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: sltu test_5")
{
    MIPS32Instr instr( "sltu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: sltu test_6")
{
    MIPS32Instr instr( "sltu");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: sra test_1")
{
    CHECK(MIPS32Instr(0x00098cc3).get_disasm() == "sra $s1, $t1, 19");
    
    MIPS32Instr instr( "sra");
    instr.set_v_src( 0xabcd1234, 0);
    instr.set_v_imm( 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xabcd1234);
}

TEST_CASE( "MIPS32_instr: sra test_2")
{
    MIPS32Instr instr( "sra");
    instr.set_v_src( 49, 0);
    instr.set_v_imm( 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 24);
}

TEST_CASE( "MIPS32_instr: sra test_3")
{
    MIPS32Instr instr( "sra");
    instr.set_v_src( 0x1000, 0);
    instr.set_v_imm( 4);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x100);
}

TEST_CASE( "MIPS32_instr: sra test_4")
{
    MIPS32Instr instr( "sra");
    instr.set_v_src( 0xffa00000, 0);
    instr.set_v_imm( 8);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffffa000);
}


TEST_CASE( "MIPS64_instr: sra test_1")
{
    CHECK(MIPS64Instr(0x00098cc3).get_disasm() == "sra $s1, $t1, 19");

    MIPS64Instr instr( "sra");
    instr.set_v_src( 0xdeadc0de, 0);
    instr.set_v_imm( 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xdeadc0de);
}

TEST_CASE( "MIPS64_instr: sra test_2")
{
    MIPS64Instr instr( "sra");
    instr.set_v_src( 0x0fffffff, 0);
    instr.set_v_imm( 2);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x03ffffff);
}

TEST_CASE( "MIPS64_instr: sra test_3")
{
    MIPS64Instr instr( "sra");
    instr.set_v_src( 0xdead, 0);
    instr.set_v_imm( 4);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x0dea);
}

TEST_CASE( "MIPS64_instr: sra test_4")
{
    MIPS64Instr instr( "sra");
    instr.set_v_src( 0xf1234567, 0);
    instr.set_v_imm( 16);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfffff123);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: srav test_1")
{
    CHECK(MIPS32Instr(0x03298807).get_disasm() == "srav $s1, $t1, $t9");
    
    MIPS32Instr instr( "srav");
    instr.set_v_src( 0x321, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x321);
}
    
TEST_CASE( "MIPS32_instr: srav test_2")
{
    MIPS32Instr instr( "srav");
    instr.set_v_src( 14, 0);
    instr.set_v_src( 0xffffff03, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: srav test_3")
{
    MIPS32Instr instr( "srav");
    instr.set_v_src( 0x333311, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x33331);
}

TEST_CASE( "MIPS32_instr: srav test_4")
{
    MIPS32Instr instr( "srav");
    instr.set_v_src( 0xaabb0000, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfaabb000);
}


TEST_CASE( "MIPS64_instr: srav test_1")
{
    CHECK(MIPS64Instr(0x03298807).get_disasm() == "srav $s1, $t1, $t9");
    
    MIPS64Instr instr( "srav");
    instr.set_v_src( 24, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 24);
}

TEST_CASE( "MIPS64_instr: srav test_2")
{
    MIPS64Instr instr( "srav");
    instr.set_v_src( 10, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 5);
}

TEST_CASE( "MIPS64_instr: srav test_3")
{
    MIPS64Instr instr( "srav");
    instr.set_v_src( 0x000a, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS64_instr: srav test_4")
{
    MIPS64Instr instr( "srav");
    instr.set_v_src( 0xff000000, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfff00000);
}

// Bug-test (#709)
/* ***********************************
TEST_CASE( "MIPS64_instr: srav test_5")
{
    MIPS64Instr instr( "srav");
    instr.set_v_src( 0xffff0000, 0);
    instr.set_v_src( 64, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffff0000);
}
// ***********************************/
////////////////////////////////////////////////////////////////////////////////

TEST_CASE ( "MIPS32_instr: srl test_1")
{
    CHECK(MIPS32Instr(0x00098cc2).get_disasm() == "srl $s1, $t1, 19");
    
    MIPS32Instr instr( "srl");
    instr.set_v_src( 0xdeadbeef, 0);
    instr.set_v_imm( 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xdeadbeef);
}

TEST_CASE ( "MIPS32_instr: srl test_2")
{
    MIPS32Instr instr( "srl");
    instr.set_v_src( 0xabcd1234, 0);
    instr.set_v_imm( 5);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x055e6891);
}

TEST_CASE ( "MIPS32_instr: srl test_3")
{
    MIPS32Instr instr( "srl");
    instr.set_v_src( 0xc0dec0de, 0);
    instr.set_v_imm( 4);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x0c0dec0d);
}

TEST_CASE ( "MIPS32_instr: srl test_4")
{
    MIPS32Instr instr( "srl");
    instr.set_v_src( 0x80000000, 0);
    instr.set_v_imm( 16);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x00008000);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: srlv test_1")
{
    CHECK(MIPS32Instr(0x03298806).get_disasm() == "srlv $s1, $t1, $t9");
    
    MIPS32Instr instr( "srlv");
    instr.set_v_src( 0xa1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xa1);
}

TEST_CASE( "MIPS32_instr: srlv test_2")
{
    MIPS32Instr instr( "srlv");
    instr.set_v_src( 153, 0);
    instr.set_v_src( 3, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 19);
}

TEST_CASE( "MIPS32_instr: srlv test_3")
{
    MIPS32Instr instr( "srlv");
    instr.set_v_src( 0xfeed, 0);
    instr.set_v_src( 8, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x00fe);
}

TEST_CASE( "MIPS32_instr: srlv test_4")
{
    MIPS32Instr instr( "srlv");
    instr.set_v_src( 0xaaa00000, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x0aaa0000);
}

// Bug-test (#709)
/* ***********************************
TEST_CASE( "MIPS32_instr: srlv test_5")
{
    MIPS32Instr instr( "srlv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 64, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}
// ***********************************/

// Bug-test (#709)
/* ***********************************
TEST_CASE( "MIPS64_instr: srlv test_1")
{
    MIPS64Instr instr( "srlv");
    instr.set_v_src( 0x11, 0);
    instr.set_v_src( 0x00000a00, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x11);
}
// ***********************************/
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: syscall test_1")
{
    CHECK(MIPS32Instr(0x0000000c).get_disasm() == "syscall");
    
    MIPS32Instr instr( "syscall");
    instr.execute();
    CHECK( instr.trap_type() == Trap::SYSCALL);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: teq test_eq")
{
    CHECK(MIPS32Instr(0x02290034).get_disasm() == "teq $s1, $t1");
    
    MIPS32Instr instr( "teq");
    instr.set_v_src( 15, 0);
    instr.set_v_src( 15, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}

TEST_CASE( "MIPS32_instr: teq test_ne")
{
    MIPS32Instr instr( "teq");
    instr.set_v_src( 0xff, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: teqi test_eq")
{
    CHECK(MIPS32Instr(0x062c04d2).get_disasm() == "teqi $s1, 0x4d2");
    CHECK(MIPS32Instr(0x062cfb2e).get_disasm() == "teqi $s1, 0xfb2e");
    
    MIPS32Instr instr( "teqi");
    instr.set_v_src( 0, 0);
    instr.set_v_imm( 0);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}

TEST_CASE( "MIPS32_instr: teqi test_ne")
{
    MIPS32Instr instr( "teqi");
    instr.set_v_src( 0, 0);
    instr.set_v_imm( 4);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: tge test_l")
{
    CHECK(MIPS32Instr(0x02290030).get_disasm() == "tge $s1, $t1");
    
    MIPS32Instr instr( "tge");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 2, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}

TEST_CASE( "MIPS32_instr: tge test_eq")
{
    MIPS32Instr instr( "tge");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}

TEST_CASE( "MIPS32_instr: tge test_g")
{
    MIPS32Instr instr( "tge");
    instr.set_v_src( 2, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: tgei test_l")
{
    CHECK(MIPS32Instr(0x062804d2).get_disasm() == "tgei $s1, 0x4d2");
    CHECK(MIPS32Instr(0x0628fb2e).get_disasm() == "tgei $s1, 0xfb2e");
    
    MIPS32Instr instr( "tgei");
    instr.set_v_src( 0, 0);
    instr.set_v_imm( 4);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}

TEST_CASE( "MIPS32_instr: tgei test_eq")
{
    MIPS32Instr instr( "tgei");
    instr.set_v_src( 0, 0);
    instr.set_v_imm( 0);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}

TEST_CASE( "MIPS32_instr: tgei test_g")
{
    MIPS32Instr instr( "tgei");
    instr.set_v_src( 8, 0);
    instr.set_v_imm( 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: tgeiu test_l")
{
    CHECK(MIPS32Instr(0x062904d2).get_disasm() == "tgeiu $s1, 0x4d2");
    CHECK(MIPS32Instr(0x0629fb2e).get_disasm() == "tgeiu $s1, 0xfb2e");

    MIPS32Instr instr( "tgeiu");
    instr.set_v_src( 0, 0);
    instr.set_v_imm( 4);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}

TEST_CASE( "MIPS32_instr: tgeiu test_eq")
{
    MIPS32Instr instr( "tgeiu");
    instr.set_v_src( 0, 0);
    instr.set_v_imm( 0);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}

TEST_CASE( "MIPS32_instr: tgeiu test_gu")
{
    MIPS32Instr instr( "tgeiu");
    instr.set_v_src( 0xfffffffc, 0);
    instr.set_v_imm( 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: tgeu test_lu")
{
    CHECK(MIPS32Instr(0x02290031).get_disasm() == "tgeu $s1, $t1");

    MIPS32Instr instr( "tgeu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xfffffffa, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}

TEST_CASE( "MIPS32_instr: tgeu test_eq")
{
    MIPS32Instr instr( "tgeu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}

TEST_CASE( "MIPS32_instr: tgeu test_gu")
{
    MIPS32Instr instr( "tgeu");
    instr.set_v_src( 0xfffffffa, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: tlt test_l")
{
    CHECK(MIPS32Instr(0x02290032).get_disasm() == "tlt $s1, $t1");

    MIPS32Instr instr( "tlt");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 2, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}

TEST_CASE( "MIPS32_instr: tlt test_eq")
{
    MIPS32Instr instr( "tlt");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}

TEST_CASE( "MIPS32_instr: tlt test_g")
{
    MIPS32Instr instr( "tlt");
    instr.set_v_src( 2, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: tlti test_l")
{
    CHECK(MIPS32Instr(0x062a04d2).get_disasm() == "tlti $s1, 0x4d2");
    CHECK(MIPS32Instr(0x062afb2e).get_disasm() == "tlti $s1, 0xfb2e");

    MIPS32Instr instr( "tlti");
    instr.set_v_src( 0, 0);
    instr.set_v_imm( 4);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}

TEST_CASE( "MIPS32_instr: tlti test_eq")
{
    MIPS32Instr instr( "tlti");
    instr.set_v_src( 0, 0);
    instr.set_v_imm( 0);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}

TEST_CASE( "MIPS32_instr: tlti test_g")
{
    MIPS32Instr instr( "tlti");
    instr.set_v_src( 8, 0);
    instr.set_v_imm( 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: tltiu test_l")
{
    CHECK(MIPS32Instr(0x062b04d2).get_disasm() == "tltiu $s1, 0x4d2");
    CHECK(MIPS32Instr(0x062bfb2e).get_disasm() == "tltiu $s1, 0xfb2e");

    MIPS32Instr instr( "tltiu");
    instr.set_v_src( 0, 0);
    instr.set_v_imm( 4);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}

TEST_CASE( "MIPS32_instr: tltiu test_eq")
{
    MIPS32Instr instr( "tltiu");
    instr.set_v_src( 0, 0);
    instr.set_v_imm( 0);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}

TEST_CASE( "MIPS32_instr: tltiu test_gu")
{
    MIPS32Instr instr( "tltiu");
    instr.set_v_src( 0xfffffffc, 0);
    instr.set_v_imm( 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: tltu test_lu")
{
    CHECK(MIPS32Instr(0x02290033).get_disasm() == "tltu $s1, $t1");

    MIPS32Instr instr( "tltu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xfffffffa, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}

TEST_CASE( "MIPS32_instr: tltu test_eq")
{
    MIPS32Instr instr( "tltu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}

TEST_CASE( "MIPS32_instr: tltu test_gu")
{
    MIPS32Instr instr( "tltu");
    instr.set_v_src( 0xfffffffa, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: tne test_ne")
{
    CHECK(MIPS32Instr(0x02290036).get_disasm() == "tne $s1, $t1");

    MIPS32Instr instr( "tne");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}

TEST_CASE( "MIPS32_instr: tne test_eq")
{
    MIPS32Instr instr( "tne");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: tnei test_ne")
{
    CHECK(MIPS32Instr(0x062e04d2).get_disasm() == "tnei $s1, 0x4d2");
    CHECK(MIPS32Instr(0x062efb2e).get_disasm() == "tnei $s1, 0xfb2e");

    MIPS32Instr instr( "tnei");
    instr.set_v_src( 0, 0);
    instr.set_v_imm( 4);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}

TEST_CASE( "MIPS32_instr: tnei test_eq")
{
    MIPS32Instr instr( "tnei");
    instr.set_v_src( 0, 0);
    instr.set_v_imm( 0);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: xor test_1")
{
    CHECK(MIPS32Instr(0x01398826).get_disasm() == "xor $s1, $t1, $t9");

    MIPS32Instr instr( "xor");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: xor test_2")
{
    MIPS32Instr instr( "xor");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: xor test_3")
{
    MIPS32Instr instr( "xor");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfffffffe);
}
////////////////////////////////////////////////////////////////////////////////
