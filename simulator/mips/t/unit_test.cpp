// generic C
#include <cassert>
#include <cstdlib>

// Google Test library
#include <gtest/gtest.h>

// uArchSim modules
#include "../mips_instr.h"

//
// Check that all incorect input params of the constructor
// are properly handled.
//
TEST( Func_instr_init, Process_Wrong_Args_Of_Constr)
{
    ASSERT_NO_THROW( FuncInstr(0x0).execute());

    // must exit and return EXIT_FAILURE
    ASSERT_EXIT( FuncInstr(0xFFFFFFFFull).execute(),
                 ::testing::ExitedWithCode( EXIT_FAILURE), "ERROR.*");
}

/*
 * Tests by Orvar Segerstrom
 * Copyright 2015 GNU GPL 3.0
 * https://github.com/awestroke/mips-dasm/blob/master/instructions_test.c
 */

#define TEST_BAD_OPCODE( opcode) \
    ASSERT_EQ(FuncInstr( opcode).Dump(), std::string(#opcode) + "\tUnknown");

TEST( Func_instr_disasm, Process_Unknown_Instruction)
{
    TEST_BAD_OPCODE(0x71208821)
    TEST_BAD_OPCODE(0x71208820)
    TEST_BAD_OPCODE(0x71398802)
    TEST_BAD_OPCODE(0x72290000)
    TEST_BAD_OPCODE(0x72290001)
    TEST_BAD_OPCODE(0x72290004)
    TEST_BAD_OPCODE(0x062c04d2)
    TEST_BAD_OPCODE(0x062cfb2e)
    TEST_BAD_OPCODE(0x062e04d2)
    TEST_BAD_OPCODE(0x062efb2e)
    TEST_BAD_OPCODE(0x062804d2)
    TEST_BAD_OPCODE(0x0628fb2e)
    TEST_BAD_OPCODE(0x062904d2)
    TEST_BAD_OPCODE(0x0629fb2e)
    TEST_BAD_OPCODE(0x062a04d2)
    TEST_BAD_OPCODE(0x062afb2e)
    TEST_BAD_OPCODE(0x062b04d2)
    TEST_BAD_OPCODE(0x062bfb2e)
    TEST_BAD_OPCODE(0x72290005)
    TEST_BAD_OPCODE(0x893104d2)
    TEST_BAD_OPCODE(0x8931fb2e)
    TEST_BAD_OPCODE(0x993104d2)
    TEST_BAD_OPCODE(0x9931fb2e)
    TEST_BAD_OPCODE(0xc13104d2)
    TEST_BAD_OPCODE(0xc131fb2e)
    TEST_BAD_OPCODE(0x0630fff5)
    TEST_BAD_OPCODE(0x06300004)
    TEST_BAD_OPCODE(0x0620fff3)
    TEST_BAD_OPCODE(0x06200002)
    TEST_BAD_OPCODE(0x0621fffd)
    TEST_BAD_OPCODE(0x0621000c)
    TEST_BAD_OPCODE(0x0631fffb)
    TEST_BAD_OPCODE(0x0631000a)
}

TEST( Func_instr_disasm, Process_Disasm)
{
    ASSERT_EQ(FuncInstr(0x01398820).Dump(), "add $s1, $t1, $t9");
    ASSERT_EQ(FuncInstr(0x01398821).Dump(), "addu $s1, $t1, $t9");
    ASSERT_EQ(FuncInstr(0x01398824).Dump(), "and $s1, $t1, $t9");
//  ASSERT_EQ(FuncInstr(0x71208821).Dump(), "clo $s1, $t1");
//  ASSERT_EQ(FuncInstr(0x71208820).Dump(), "clz $s1, $t1");
    ASSERT_EQ(FuncInstr(0x0229001a).Dump(), "div $zero, $s1, $t1");
    ASSERT_EQ(FuncInstr(0x0229001b).Dump(), "divu $zero, $s1, $t1");
    ASSERT_EQ(FuncInstr(0x02290018).Dump(), "mult $zero, $s1, $t1");
    ASSERT_EQ(FuncInstr(0x02290019).Dump(), "multu $zero, $s1, $t1");
//  ASSERT_EQ(FuncInstr(0x71398802).Dump(), "mul $s1, $t1, $t9");
//  ASSERT_EQ(FuncInstr(0x72290000).Dump(), "madd $s1, $t1");
//  ASSERT_EQ(FuncInstr(0x72290001).Dump(), "maddu $s1, $t1");
//  ASSERT_EQ(FuncInstr(0x72290004).Dump(), "msub $s1, $t1");
//  ASSERT_EQ(FuncInstr(0x72290005).Dump(), "msubu $s1, $t1");
    ASSERT_EQ(FuncInstr(0x01398827).Dump(), "nor $s1, $t1, $t9");
    ASSERT_EQ(FuncInstr(0x01398825).Dump(), "or $s1, $t1, $t9");
    ASSERT_EQ(FuncInstr(0x00098cc0).Dump(), "sll $s1, $t1, 19");
    ASSERT_EQ(FuncInstr(0x03298804).Dump(), "sllv $s1, $t1, $t9");
    ASSERT_EQ(FuncInstr(0x00098cc3).Dump(), "sra $s1, $t1, 19");
    ASSERT_EQ(FuncInstr(0x03298807).Dump(), "srav $s1, $t1, $t9");
    ASSERT_EQ(FuncInstr(0x00098cc2).Dump(), "srl $s1, $t1, 19");
    ASSERT_EQ(FuncInstr(0x03298806).Dump(), "srlv $s1, $t1, $t9");
    ASSERT_EQ(FuncInstr(0x01398822).Dump(), "sub $s1, $t1, $t9");
    ASSERT_EQ(FuncInstr(0x01398823).Dump(), "subu $s1, $t1, $t9");
    ASSERT_EQ(FuncInstr(0x01398826).Dump(), "xor $s1, $t1, $t9");
    ASSERT_EQ(FuncInstr(0x0139882a).Dump(), "slt $s1, $t1, $t9");
    ASSERT_EQ(FuncInstr(0x0139882b).Dump(), "sltu $s1, $t1, $t9");
    ASSERT_EQ(FuncInstr(0x01208809).Dump(), "jalr $s1, $t1");
    ASSERT_EQ(FuncInstr(0x00000000).Dump(), "nop ");
    
    ASSERT_EQ(FuncInstr(0x213104d2).Dump(), "addi $s1, $t1, 0x4d2");
    ASSERT_EQ(FuncInstr(0x2131fb2e).Dump(), "addi $s1, $t1, 0xfb2e");
    ASSERT_EQ(FuncInstr(0x253104d2).Dump(), "addiu $s1, $t1, 0x4d2");
    ASSERT_EQ(FuncInstr(0x2531fb2e).Dump(), "addiu $s1, $t1, 0xfb2e");
    ASSERT_EQ(FuncInstr(0x293104d2).Dump(), "slti $s1, $t1, 0x4d2");
    ASSERT_EQ(FuncInstr(0x2931fb2e).Dump(), "slti $s1, $t1, 0xfb2e");
    ASSERT_EQ(FuncInstr(0x2d3104d2).Dump(), "sltiu $s1, $t1, 0x4d2");
    ASSERT_EQ(FuncInstr(0x2d31fb2e).Dump(), "sltiu $s1, $t1, 0xfb2e");
    ASSERT_EQ(FuncInstr(0x313104d2).Dump(), "andi $s1, $t1, 0x4d2");
    ASSERT_EQ(FuncInstr(0x353104d2).Dump(), "ori $s1, $t1, 0x4d2");
    ASSERT_EQ(FuncInstr(0x393104d2).Dump(), "xori $s1, $t1, 0x4d2");
    ASSERT_EQ(FuncInstr(0x3c1104d2).Dump(), "lui $s1, 0x4d2");
//  ASSERT_EQ(FuncInstr(0x062c04d2).Dump(), "teqi $s1, 0x4d2");
//  ASSERT_EQ(FuncInstr(0x062cfb2e).Dump(), "teqi $s1, 0xfb2e");
//  ASSERT_EQ(FuncInstr(0x062e04d2).Dump(), "tnei $s1, 0x4d2");
//  ASSERT_EQ(FuncInstr(0x062efb2e).Dump(), "tnei $s1, 0xfb2e");
//  ASSERT_EQ(FuncInstr(0x062804d2).Dump(), "tgei $s1, 0x4d2");
//  ASSERT_EQ(FuncInstr(0x0628fb2e).Dump(), "tgei $s1, 0xfb2e");
//  ASSERT_EQ(FuncInstr(0x062904d2).Dump(), "tgeiu $s1, 0x4d2");
//  ASSERT_EQ(FuncInstr(0x0629fb2e).Dump(), "tgeiu $s1, 0xfb2e");
//  ASSERT_EQ(FuncInstr(0x062a04d2).Dump(), "tlti $s1, 0x4d2");
//  ASSERT_EQ(FuncInstr(0x062afb2e).Dump(), "tlti $s1, 0xfb2e");
//  ASSERT_EQ(FuncInstr(0x062b04d2).Dump(), "tltiu $s1, 0x4d2");
//  ASSERT_EQ(FuncInstr(0x062bfb2e).Dump(), "tltiu $s1, 0xfb2e");
    ASSERT_EQ(FuncInstr(0x813104d2).Dump(), "lb $s1, 0x4d2($t1)");
    ASSERT_EQ(FuncInstr(0x8131fb2e).Dump(), "lb $s1, 0xfb2e($t1)");
    ASSERT_EQ(FuncInstr(0x913104d2).Dump(), "lbu $s1, 0x4d2($t1)");
    ASSERT_EQ(FuncInstr(0x9131fb2e).Dump(), "lbu $s1, 0xfb2e($t1)");
    ASSERT_EQ(FuncInstr(0x853104d2).Dump(), "lh $s1, 0x4d2($t1)");
    ASSERT_EQ(FuncInstr(0x8531fb2e).Dump(), "lh $s1, 0xfb2e($t1)");
    ASSERT_EQ(FuncInstr(0x953104d2).Dump(), "lhu $s1, 0x4d2($t1)");
    ASSERT_EQ(FuncInstr(0x9531fb2e).Dump(), "lhu $s1, 0xfb2e($t1)");
    ASSERT_EQ(FuncInstr(0x8d3104d2).Dump(), "lw $s1, 0x4d2($t1)");
    ASSERT_EQ(FuncInstr(0x8d31fb2e).Dump(), "lw $s1, 0xfb2e($t1)");
//  ASSERT_EQ(FuncInstr(0x893104d2).Dump(), "lwl $s1, 0x4d2($t1)");
//  ASSERT_EQ(FuncInstr(0x8931fb2e).Dump(), "lwl $s1, 0xfb2e($t1)");
//  ASSERT_EQ(FuncInstr(0x993104d2).Dump(), "lwr $s1, 0x4d2($t1)");
//  ASSERT_EQ(FuncInstr(0x9931fb2e).Dump(), "lwr $s1, 0xfb2e($t1)");
//  ASSERT_EQ(FuncInstr(0xc13104d2).Dump(), "ll $s1, 0x4d2($t1)");
//  ASSERT_EQ(FuncInstr(0xc131fb2e).Dump(), "ll $s1, 0xfb2e($t1)");
    ASSERT_EQ(FuncInstr(0xa13104d2).Dump(), "sb $s1, 0x4d2($t1)");
    ASSERT_EQ(FuncInstr(0xa131fb2e).Dump(), "sb $s1, 0xfb2e($t1)");
    ASSERT_EQ(FuncInstr(0xa53104d2).Dump(), "sh $s1, 0x4d2($t1)");
    ASSERT_EQ(FuncInstr(0xa531fb2e).Dump(), "sh $s1, 0xfb2e($t1)");
    ASSERT_EQ(FuncInstr(0xad3104d2).Dump(), "sw $s1, 0x4d2($t1)");
    ASSERT_EQ(FuncInstr(0xad31fb2e).Dump(), "sw $s1, 0xfb2e($t1)");
//  ASSERT_EQ(FuncInstr(0xa93104d2).Dump(), "swl $s1, 0x4d2($t1)");
//  ASSERT_EQ(FuncInstr(0xa931fb2e).Dump(), "swl $s1, 0xfb2e($t1)");
//  ASSERT_EQ(FuncInstr(0xb93104d2).Dump(), "swr $s1, 0x4d2($t1)");
//  ASSERT_EQ(FuncInstr(0xb931fb2e).Dump(), "swr $s1, 0xfb2e($t1)");
//  ASSERT_EQ(FuncInstr(0xe13104d2).Dump(), "sc $s1, 0x4d2($t1)");
//  ASSERT_EQ(FuncInstr(0xe131fb2e).Dump(), "sc $s1, 0xfb2e($t1)");
    ASSERT_EQ(FuncInstr(0x1229ffff).Dump(), "beq $s1, $t1, -1");
    ASSERT_EQ(FuncInstr(0x1229000e).Dump(), "beq $s1, $t1, 14");
//  ASSERT_EQ(FuncInstr(0x0621fffd).Dump(), "bgez $s1, 0xfffc");
//  ASSERT_EQ(FuncInstr(0x0621000c).Dump(), "bgez $s1, 0xc");
//  ASSERT_EQ(FuncInstr(0x0631fffb).Dump(), "bgezal $s1, 0xfffa");
//  ASSERT_EQ(FuncInstr(0x0631000a).Dump(), "bgezal $s1, 0xa");
    ASSERT_EQ(FuncInstr(0x1e20fff9).Dump(), "bgtz $s1, -7");
    ASSERT_EQ(FuncInstr(0x1e200008).Dump(), "bgtz $s1, 8");
    ASSERT_EQ(FuncInstr(0x1a20fff7).Dump(), "blez $s1, -9");
    ASSERT_EQ(FuncInstr(0x1a200006).Dump(), "blez $s1, 6");
//  ASSERT_EQ(FuncInstr(0x0630fff5).Dump(), "bltzal $s1, -11");
//  ASSERT_EQ(FuncInstr(0x06300004).Dump(), "bltzal $s1, 4");
//  ASSERT_EQ(FuncInstr(0x0620fff3).Dump(), "bltz $s1, -13");
//  ASSERT_EQ(FuncInstr(0x06200002).Dump(), "bltz $s1, 2");
    ASSERT_EQ(FuncInstr(0x1629fff1).Dump(), "bne $s1, $t1, -15");
    ASSERT_EQ(FuncInstr(0x16290000).Dump(), "bne $s1, $t1, 0");
    
    ASSERT_EQ(FuncInstr(0x0bfffb2e).Dump(), "j 0xfb2e");
    ASSERT_EQ(FuncInstr(0x080004d2).Dump(), "j 0x4d2");
    ASSERT_EQ(FuncInstr(0x0ffffb2e).Dump(), "jal 0xfb2e");
    ASSERT_EQ(FuncInstr(0x0c0004d2).Dump(), "jal 0x4d2");
}

int main( int argc, char* argv[])
{
    ::testing::InitGoogleTest( &argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}
