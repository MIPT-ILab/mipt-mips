/* MIPS Instruction unit tests
 * @author: Pavel Kryukov, Vsevolod Pukhov, Egor Bova
 * Copyright (C) MIPT-MIPS 2017-2019
 */

#include "mips32_test.h"

#include <catch.hpp>

TEST_CASE ( "MIPS32_instr: disasm CP1 instructions a-c")
{
    CHECK(MIPS32Instr(0x462008c5).get_disasm() == "abs.d $f3, $f1");
    CHECK(MIPS32Instr(0x460008c5).get_disasm() == "abs.s $f3, $f1");
    CHECK(MIPS32Instr(0x462208c0).get_disasm() == "add.d $f3, $f1, $f2");
    CHECK(MIPS32Instr(0x460208c0).get_disasm() == "add.s $f3, $f1, $f2");
    CHECK(MIPS32Instr(0x45000002).get_disasm() == "bc1f $f31, 2");
    CHECK(MIPS32Instr(0x4500fffe).get_disasm() == "bc1f $f31, -2");
    CHECK(MIPS32Instr(0x45010002).get_disasm() == "bc1t $f31, 2");
    CHECK(MIPS32Instr(0x4501fffe).get_disasm() == "bc1t $f31, -2");
    CHECK(MIPS32Instr(0x45020002).get_disasm() == "bc1fl $f31, 2");
    CHECK(MIPS32Instr(0x4502fffe).get_disasm() == "bc1fl $f31, -2");
    CHECK(MIPS32Instr(0x45030002).get_disasm() == "bc1tl $f31, 2");
    CHECK(MIPS32Instr(0x4503fffe).get_disasm() == "bc1tl $f31, -2");
    CHECK(MIPS32Instr(0x46211830).get_disasm() == "c.f.d $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x46011830).get_disasm() == "c.f.s $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x46211831).get_disasm() == "c.un.d $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x46011831).get_disasm() == "c.un.s $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x46211832).get_disasm() == "c.eq.d $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x46011832).get_disasm() == "c.eq.s $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x46211833).get_disasm() == "c.ueq.d $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x46011833).get_disasm() == "c.ueq.s $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x46211834).get_disasm() == "c.olt.d $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x46011834).get_disasm() == "c.olt.s $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x46211835).get_disasm() == "c.ult.d $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x46011835).get_disasm() == "c.ult.s $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x46211836).get_disasm() == "c.ole.d $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x46011836).get_disasm() == "c.ole.s $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x46211837).get_disasm() == "c.ule.d $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x46011837).get_disasm() == "c.ule.s $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x46211838).get_disasm() == "c.sf.d $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x46011838).get_disasm() == "c.sf.s $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x46211839).get_disasm() == "c.ngle.d $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x46011839).get_disasm() == "c.ngle.s $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x4621183a).get_disasm() == "c.seq.d $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x4601183a).get_disasm() == "c.seq.s $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x4621183b).get_disasm() == "c.ngl.d $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x4601183b).get_disasm() == "c.ngl.s $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x4621183c).get_disasm() == "c.lt.d $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x4601183c).get_disasm() == "c.lt.s $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x4621183d).get_disasm() == "c.nge.d $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x4601183d).get_disasm() == "c.nge.s $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x4621183e).get_disasm() == "c.le.d $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x4601183e).get_disasm() == "c.le.s $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x4621183f).get_disasm() == "c.ngt.d $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x4601183f).get_disasm() == "c.ngt.s $f31, $f3, $f1");
    CHECK(MIPS32Instr(0x462008ca).get_disasm() == "ceil.l.d $f3, $f1");
    CHECK(MIPS32Instr(0x460008ca).get_disasm() == "ceil.l.s $f3, $f1");
    CHECK(MIPS32Instr(0x462008ce).get_disasm() == "ceil.w.d $f3, $f1");
    CHECK(MIPS32Instr(0x460008ce).get_disasm() == "ceil.w.s $f3, $f1");
    CHECK(MIPS32Instr(0x44490800).get_disasm() == "cfc1 $t1, $f1");
    CHECK(MIPS32Instr(0x44c90800).get_disasm() == "ctc1 $f1, $t1");
    CHECK(MIPS32Instr(0x46a008e1).get_disasm() == "cvt.d.l $f3, $f1");
    CHECK(MIPS32Instr(0x460008e1).get_disasm() == "cvt.d.s $f3, $f1");
    CHECK(MIPS32Instr(0x468008e1).get_disasm() == "cvt.d.w $f3, $f1");
    CHECK(MIPS32Instr(0x462008e5).get_disasm() == "cvt.l.d $f3, $f1");
    CHECK(MIPS32Instr(0x460008e5).get_disasm() == "cvt.l.s $f3, $f1");
    CHECK(MIPS32Instr(0x462008e0).get_disasm() == "cvt.s.d $f3, $f1");
    CHECK(MIPS32Instr(0x46a008e0).get_disasm() == "cvt.s.l $f3, $f1");
    CHECK(MIPS32Instr(0x468008e0).get_disasm() == "cvt.s.w $f3, $f1");
    CHECK(MIPS32Instr(0x462008e4).get_disasm() == "cvt.w.d $f3, $f1");
    CHECK(MIPS32Instr(0x460008e4).get_disasm() == "cvt.w.s $f3, $f1");
}

TEST_CASE ( "MIPS32_instr: disasm CP1 instructions d-t")
{
    CHECK(MIPS32Instr(0x462208c3).get_disasm() == "div.d $f3, $f1, $f2");
    CHECK(MIPS32Instr(0x460208c3).get_disasm() == "div.s $f3, $f1, $f2");
    CHECK(MIPS32Instr(0x44290800).get_disasm() == "dmfc1 $t1, $f1");
    CHECK(MIPS32Instr(0x44a90800).get_disasm() == "dmtc1 $f1, $t1");
    CHECK(MIPS32Instr(0x462008cb).get_disasm() == "floor.l.d $f3, $f1");
    CHECK(MIPS32Instr(0x460008cb).get_disasm() == "floor.l.s $f3, $f1");
    CHECK(MIPS32Instr(0x462008cf).get_disasm() == "floor.w.d $f3, $f1");
    CHECK(MIPS32Instr(0x460008cf).get_disasm() == "floor.w.s $f3, $f1");
    CHECK(MIPS32Instr(0xd5230fa3).get_disasm() == "ldc1 $f3, 0xfa3($t1)");
    CHECK(MIPS32Instr(0xc5230fa3).get_disasm() == "lwc1 $f3, 0xfa3($t1)");
    CHECK(MIPS32Instr(0x4d2b0041).get_disasm() == "ldxc1 $f1, $t1, $t3");
    CHECK(MIPS32Instr(0x4d2b0040).get_disasm() == "lwxc1 $f1, $t1, $t3");
    CHECK(MIPS32Instr(0x44090800).get_disasm() == "mfc1 $t1, $f1");
    CHECK(MIPS32Instr(0x4c0208e1).get_disasm() == "madd.d $f3, $f0, $f1, $f2");
    CHECK(MIPS32Instr(0x4c0208e0).get_disasm() == "madd.s $f3, $f0, $f1, $f2");
    CHECK(MIPS32Instr(0x462008c6).get_disasm() == "mov.d $f3, $f1");
    CHECK(MIPS32Instr(0x460008c6).get_disasm() == "mov.s $f3, $f1");
    CHECK(MIPS32Instr(0x01205801).get_disasm() == "movf $t3, $t1, $f31");
    CHECK(MIPS32Instr(0x462008d1).get_disasm() == "movf.d $f3, $f1, $f31");
    CHECK(MIPS32Instr(0x460008d1).get_disasm() == "movf.s $f3, $f1, $f31");
    CHECK(MIPS32Instr(0x462908d3).get_disasm() == "movn.d $f3, $f1, $t1");
    CHECK(MIPS32Instr(0x460908d3).get_disasm() == "movn.s $f3, $f1, $t1");
    CHECK(MIPS32Instr(0x01215801).get_disasm() == "movt $t3, $t1, $f31");
    CHECK(MIPS32Instr(0x462108d1).get_disasm() == "movt.d $f3, $f1, $f31");
    CHECK(MIPS32Instr(0x460108d1).get_disasm() == "movt.s $f3, $f1, $f31");
    CHECK(MIPS32Instr(0x462908d2).get_disasm() == "movz.d $f3, $f1, $t1");
    CHECK(MIPS32Instr(0x460908d2).get_disasm() == "movz.s $f3, $f1, $t1");
    CHECK(MIPS32Instr(0x4c0208e9).get_disasm() == "msub.d $f3, $f0, $f1, $f2");
    CHECK(MIPS32Instr(0x4c0208e8).get_disasm() == "msub.s $f3, $f0, $f1, $f2");
    CHECK(MIPS32Instr(0x44890800).get_disasm() == "mtc1 $f1, $t1");
    CHECK(MIPS32Instr(0x462208c2).get_disasm() == "mul.d $f3, $f1, $f2");
    CHECK(MIPS32Instr(0x460208c2).get_disasm() == "mul.s $f3, $f1, $f2");
    CHECK(MIPS32Instr(0x462008c7).get_disasm() == "neg.d $f3, $f1");
    CHECK(MIPS32Instr(0x460008c7).get_disasm() == "neg.s $f3, $f1");
    CHECK(MIPS32Instr(0x4c0208f1).get_disasm() == "nmadd.d $f3, $f0, $f1, $f2");
    CHECK(MIPS32Instr(0x4c0208f0).get_disasm() == "nmadd.s $f3, $f0, $f1, $f2");
    CHECK(MIPS32Instr(0x4c0208f9).get_disasm() == "nmsub.d $f3, $f0, $f1, $f2");
    CHECK(MIPS32Instr(0x4c0208f8).get_disasm() == "nmsub.s $f3, $f0, $f1, $f2");
    CHECK(MIPS32Instr(0x462008d5).get_disasm() == "recip.d $f3, $f1");
    CHECK(MIPS32Instr(0x460008d5).get_disasm() == "recip.s $f3, $f1");
    CHECK(MIPS32Instr(0x462008c8).get_disasm() == "round.l.d $f3, $f1");
    CHECK(MIPS32Instr(0x460008c8).get_disasm() == "round.l.s $f3, $f1");
    CHECK(MIPS32Instr(0x462008cc).get_disasm() == "round.w.d $f3, $f1");
    CHECK(MIPS32Instr(0x460008cc).get_disasm() == "round.w.s $f3, $f1");
    CHECK(MIPS32Instr(0x462008d6).get_disasm() == "rsqrt.d $f3, $f1");
    CHECK(MIPS32Instr(0x460008d6).get_disasm() == "rsqrt.s $f3, $f1");
    CHECK(MIPS32Instr(0xf5230fa3).get_disasm() == "sdc1 $f3, 0xfa3($t1)");
    CHECK(MIPS32Instr(0x4d2b0049).get_disasm() == "sdxc1 $f1, $t1, $t3");
    CHECK(MIPS32Instr(0x462008c4).get_disasm() == "sqrt.d $f3, $f1");
    CHECK(MIPS32Instr(0x460008c4).get_disasm() == "sqrt.s $f3, $f1");
    CHECK(MIPS32Instr(0x462208c1).get_disasm() == "sub.d $f3, $f1, $f2");
    CHECK(MIPS32Instr(0x460208c1).get_disasm() == "sub.s $f3, $f1, $f2");
    CHECK(MIPS32Instr(0xe5230fa3).get_disasm() == "swc1 $f3, 0xfa3($t1)");
    CHECK(MIPS32Instr(0x4d2b0048).get_disasm() == "swxc1 $f1, $t1, $t3");
    CHECK(MIPS32Instr(0x462008c9).get_disasm() == "trunc.l.d $f3, $f1");
    CHECK(MIPS32Instr(0x460008c9).get_disasm() == "trunc.l.s $f3, $f1");
    CHECK(MIPS32Instr(0x462008cd).get_disasm() == "trunc.w.d $f3, $f1");
    CHECK(MIPS32Instr(0x460008cd).get_disasm() == "trunc.w.s $f3, $f1");
}
