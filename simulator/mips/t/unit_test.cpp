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
    MIPS32Instr instr( "invalid_instruction");
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
    CHECK(MIPS32Instr(0x71208821).get_disasm() == "clo $s1, $t1");
    CHECK(MIPS32Instr(0x71208825).get_disasm() == "dclo $s1, $t1");
    CHECK(MIPS32Instr(0x71208820).get_disasm() == "clz $s1, $t1");
    CHECK(MIPS32Instr(0x71208824).get_disasm() == "dclz $s1, $t1");
    CHECK(MIPS32Instr(0x0229001a).get_disasm() == "div $s1, $t1");
    CHECK(MIPS32Instr(0x0229001e).get_disasm() == "ddiv $s1, $t1");
    CHECK(MIPS32Instr(0x0229001b).get_disasm() == "divu $s1, $t1");
    CHECK(MIPS32Instr(0x0229001f).get_disasm() == "ddivu $s1, $t1");
    CHECK(MIPS32Instr(0x02290018).get_disasm() == "mult $s1, $t1");
    CHECK(MIPS32Instr(0x0229001c).get_disasm() == "dmult $s1, $t1");
    CHECK(MIPS32Instr(0x02290019).get_disasm() == "multu $s1, $t1");
    CHECK(MIPS32Instr(0x0229001d).get_disasm() == "dmultu $s1, $t1");
    CHECK(MIPS32Instr(0x72290000).get_disasm() == "madd $s1, $t1");
    CHECK(MIPS32Instr(0x72290001).get_disasm() == "maddu $s1, $t1");
    CHECK(MIPS32Instr(0x72290004).get_disasm() == "msub $s1, $t1");
    CHECK(MIPS32Instr(0x72290005).get_disasm() == "msubu $s1, $t1");
    CHECK(MIPS32Instr(0x01398827).get_disasm() == "nor $s1, $t1, $t9");
    CHECK(MIPS32Instr(0x01398825).get_disasm() == "or $s1, $t1, $t9");
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
    CHECK(MIPS32Instr(0x01398826).get_disasm() == "xor $s1, $t1, $t9");
    CHECK(MIPS32Instr(0x0139882a).get_disasm() == "slt $s1, $t1, $t9");
    CHECK(MIPS32Instr(0x0139882b).get_disasm() == "sltu $s1, $t1, $t9");
    CHECK(MIPS32Instr(0x02290030).get_disasm() == "tge $s1, $t1");
    CHECK(MIPS32Instr(0x02290031).get_disasm() == "tgeu $s1, $t1");
    CHECK(MIPS32Instr(0x02290032).get_disasm() == "tlt $s1, $t1");
    CHECK(MIPS32Instr(0x02290033).get_disasm() == "tltu $s1, $t1");
    CHECK(MIPS32Instr(0x02290034).get_disasm() == "teq $s1, $t1");
    CHECK(MIPS32Instr(0x02290036).get_disasm() == "tne $s1, $t1");
}

TEST_CASE( "MIPS32_instr_disasm: Process_Disasm_IJ")
{
    CHECK(MIPS32Instr(0x613104d2).get_disasm() == "daddi $s1, $t1, 0x4d2");
    CHECK(MIPS32Instr(0x6131fb2e).get_disasm() == "daddi $s1, $t1, 0xfb2e");
    CHECK(MIPS32Instr(0x653104d2).get_disasm() == "daddiu $s1, $t1, 0x4d2");
    CHECK(MIPS32Instr(0x6531fb2e).get_disasm() == "daddiu $s1, $t1, 0xfb2e");
    CHECK(MIPS32Instr(0x293104d2).get_disasm() == "slti $s1, $t1, 0x4d2");
    CHECK(MIPS32Instr(0x2931fb2e).get_disasm() == "slti $s1, $t1, 0xfb2e");
    CHECK(MIPS32Instr(0x2d3104d2).get_disasm() == "sltiu $s1, $t1, 0x4d2");
    CHECK(MIPS32Instr(0x2d31fb2e).get_disasm() == "sltiu $s1, $t1, 0xfb2e");
    CHECK(MIPS32Instr(0x353104d2).get_disasm() == "ori $s1, $t1, 0x4d2");
    CHECK(MIPS32Instr(0x393104d2).get_disasm() == "xori $s1, $t1, 0x4d2");
    CHECK(MIPS32Instr(0x3c1104d2).get_disasm() == "lui $s1, 0x4d2");
    CHECK(MIPS32Instr(0x062e04d2).get_disasm() == "tnei $s1, 0x4d2");
    CHECK(MIPS32Instr(0x062efb2e).get_disasm() == "tnei $s1, 0xfb2e");
    CHECK(MIPS32Instr(0x062804d2).get_disasm() == "tgei $s1, 0x4d2");
    CHECK(MIPS32Instr(0x0628fb2e).get_disasm() == "tgei $s1, 0xfb2e");
    CHECK(MIPS32Instr(0x062904d2).get_disasm() == "tgeiu $s1, 0x4d2");
    CHECK(MIPS32Instr(0x0629fb2e).get_disasm() == "tgeiu $s1, 0xfb2e");
    CHECK(MIPS32Instr(0x062a04d2).get_disasm() == "tlti $s1, 0x4d2");
    CHECK(MIPS32Instr(0x062afb2e).get_disasm() == "tlti $s1, 0xfb2e");
    CHECK(MIPS32Instr(0x062b04d2).get_disasm() == "tltiu $s1, 0x4d2");
    CHECK(MIPS32Instr(0x062bfb2e).get_disasm() == "tltiu $s1, 0xfb2e");
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
    CHECK(MIPS32Instr(0x1629fff1).get_disasm() == "bne $s1, $t1, -15");
    CHECK(MIPS32Instr(0x16290000).get_disasm() == "bne $s1, $t1, 0");
    CHECK(MIPS32Instr(0x0622fff3).get_disasm() == "bltzl $s1, -13");
    CHECK(MIPS32Instr(0x06220003).get_disasm() == "bltzl $s1, 3");
    CHECK(MIPS32Instr(0x0623fffd).get_disasm() == "bgezl $s1, -3");
    CHECK(MIPS32Instr(0x0623000d).get_disasm() == "bgezl $s1, 13");
    CHECK(MIPS32Instr(0x0632fff6).get_disasm() == "bltzall $s1, -10");
    CHECK(MIPS32Instr(0x06320003).get_disasm() == "bltzall $s1, 3");
    CHECK(MIPS32Instr(0x0633fffb).get_disasm() == "bgezall $s1, -5");
    CHECK(MIPS32Instr(0x0633000a).get_disasm() == "bgezall $s1, 10");
    CHECK(MIPS32Instr(0x0bfffb2e).get_disasm() == "j 0x3fffb2e");
    CHECK(MIPS32Instr(0x080004d2).get_disasm() == "j 0x4d2");
    CHECK(MIPS32Instr(0x0ffffb2e).get_disasm() == "jal 0x3fffb2e");
    CHECK(MIPS32Instr(0x0c0004d2).get_disasm() == "jal 0x4d2");
}


// ********* Converted SPIM TT tests **********
// 27 done

TEST_CASE ( "MIPS32_instr: add")
{
    CHECK(MIPS32Instr(0x01398820).get_disasm() == "add $s1, $t1, $t9");
    MIPS32Instr instr( "add");

    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);

    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);

    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);

/*     Overflow exception isn't implemented
    instr.set_v_dst( 0xfee1dead);
    instr.set_v_src( 0x7fffffff, 0);
    instr.set_v_src( 0x7fffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfee1dead);
    CHECK( instr.trap_type() != Trap::NO_TRAP);
*/
}

TEST_CASE( "MIPS32_instr: addi")
{
    CHECK(MIPS32Instr(0x213104d2).get_disasm() == "addi $s1, $t1, 0x4d2");
    CHECK(MIPS32Instr(0x2131fb2e).get_disasm() == "addi $s1, $t1, 0xfb2e");
    MIPS32Instr instr( "addi");

    instr.set_v_src( 0, 0);
    instr.set_v_imm( 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);

    instr.set_v_src( 0, 0);
    instr.set_v_imm( 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);

    instr.set_v_src( 1, 0);
    instr.set_v_imm( 0xffff);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
    
/*      Overflow exception isn't implemented
    instr.set_v_dst( 0xfee1dead);
    instr.set_v_src( 0x7fffffff, 0);
    instr.set_v_imm( 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfee1dead);
    CHECK( instr.trap_type() != Trap::NO_TRAP);
*/
}

TEST_CASE( "MIPS32_instr: addiu")
{
    CHECK(MIPS32Instr(0x253104d2).get_disasm() == "addiu $s1, $t1, 0x4d2");
    CHECK(MIPS32Instr(0x2531fb2e).get_disasm() == "addiu $s1, $t1, 0xfb2e");
    MIPS32Instr instr( "addiu");

    instr.set_v_src( 0, 0);
    instr.set_v_imm( 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);

    instr.set_v_src( 0, 0);
    instr.set_v_imm( 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);

    instr.set_v_src( 1, 0);
    instr.set_v_imm( 0xffff);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
    
    instr.set_v_src( 0x7fffffff, 0);
    instr.set_v_imm( 0x1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x80000000);
    CHECK( instr.has_trap() == false);
}

TEST_CASE ( "MIPS32_instr: addu")
{
    CHECK(MIPS32Instr(0x01398821).get_disasm() == "addu $s1, $t1, $t9");
    MIPS32Instr instr( "addu");

    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);

    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);

    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);

    instr.set_v_src( 0x7fffffff, 0);
    instr.set_v_src( 0x7fffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfffffffe);
    CHECK( instr.has_trap() == false);
}

TEST_CASE ( "MIPS32_instr: and")
{
    CHECK(MIPS32Instr(0x01398824).get_disasm() == "and $s1, $t1, $t9");
    MIPS32Instr instr( "and");

    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);

    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);

    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE ( "MIPS32_instr: andi")
{
    CHECK(MIPS32Instr(0x313104d2).get_disasm() == "andi $s1, $t1, 0x4d2");
    MIPS32Instr instr( "andi");

    instr.set_v_src( 0, 0);
    instr.set_v_imm( 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);

    instr.set_v_src( 1, 0);
    instr.set_v_imm( 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);

    instr.set_v_src( 1, 0);
    instr.set_v_imm( 0xffff);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);

    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_imm( 0xffff);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x0000ffff);
}

TEST_CASE( "MIPS32_instr: beq")
{
    CHECK(MIPS32Instr(0x1229ffff).get_disasm() == "beq $s1, $t1, -1");
    CHECK(MIPS32Instr(0x1229000e).get_disasm() == "beq $s1, $t1, 14");
    MIPS32Instr instr( "beq");

    instr.set_new_PC( instr.get_PC() + 4);
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.set_v_imm( 1);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);

    instr.set_new_PC( instr.get_PC() + 4);
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.set_v_imm( 1);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);

    instr.set_new_PC( instr.get_PC() + 4);
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4);

    instr.set_new_PC( instr.get_PC() + 4);
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.set_v_imm( 1024);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 + 1024 * 4);

    instr.set_new_PC( instr.get_PC() + 4);
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.set_v_imm( 0xffffffff - 1024 + 1);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 - 1024 * 4);
}

TEST_CASE( "MIPS32_instr: break")
{
    CHECK(MIPS32Instr(0x0000000d).get_disasm() == "break");
    MIPS32Instr instr( "break");
    instr.execute();
    CHECK( instr.trap_type() == Trap::BREAKPOINT);
}

TEST_CASE( "MIPS64_instr: dsllv")
{
    CHECK(MIPS32Instr(0x03298814).get_disasm() == "dsllv $s1, $t1, $t9");
    MIPS64Instr instr( "dsllv");

    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);

    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 2);

    instr.set_v_src( 1, 0);
    instr.set_v_src( 32, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x100000000);

    instr.set_v_src( 1, 0);
    instr.set_v_src( 64, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS64_instr: dsrav")
{
    CHECK(MIPS32Instr(0x03298817).get_disasm() == "dsrav $s1, $t1, $t9");
    MIPS64Instr instr( "dsrav");

    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);

    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);

    instr.set_v_src( 0x100000000000, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x010000000000);

    instr.set_v_src( 0xff00000000000000, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfff0000000000000);
}

TEST_CASE( "MIPS64_instr: dsrlv")
{
    CHECK(MIPS32Instr(0x03298816).get_disasm() == "dsrlv $s1, $t1, $t9");
    MIPS64Instr instr( "dsrlv");

    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);

    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);

    instr.set_v_src( 0x10000000, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x01000000);

    instr.set_v_src( 0x8000000000000000, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x0800000000000000);
}

TEST_CASE( "MIPS32_instr: jalr")
{
    CHECK(MIPS32Instr(0x01208809).get_disasm() == "jalr $s1, $t1");
    MIPS32Instr instr( "jalr");

    instr.set_new_PC( instr.get_PC() + 4);
    instr.set_v_src( instr.get_PC() + 8, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
    CHECK( instr.get_v_dst() == instr.get_PC() + 4);

    instr.set_new_PC( instr.get_PC() + 4);
    instr.set_v_src( instr.get_PC() + 2, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4);
}

TEST_CASE( "MIPS32_instr: jr")
{
    CHECK(MIPS32Instr(0x02200008).get_disasm() == "jr $s1");
    MIPS32Instr instr( "jr");

    instr.set_new_PC( instr.get_PC() + 4);
    instr.set_v_src( 1024, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == 1024);

    instr.set_new_PC( instr.get_PC() + 4);
    instr.set_v_src( instr.get_PC() + 2, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4);
}

TEST_CASE( "MIPS32_instr: mfhi")
{
    CHECK(MIPS32Instr(0x00008810).get_disasm() == "mfhi $s1");
    MIPS32Instr instr( "mfhi");
    instr.set_v_dst( 0);
    instr.set_v_src( 0xfee1dead, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfee1dead);
}

TEST_CASE( "MIPS32_instr: mflo")
{
    CHECK(MIPS32Instr(0x00008812).get_disasm() == "mflo $s1");
    MIPS32Instr instr( "mflo");
    instr.set_v_dst( 0);
    instr.set_v_src( 0xfee1dead, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfee1dead);
}

/*      Not implemented on the register file level
TEST_CASE( "MIPS32_instr: movn")
{
    CHECK(MIPS32Instr(0x0139880b).get_disasm() == "movn $s1, $t1, $t9");
    MIPS32Instr instr( "movn");

    instr.set_v_src( 3, 0);
    instr.set_v_src( 0, 1);
    instr.set_v_dst( 4);
    instr.execute();
    CHECK( instr.get_v_dst() == 4);

    instr.set_v_src( 3, 0);
    instr.set_v_src( 2, 1);
    instr.set_v_dst( 4);
    instr.execute();
    CHECK( instr.get_v_dst() == 3);
} */

/*      Not implemented on the register file level
TEST_CASE( "MIPS32_instr: movz")
{
    CHECK(MIPS32Instr(0x0139880a).get_disasm() == "movz $s1, $t1, $t9");
    MIPS32Instr instr( "movz");

    instr.set_v_src( 3, 0);
    instr.set_v_src( 2, 1);
    instr.set_v_dst( 4);
    instr.execute();
    CHECK( instr.get_v_dst() == 4);

    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.set_v_dst( 4);
    instr.execute();
    CHECK( instr.get_v_dst() == 4);
    
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.set_v_dst( 4);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);

    instr.set_v_src( 3, 0);
    instr.set_v_src( 0, 1);
    instr.set_v_dst( 4);
    instr.execute();
    CHECK( instr.get_v_dst() == 3);
} */

TEST_CASE( "MIPS32_instr: mthi")
{
    CHECK(MIPS32Instr(0x02200011).get_disasm() == "mthi $s1");
    MIPS32Instr instr( "mthi");
    instr.set_v_dst( 0);
    instr.set_v_src( 0xfee1dead, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfee1dead);
}

TEST_CASE( "MIPS32_instr: mtlo")
{
    CHECK(MIPS32Instr(0x02200013).get_disasm() == "mtlo $s1");
    MIPS32Instr instr( "mtlo");
    instr.set_v_dst( 0);
    instr.set_v_src( 0xfee1dead, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfee1dead);
}

TEST_CASE( "MIPS32_instr: nop")
{
    CHECK(MIPS32Instr(0x00000000).get_disasm() == "nop");
    CHECK(MIPS32Instr     ("nop").get_disasm() == "nop");
    MIPS32Instr instr( "nop");
    instr.execute();
}

TEST_CASE ( "MIPS32_instr: sll")
{
    CHECK(MIPS32Instr(0x00098cc0).get_disasm() == "sll $s1, $t1, 19");
    MIPS32Instr instr( "sll");

    instr.set_v_src( 1, 0);
    instr.set_shamt( 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);

    instr.set_v_src( 1, 0);
    instr.set_shamt( 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 2);

    instr.set_v_src( 0x100, 0);
    instr.set_shamt( 4);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x1000);

    instr.set_v_src( 0x08000000, 0);
    instr.set_shamt( 4);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x80000000);
}

TEST_CASE( "MIPS32_instr: sllv")
{
    CHECK(MIPS32Instr(0x03298804).get_disasm() == "sllv $s1, $t1, $t9");
    MIPS32Instr instr( "sllv");

    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);

    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 2);

    instr.set_v_src( 1, 0);
    instr.set_v_src( 16, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x10000);

    instr.set_v_src( 1, 0);
    instr.set_v_src( 32, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: sra")
{
    CHECK(MIPS32Instr(0x00098cc3).get_disasm() == "sra $s1, $t1, 19");
    MIPS32Instr instr( "sra");

    instr.set_v_src( 1, 0);
    instr.set_shamt( 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);

    instr.set_v_src( 1, 0);
    instr.set_shamt( 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);

    instr.set_v_src( 0x1000, 0);
    instr.set_shamt( 4);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x100);

    instr.set_v_src( 0xff000000, 0);
    instr.set_shamt( 8);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffff0000);
}

TEST_CASE( "MIPS32_instr: srav")
{
    CHECK(MIPS32Instr(0x03298807).get_disasm() == "srav $s1, $t1, $t9");
    MIPS32Instr instr( "srav");

    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);

    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);

    instr.set_v_src( 0x1000, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x100);

    instr.set_v_src( 0xff000000, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfff00000);
}

TEST_CASE ( "MIPS32_instr: srl")
{
    CHECK(MIPS32Instr(0x00098cc2).get_disasm() == "srl $s1, $t1, 19");
    MIPS32Instr instr( "srl");

    instr.set_v_src( 1, 0);
    instr.set_shamt( 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);

    instr.set_v_src( 1, 0);
    instr.set_shamt( 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);

    instr.set_v_src( 0x1000, 0);
    instr.set_shamt( 4);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x100);

    instr.set_v_src( 0x80000000, 0);
    instr.set_shamt( 4);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x08000000);
}

TEST_CASE( "MIPS32_instr: srlv")
{
    CHECK(MIPS32Instr(0x03298806).get_disasm() == "srlv $s1, $t1, $t9");
    MIPS32Instr instr( "srlv");

    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);

    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);

    instr.set_v_src( 0x1000, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x100);

    instr.set_v_src( 0x80000000, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x08000000);
}

TEST_CASE( "MIPS32_instr: syscall")
{
    CHECK(MIPS32Instr(0x0000000c).get_disasm() == "syscall");
    MIPS32Instr instr( "syscall");
    instr.execute();
    CHECK( instr.trap_type() == Trap::SYSCALL);
}


// Outdated
TEST_CASE( "MIPS32_instr: mul")
{
    CHECK(MIPS32Instr(0x71398802).get_disasm() == "mul $s1, $t1, $t9");
    MIPS32Instr instr( "mul");
    instr.set_v_src( 10, 0);
    instr.set_v_src( 20, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 200);
}

// Outdated
TEST_CASE( "MIPS32_instr: teqi")
{
    CHECK(MIPS32Instr(0x062c04d2).get_disasm() == "teqi $s1, 0x4d2");
    CHECK(MIPS32Instr(0x062cfb2e).get_disasm() == "teqi $s1, 0xfb2e");
    MIPS32Instr instr( "teqi");
    instr.set_v_src( 0, 0);
    instr.set_v_imm( 0);
    instr.execute();
    CHECK( instr.has_trap());
}