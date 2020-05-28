/* MIPS Instruction unit tests
 * @author: Pavel Kryukov, Vsevolod Pukhov, Egor Bova
 * Copyright (C) MIPT-MIPS 2017-2019
 */

#include "mips32_test.h"

#include <catch.hpp>

////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: bgez 0, 1 instruction ahead")
{
    CHECK(MIPS32Instr(0x0621fffc).get_disasm() == "bgez $s1, -4");
    CHECK(MIPS32Instr(0x0621000c).get_disasm() == "bgez $s1, 12");

    MIPS32Instr instr( "bgez", 1);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);

    CHECK( instr.is_common_branch() );
    CHECK( !instr.is_direct_jump() );
    CHECK( !instr.is_indirect_jump() );
    CHECK( !instr.is_likely_branch() );
}

TEST_CASE( "MIPS32_instr: bgez 1, 1 instruction ahead")
{
    MIPS32Instr instr( "bgez", 1);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bgez -1, 1 instruction ahead")
{
    MIPS32Instr instr( "bgez", 1);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bgez 1, 1024 instructions ahead")
{
    MIPS32Instr instr( "bgez", 1024);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 + 1024 * 4);
}

TEST_CASE( "MIPS32_instr: bgez 1, back to 1024 instruction")
{
    MIPS32Instr instr( "bgez", 0xffff - 1024 + 1);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 - 1024 * 4);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: bgezal 0, 1 instruction ahead")
{
    CHECK(MIPS32Instr(0x0631fffa).get_disasm() == "bgezal $s1, -6");
    CHECK(MIPS32Instr(0x0631000a).get_disasm() == "bgezal $s1, 10");

    MIPS32Instr instr( "bgezal", 1);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
    CHECK( instr.get_v_dst( 0)  == instr.get_PC() + 8);

    CHECK( instr.is_common_branch() );
    CHECK( !instr.is_direct_jump() );
    CHECK( !instr.is_indirect_jump() );
    CHECK( !instr.is_likely_branch() );
}

TEST_CASE( "MIPS32_instr: bgezal 1, 1 instruction ahead")
{
    MIPS32Instr instr( "bgezal", 1);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
    CHECK( instr.get_v_dst( 0)  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bgezal -1, 1 instruction ahead")
{
    MIPS32Instr instr( "bgezal", 1);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
    CHECK( instr.get_v_dst( 0)  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bgezal 1, 1024 instructions ahead")
{
    MIPS32Instr instr( "bgezal", 1024);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 + 1024 * 4);
    CHECK( instr.get_v_dst( 0)  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bgezal 1, back to 1024 instruction")
{
    MIPS32Instr instr( "bgezal", 0xffff - 1024 + 1);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 - 1024 * 4);
    CHECK( instr.get_v_dst( 0)  == instr.get_PC() + 8);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: bgezall 0, 1 instruction ahead")
{
    CHECK(MIPS32Instr(0x0633fffb).get_disasm() == "bgezall $s1, -5");
    CHECK(MIPS32Instr(0x0633000a).get_disasm() == "bgezall $s1, 10");

    MIPS32Instr instr( "bgezall", 1);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
    CHECK( instr.get_v_dst( 0)  == instr.get_PC() + 8);

    CHECK( !instr.is_common_branch() );
    CHECK( !instr.is_direct_jump() );
    CHECK( !instr.is_indirect_jump() );
    CHECK( instr.is_likely_branch() );
}

TEST_CASE( "MIPS32_instr: bgezall 1, 1 instruction ahead")
{
    MIPS32Instr instr( "bgezall", 1);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
    CHECK( instr.get_v_dst( 0)  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bgezall -1, 1 instruction ahead")
{
    MIPS32Instr instr( "bgezall", 1);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
    CHECK( instr.get_v_dst( 0)  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bgezall 1, 1024 instructions ahead")
{
    MIPS32Instr instr( "bgezall", 1024);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 + 1024 * 4);
    CHECK( instr.get_v_dst( 0)  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bgezall 1, back to 1024 instruction")
{
    MIPS32Instr instr( "bgezall", 0xffff - 1024 + 1);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 - 1024 * 4);
    CHECK( instr.get_v_dst( 0)  == instr.get_PC() + 8);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: bgezl 0, 1 instruction ahead")
{
    CHECK(MIPS32Instr(0x0623fffd).get_disasm() == "bgezl $s1, -3");
    CHECK(MIPS32Instr(0x0623000d).get_disasm() == "bgezl $s1, 13");

    MIPS32Instr instr( "bgezl", 1);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);

    CHECK( !instr.is_common_branch() );
    CHECK( !instr.is_direct_jump() );
    CHECK( !instr.is_indirect_jump() );
    CHECK( instr.is_likely_branch() );
}

TEST_CASE( "MIPS32_instr: bgezl 1, 1 instruction ahead")
{
    MIPS32Instr instr( "bgezl", 1);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bgezl -1, 1 instruction ahead")
{
    MIPS32Instr instr( "bgezl", 1);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bgezl 1, 1024 instructions ahead")
{
    MIPS32Instr instr( "bgezl", 1024);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 + 1024 * 4);
}

TEST_CASE( "MIPS32_instr: bgezl 1, back to 1024 instruction")
{
    MIPS32Instr instr( "bgezl", 0xffff - 1024 + 1);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 - 1024 * 4);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: bgtz 0, 1 instruction ahead")
{
    CHECK(MIPS32Instr(0x1e20fff9).get_disasm() == "bgtz $s1, -7");
    CHECK(MIPS32Instr(0x1e200008).get_disasm() == "bgtz $s1, 8");

    MIPS32Instr instr( "bgtz", 1);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);

    CHECK (instr.is_common_branch() );
    CHECK( !instr.is_direct_jump() );
    CHECK( !instr.is_indirect_jump() );
    CHECK( !instr.is_likely_branch() );
}

TEST_CASE( "MIPS32_instr: bgtz 1, 1 instruction ahead")
{
    MIPS32Instr instr( "bgtz", 1);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bgtz -1, 1 instruction ahead")
{
    MIPS32Instr instr( "bgtz", 1);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bgtz 1, 1024 instructions ahead")
{
    MIPS32Instr instr( "bgtz", 1024);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 + 1024 * 4);
}

TEST_CASE( "MIPS32_instr: bgtz 1, back to 1024 instruction")
{
    MIPS32Instr instr( "bgtz", 0xffff - 1024 + 1);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 - 1024 * 4);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: blez 0, 1 instruction ahead")
{
    CHECK(MIPS32Instr(0x1a20fff7).get_disasm() == "blez $s1, -9");
    CHECK(MIPS32Instr(0x1a200006).get_disasm() == "blez $s1, 6");

    MIPS32Instr instr( "blez", 1);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);

    CHECK( instr.is_common_branch() );
    CHECK( !instr.is_direct_jump() );
    CHECK( !instr.is_indirect_jump() );
    CHECK( !instr.is_likely_branch() );
}

TEST_CASE( "MIPS32_instr: blez 1, 1 instruction ahead")
{
    MIPS32Instr instr( "blez", 1);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: blez -1, 1 instruction ahead")
{
    MIPS32Instr instr( "blez", 1);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: blez -1, 1024 instructions ahead")
{
    MIPS32Instr instr( "blez", 1024);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 + 1024 * 4);
}

TEST_CASE( "MIPS32_instr: blez -1, back to 1024 instruction")
{
    MIPS32Instr instr( "blez", 0xffff - 1024 + 1);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 - 1024 * 4);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: bltz 0, 1 instruction ahead")
{
    CHECK(MIPS32Instr(0x0620fff3).get_disasm() == "bltz $s1, -13");
    CHECK(MIPS32Instr(0x06200002).get_disasm() == "bltz $s1, 2");

    MIPS32Instr instr( "bltz", 1);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);

    CHECK( instr.is_common_branch() );
    CHECK( !instr.is_direct_jump() );
    CHECK( !instr.is_indirect_jump() );
    CHECK( !instr.is_likely_branch() );
}

TEST_CASE( "MIPS32_instr: bltz 1, 1 instruction ahead")
{
    MIPS32Instr instr( "bltz", 1);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bltz -1, 1 instruction ahead")
{
    MIPS32Instr instr( "bltz", 1);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bltz -1, 1024 instructions ahead")
{
    MIPS32Instr instr( "bltz", 1024);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 + 1024 * 4);
}

TEST_CASE( "MIPS32_instr: bltz -1, back to 1024 instruction")
{
    MIPS32Instr instr( "bltz", 0xffff - 1024 + 1);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 - 1024 * 4);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: bltzal 0, 1 instruction ahead")
{
    CHECK(MIPS32Instr(0x0630fff5).get_disasm() == "bltzal $s1, -11");
    CHECK(MIPS32Instr(0x06300004).get_disasm() == "bltzal $s1, 4");

    MIPS32Instr instr( "bltzal", 1);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
    CHECK( instr.get_v_dst( 0)  == instr.get_PC() + 8);

    CHECK( instr.is_common_branch() );
    CHECK( !instr.is_direct_jump() );
    CHECK( !instr.is_indirect_jump() );
    CHECK( !instr.is_likely_branch() );
}

TEST_CASE( "MIPS32_instr: bltzal 1, 1 instruction ahead")
{
    MIPS32Instr instr( "bltzal", 1);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
    CHECK( instr.get_v_dst( 0)  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bltzal -1, 1 instruction ahead")
{
    MIPS32Instr instr( "bltzal", 1);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
    CHECK( instr.get_v_dst( 0)  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bltzal -1, 1024 instructions ahead")
{
    MIPS32Instr instr( "bltzal", 1024);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 + 1024 * 4);
    CHECK( instr.get_v_dst( 0)  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bltzal -1, back to 1024 instruction")
{
    MIPS32Instr instr( "bltzal", 0xffff - 1024 + 1);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 - 1024 * 4);
    CHECK( instr.get_v_dst( 0)  == instr.get_PC() + 8);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: bltzl 0, 1 instruction ahead")
{
    CHECK(MIPS32Instr(0x0622fff3).get_disasm() == "bltzl $s1, -13");
    CHECK(MIPS32Instr(0x06220003).get_disasm() == "bltzl $s1, 3");

    MIPS32Instr instr( "bltzl", 1);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);

    CHECK( !instr.is_common_branch() );
    CHECK( !instr.is_direct_jump() );
    CHECK( !instr.is_indirect_jump() );
    CHECK( instr.is_likely_branch() );
}

TEST_CASE( "MIPS32_instr: bltzl 1, 1 instruction ahead")
{
    MIPS32Instr instr( "bltzl", 1);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bltzl -1, 1 instruction ahead")
{
    MIPS32Instr instr( "bltzl", 1);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bltzl -1, 1024 instructions ahead")
{
    MIPS32Instr instr( "bltzl", 1024);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 + 1024 * 4);
}

TEST_CASE( "MIPS32_instr: bltzl -1, back to 1024 instruction")
{
    MIPS32Instr instr( "bltzl", 0xffff - 1024 + 1);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 - 1024 * 4);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: bltzall 0, 1 instruction ahead")
{
    CHECK(MIPS32Instr(0x0632fff6).get_disasm() == "bltzall $s1, -10");
    CHECK(MIPS32Instr(0x06320003).get_disasm() == "bltzall $s1, 3");

    MIPS32Instr instr( "bltzall", 1);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
    CHECK( instr.get_v_dst( 0)  == instr.get_PC() + 8);

    CHECK( !instr.is_common_branch() );
    CHECK( !instr.is_direct_jump() );
    CHECK( !instr.is_indirect_jump() );
    CHECK( instr.is_likely_branch() );
}

TEST_CASE( "MIPS32_instr: bltzall 1, 1 instruction ahead")
{
    MIPS32Instr instr( "bltzall", 1);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
    CHECK( instr.get_v_dst( 0)  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bltzall -1, 1 instruction ahead")
{
    MIPS32Instr instr( "bltzall", 1);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
    CHECK( instr.get_v_dst( 0)  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bltzall -1, 1024 instructions ahead")
{
    MIPS32Instr instr( "bltzall", 1024);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 + 1024 * 4);
    CHECK( instr.get_v_dst( 0)  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bltzall -1, back to 1024 instruction")
{
    MIPS32Instr instr( "bltzall", 0xffff - 1024 + 1);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 - 1024 * 4);
    CHECK( instr.get_v_dst( 0)  == instr.get_PC() + 8);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: beq two zeroes, one instr ahead")
{
    CHECK(MIPS32Instr(0x1229ffff).get_disasm() == "beq $s1, $t1, -1");
    CHECK(MIPS32Instr(0x1229000e).get_disasm() == "beq $s1, $t1, 14");
    
    MIPS32Instr instr( "beq", 1);
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);

    CHECK( instr.is_common_branch() );
    CHECK( !instr.is_direct_jump() );
    CHECK( !instr.is_indirect_jump() );
    CHECK( !instr.is_likely_branch() );
}

TEST_CASE( "MIPS32_instr: beq -1 and -1, one instr ahead")
{
    MIPS32Instr instr( "beq", 1);
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: beq 0 and 1, one instr and delayed slot ahead")
{
    MIPS32Instr instr( "beq", 1);
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: beq -1 and -1, 1024 instr ahead")
{
    MIPS32Instr instr( "beq", 1024);
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 + 1024 * 4);
}

TEST_CASE( "MIPS32_instr: beq two zeroes, back to 1024 instr")
{
    MIPS32Instr instr( "beq", 0xffff - 1024 + 1);
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 - 1024 * 4);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: bne two zeroes, 1024 instr ahead")
{
    CHECK(MIPS32Instr(0x1629fff1).get_disasm() == "bne $s1, $t1, -15");
    CHECK(MIPS32Instr(0x16290000).get_disasm() == "bne $s1, $t1, 0");
    
    MIPS32Instr instr( "bne", 1024);
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    // Not taken
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);

    CHECK( instr.is_common_branch() );
    CHECK( !instr.is_direct_jump() );
    CHECK( !instr.is_indirect_jump() );
    CHECK( !instr.is_likely_branch() );
}

TEST_CASE( "MIPS32_instr: bne -1 and -1, one instr ahead")
{
    MIPS32Instr instr( "bne", 1);
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    // Not taken
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
}
    
TEST_CASE( "MIPS32_instr: bne -1 and 1, 1024 instr ahead")
{
    MIPS32Instr instr( "bne", 1024);
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 + 1024 * 4);
}

TEST_CASE( "MIPS32_instr: bnel -4 and 1, 1024 instr ahead")
{
    MIPS32Instr instr( "bnel", 1024);
    instr.set_v_src( 0xfffffffc, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 + 1024 * 4);

    CHECK( instr.is_likely_branch() );
    CHECK( !instr.is_common_branch() );
    CHECK( !instr.is_direct_jump() );
    CHECK( !instr.is_indirect_jump() );
}
    
TEST_CASE( "MIPS32_instr: bne 1 and -1, 0 instr ahead")
{
    MIPS32Instr instr( "bne", 0);
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4);
}

TEST_CASE( "MIPS32_instr: eret")
{
    CHECK(MIPS32Instr(0x42000018).get_disasm() == "eret");

    MIPS32Instr instr( "eret");
    instr.set_v_src( 0x200, 0);
    instr.set_v_src( 0b1111, 1);
    instr.execute();
    CHECK( instr.get_new_PC() == 0x200);
    CHECK( instr.get_v_dst( 0) == 0b1011);
    CHECK( instr.get_delayed_slots() == 0);
}

TEST_CASE( "MIPS32_instr: jal to 0xfff-th instr")
{
    CHECK(MIPS32Instr(0x0ffffb2e).get_disasm() == "jal 0x3fffb2e");
    CHECK(MIPS32Instr(0x0c0004d2).get_disasm() == "jal 0x4d2");
    
    MIPS32Instr instr( "jal", 0x0fff);
    instr.execute();
    CHECK( instr.get_new_PC() == 0xfff * 4);
    CHECK( instr.get_v_dst( 0) == instr.get_PC() + 8);

    CHECK( !instr.is_common_branch() );
    CHECK( instr.is_direct_jump() );
    CHECK( !instr.is_indirect_jump() );
    CHECK( !instr.is_likely_branch() );
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: jalr to 1024th byte")
{
    CHECK(MIPS32Instr(0x01208809).get_disasm() == "jalr $s1, $t1");

    MIPS32Instr instr( "jalr");
    instr.set_v_src( 1024, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == 1024);
    CHECK( instr.get_v_dst( 0) == instr.get_PC() + 8);

    CHECK( !instr.is_common_branch() );
    CHECK( !instr.is_direct_jump() );
    CHECK( instr.is_indirect_jump() );
    CHECK( !instr.is_likely_branch() );
}

TEST_CASE( "MIPS32_instr: jalr to 2nd byte (round up to 4th)")
{
    MIPS32Instr instr( "jalr");
    instr.set_v_src( 2, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == 4);
    CHECK( instr.get_v_dst( 0) == instr.get_PC() + 8);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: jr to 0xfffffff4-th byte")
{
    CHECK(MIPS32Instr(0x02200008).get_disasm() == "jr $s1");
    
    MIPS32Instr instr( "jr");
    instr.set_v_src( 0xfffffff4, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == 0xfffffff4);

    CHECK( !instr.is_common_branch() );
    CHECK( !instr.is_direct_jump() );
    CHECK( instr.is_indirect_jump() );
    CHECK( !instr.is_likely_branch() );
}

TEST_CASE( "MIPS32_instr: jr to 2nd byte (round up to 4th)")
{
    MIPS32Instr instr( "jr");
    instr.set_v_src( 2, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == 4);
}

TEST_CASE( "MIPS32_instr: j to 4th instr")
{
    CHECK(MIPS32Instr(0x0bfffb2e).get_disasm() == "j 0x3fffb2e");
    CHECK(MIPS32Instr(0x080004d2).get_disasm() == "j 0x4d2");
    
    MIPS32Instr instr( "j", 4);
    instr.execute();
    CHECK( instr.get_new_PC() == 16);

    CHECK( !instr.is_common_branch() );
    CHECK( instr.is_direct_jump() );
    CHECK( !instr.is_indirect_jump() );
    CHECK( !instr.is_likely_branch() );
}

TEST_CASE( "MIPS32_instr: j loop")
{
    MIPS32Instr instr( "j", 0);
    instr.execute();
    CHECK( instr.has_trap());
    CHECK( instr.trap_type() == Trap::HALT);
}
