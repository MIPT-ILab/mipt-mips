/* MIPS Instruction unit tests
 * @author: Pavel Kryukov, Vsevolod Pukhov, Egor Bova
 * Copyright (C) MIPT-MIPS 2017-2019
 */

#include <catch.hpp>
#include "../mips.h"
#include "../mips_instr.h"
#include <memory/memory.h>

class MIPS32Instr : public BaseMIPSInstr<uint32>
{
public:
    explicit MIPS32Instr( uint32 bytes, Addr pc = 0x0) : BaseMIPSInstr<uint32>( MIPSVersion::v32, Endian::little, bytes, pc) { }
    explicit MIPS32Instr( std::string_view str_opcode, uint32 immediate = 0) : BaseMIPSInstr<uint32>( MIPSVersion::v32, str_opcode, Endian::little, immediate, 0xc000) { }
};

class MIPS32BEInstr : public BaseMIPSInstr<uint32>
{
public:
    explicit MIPS32BEInstr( uint32 bytes, Addr pc = 0x0) : BaseMIPSInstr<uint32>( MIPSVersion::v32, Endian::big, bytes, pc) { }
    explicit MIPS32BEInstr( std::string_view str_opcode, uint32 immediate = 0) : BaseMIPSInstr<uint32>( MIPSVersion::v32, str_opcode, Endian::big, immediate, 0xc000) { }
};

class MIPS64Instr : public BaseMIPSInstr<uint64>
{
public:
    explicit MIPS64Instr( uint32 bytes, Addr pc = 0x0) : BaseMIPSInstr<uint64>( MIPSVersion::v64, Endian::little, bytes, pc) { }
    explicit MIPS64Instr( std::string_view str_opcode, uint32 immediate = 0) : BaseMIPSInstr<uint64>( MIPSVersion::v64, str_opcode, Endian::little, immediate, 0xc000) { }
};

class MIPS64BEInstr : public BaseMIPSInstr<uint64>
{
public:
    explicit MIPS64BEInstr( uint32 bytes, Addr pc = 0x0) : BaseMIPSInstr<uint64>( MIPSVersion::v64, Endian::big, bytes, pc) { }
    explicit MIPS64BEInstr( std::string_view str_opcode, uint32 immediate = 0) : BaseMIPSInstr<uint64>( MIPSVersion::v64, str_opcode, Endian::big, immediate, 0xc000) { }
};

static_assert( std::is_base_of_v<MIPS32::FuncInstr, MIPS32Instr>);
static_assert( std::is_base_of_v<MIPS64::FuncInstr, MIPS64Instr>);

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
    CHECK(MIPS64Instr(0x0139882D).get_disasm() == MIPS64BEInstr(0x0139882D).get_disasm());
}

TEST_CASE("Sequence id print")
{
    MIPS32Instr instr( 0xb531fb2e);
    instr.set_sequence_id( 50);
    std::ostringstream oss;
    oss << std::hex << instr;
    CHECK( oss.str() == "{50}	sdr $s1, 0xfb2e($t1)	 [ ]" );
}

TEST_CASE ( "MIPS32_instr: disasm CP1 instructions")
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
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE ( "MIPS32_instr: add 0 and 1")
{
    MIPS32Instr instr( "add");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE ( "MIPS32_instr: add 1 and -1")
{
    MIPS32Instr instr( "add");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}


TEST_CASE ( "MIPS32_instr: add overflow")
{
    MIPS32Instr instr( "add");
    instr.set_v_src( 0x7fffffff, 0);
    instr.set_v_src( 0x7fffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == NO_VAL32);
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
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: addi 0 and 1")
{
    MIPS32Instr instr( "addi", 1);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: addi 1 and -1")
{
    MIPS32Instr instr( "addi", 0xffff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: addi overflow")
{
    MIPS32Instr instr( "addi", 1);
    instr.set_v_src( 0x7fffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == NO_VAL32);
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
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: addiu 0 and 1")
{
    MIPS32Instr instr( "addiu", 1);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: addiu 1 and -1")
{
    MIPS32Instr instr( "addiu", 0xffff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: addiu overflow")
{
    MIPS32Instr instr( "addiu", 2);
    instr.set_v_src( 0x7fffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x80000001);
    CHECK( instr.has_trap() == false);
}


TEST_CASE( "MIPS64_instr: addiu two zeroes")
{
    CHECK(MIPS64Instr(0x253104d2).get_disasm() == "addiu $s1, $t1, 1234");
    CHECK(MIPS64Instr(0x2531fb2e).get_disasm() == "addiu $s1, $t1, -1234");
    
    MIPS64Instr instr( "addiu");
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS64_instr: addiu 0 and 1")
{
    MIPS64Instr instr( "addiu", 1);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS64_instr: addiu 1 and -1")
{
    MIPS64Instr instr( "addiu", 0xffff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}
    
TEST_CASE( "MIPS64_instr: addiu overflow")
{
    MIPS64Instr instr( "addiu", 2);
    instr.set_v_src( 0x7fffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x80000001);
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
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE ( "MIPS32_instr: addu 0 and 1")
{
    MIPS32Instr instr( "addu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE ( "MIPS32_instr: addu 1 and -1")
{
    MIPS32Instr instr( "addu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE ( "MIPS32_instr: addu overflow")
{
    MIPS32Instr instr( "addu");
    instr.set_v_src( 0x7fffffff, 0);
    instr.set_v_src( 0x7fffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfffffffe);
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
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE ( "MIPS32_instr: and 1 and 1")
{
    MIPS32Instr instr( "and");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE ( "MIPS32_instr: and 1 and -1")
{
    MIPS32Instr instr( "and");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE ( "MIPS32_instr: andi two zeroes")
{
    CHECK(MIPS32Instr(0x313104d2).get_disasm() == "andi $s1, $t1, 0x4d2");
    
    MIPS32Instr instr( "andi");
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE ( "MIPS32_instr: andi 1 and 1")
{
    MIPS32Instr instr( "andi", 1);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE ( "MIPS32_instr: andi 1 and -1")
{
    MIPS32Instr instr( "andi", 0xffff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE ( "MIPS32_instr: andi -1 and -1")
{
    MIPS32Instr instr( "andi", 0xffff);
    instr.set_v_src( 0xffffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x0000ffff);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: bgez 0, 1 instruction ahead")
{
    CHECK(MIPS32Instr(0x0621fffc).get_disasm() == "bgez $s1, -4");
    CHECK(MIPS32Instr(0x0621000c).get_disasm() == "bgez $s1, 12");

    MIPS32Instr instr( "bgez", 1);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
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
    CHECK( instr.get_v_dst()  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bgezal 1, 1 instruction ahead")
{
    MIPS32Instr instr( "bgezal", 1);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
    CHECK( instr.get_v_dst()  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bgezal -1, 1 instruction ahead")
{
    MIPS32Instr instr( "bgezal", 1);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4);
    CHECK( instr.get_v_dst()  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bgezal 1, 1024 instructions ahead")
{
    MIPS32Instr instr( "bgezal", 1024);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 + 1024 * 4);
    CHECK( instr.get_v_dst()  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bgezal 1, back to 1024 instruction")
{
    MIPS32Instr instr( "bgezal", 0xffff - 1024 + 1);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 - 1024 * 4);
    CHECK( instr.get_v_dst()  == instr.get_PC() + 8);
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
    CHECK( instr.get_v_dst()  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bgezall 1, 1 instruction ahead")
{
    MIPS32Instr instr( "bgezall", 1);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
    CHECK( instr.get_v_dst()  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bgezall -1, 1 instruction ahead")
{
    MIPS32Instr instr( "bgezall", 1);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4);
    CHECK( instr.get_v_dst()  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bgezall 1, 1024 instructions ahead")
{
    MIPS32Instr instr( "bgezall", 1024);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 + 1024 * 4);
    CHECK( instr.get_v_dst()  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bgezall 1, back to 1024 instruction")
{
    MIPS32Instr instr( "bgezall", 0xffff - 1024 + 1);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 - 1024 * 4);
    CHECK( instr.get_v_dst()  == instr.get_PC() + 8);
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
    CHECK( instr.get_new_PC() == instr.get_PC() + 4);
    CHECK( instr.get_v_dst()  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bltzal 1, 1 instruction ahead")
{
    MIPS32Instr instr( "bltzal", 1);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4);
    CHECK( instr.get_v_dst()  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bltzal -1, 1 instruction ahead")
{
    MIPS32Instr instr( "bltzal", 1);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
    CHECK( instr.get_v_dst()  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bltzal -1, 1024 instructions ahead")
{
    MIPS32Instr instr( "bltzal", 1024);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 + 1024 * 4);
    CHECK( instr.get_v_dst()  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bltzal -1, back to 1024 instruction")
{
    MIPS32Instr instr( "bltzal", 0xffff - 1024 + 1);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 - 1024 * 4);
    CHECK( instr.get_v_dst()  == instr.get_PC() + 8);
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
    CHECK( instr.get_new_PC() == instr.get_PC() + 4);
    CHECK( instr.get_v_dst()  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bltzall 1, 1 instruction ahead")
{
    MIPS32Instr instr( "bltzall", 1);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4);
    CHECK( instr.get_v_dst()  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bltzall -1, 1 instruction ahead")
{
    MIPS32Instr instr( "bltzall", 1);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 8);
    CHECK( instr.get_v_dst()  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bltzall -1, 1024 instructions ahead")
{
    MIPS32Instr instr( "bltzall", 1024);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 + 1024 * 4);
    CHECK( instr.get_v_dst()  == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: bltzall -1, back to 1024 instruction")
{
    MIPS32Instr instr( "bltzall", 0xffff - 1024 + 1);
    instr.set_v_src( 0xffff'ffff, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == instr.get_PC() + 4 - 1024 * 4);
    CHECK( instr.get_v_dst()  == instr.get_PC() + 8);
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
    
    CHECK( instr.is_branch() );
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
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: break")
{
    CHECK(MIPS32Instr(0x0000000d).get_disasm() == "break");
    
    MIPS32Instr instr( "break");
    instr.execute();
    CHECK( instr.trap_type() == Trap::BREAKPOINT);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: clo zero")
{
    CHECK(MIPS32Instr(0x71208821).get_disasm() == "clo $s1, $t1");
    
    MIPS32Instr instr( "clo");
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: clo -1")
{
    MIPS32Instr instr( "clo");
    instr.set_v_src( 0xffffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 32);
}

TEST_CASE( "MIPS32_instr: clo 0xf0000000")
{
    MIPS32Instr instr( "clo");
    instr.set_v_src( 0xf0000000, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 4);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: clz zero")
{
    CHECK(MIPS32Instr(0x71208820).get_disasm() == "clz $s1, $t1");
    
    MIPS32Instr instr( "clz");
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 32);
}

TEST_CASE( "MIPS32_instr: clz -1")
{
    MIPS32Instr instr( "clz");
    instr.set_v_src( 0xffffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: clz 0x0fff0000")
{
    MIPS32Instr instr( "clz");
    instr.set_v_src( 0x0fff0000, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 4);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dclo zero")
{
    CHECK(MIPS64Instr(0x71208825).get_disasm() == "dclo $s1, $t1");
    
    MIPS64Instr instr( "dclo");
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS64_instr: dclo -1")
{
    MIPS64Instr instr( "dclo");
    instr.set_v_src( 0xffffffffffffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 64);
}

TEST_CASE( "MIPS64_instr: dclo ffe002200011000a")
{
    MIPS64Instr instr( "dclo");
    instr.set_v_src( 0xffe002200011000a, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 11);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dclz zero")
{
    CHECK(MIPS64Instr(0x71208824).get_disasm() == "dclz $s1, $t1");
    
    MIPS64Instr instr( "dclz");
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 64);
}

TEST_CASE( "MIPS64_instr: dclz -1")
{
    MIPS64Instr instr( "dclz");
    instr.set_v_src( 0xffffffffffffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS64_instr: dclz 0x02ffaa00cc720000")
{
    MIPS64Instr instr( "dclz");
    instr.set_v_src( 0x02ffaa00cc720000, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 6);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: div 1 by 1")
{
    CHECK(MIPS32Instr(0x0229001a).get_disasm() == "div $s1, $t1");
    CHECK(MIPS32Instr(0x0229001a).is_divmult());
    CHECK(MIPS32Instr("div").is_divmult());

    MIPS32Instr instr( "div");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 1);
}

TEST_CASE( "MIPS32_instr: div -1 by 1")
{
    MIPS32Instr instr( "div");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0xffffffff);
}

TEST_CASE( "MIPS32_instr: div -1 by -1")
{
    MIPS32Instr instr( "div");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 1);
}

TEST_CASE( "MIPS32_instr: div 1 by -1")
{
    MIPS32Instr instr( "div");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0xffffffff);
}

TEST_CASE( "MIPS32_instr: div 0 by 1")
{
    MIPS32Instr instr( "div");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS32_instr: div 1 by 0")
{
    MIPS32Instr instr( "div");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS32_instr: div 0x80000000 by -1")
{
    MIPS32Instr instr( "div");
    instr.set_v_src( 0x80000000, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS32_instr: div 0x4c4b4000 by 0x1dcd6500")
{
    MIPS32Instr instr( "div");
    instr.set_v_src( 0x4c4b4000, 0);
    instr.set_v_src( 0x1dcd6500, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0x10b07600);
    CHECK( instr.get_v_dst()  == 2);
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
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 1);
}

TEST_CASE( "MIPS32_instr: divu -1 by 1")
{
    MIPS32Instr instr( "divu");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0xffffffff);
}

TEST_CASE( "MIPS32_instr: divu -1 by -1")
{
    MIPS32Instr instr( "divu");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 1);
}

TEST_CASE( "MIPS32_instr: divu 1 by -1")
{
    MIPS32Instr instr( "divu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 1);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS32_instr: divu 0 by 1")
{
    MIPS32Instr instr( "divu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS32_instr: divu 1 by 0")
{
    MIPS32Instr instr( "divu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS32_instr: divu 0x80000000 by -1")
{
    MIPS32Instr instr( "divu");
    instr.set_v_src( 0x80000000, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0x80000000);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS32_instr: divu 0x4c4b4000 by 0x1dcd6500")
{
    MIPS32Instr instr( "divu");
    instr.set_v_src( 0x4c4b4000, 0);
    instr.set_v_src( 0x1dcd6500, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0x10b07600);
    CHECK( instr.get_v_dst()  == 2);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dsllv 0xaaaaaaaafee1dead by 0")
{
    CHECK(MIPS64Instr(0x03298814).get_disasm() == "dsllv $s1, $t1, $t9");
    
    MIPS64Instr instr( "dsllv");
    instr.set_v_src( 0xaaaaaaaafee1dead, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xaaaaaaaafee1dead);
}

TEST_CASE( "MIPS64_instr: dsllv 2 by 1")
{
    MIPS64Instr instr( "dsllv");
    
    instr.set_v_src( 2, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 4);
}

TEST_CASE( "MIPS64_instr: dsllv 1 by 32")
{
    MIPS64Instr instr( "dsllv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 32, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x100000000);
}

TEST_CASE( "MIPS64_instr: dsllv 1 by 64 (shift-variable overflow)")
{
    MIPS64Instr instr( "dsllv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 64, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS64_instr: dsllv 1 by 128 (shift-variable overflow)")
{
    MIPS64Instr instr( "dsllv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 128, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS64_instr: dsrav 0xfeedabcd by 0")
{
    CHECK(MIPS64Instr(0x03298817).get_disasm() == "dsrav $s1, $t1, $t9");
    
    MIPS64Instr instr( "dsrav");
    instr.set_v_src( 0xfeedabcd, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfeedabcd);
}

TEST_CASE( "MIPS64_instr: dsrav 0xab by 0xff")
{
    MIPS64Instr instr( "dsrav");
    instr.set_v_src( 0xab, 0);
    instr.set_v_src( 0xff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS64_instr: dsrav 0x123400000000 by 4")
{
    MIPS64Instr instr( "dsrav");
    instr.set_v_src( 0x123400000000, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x012340000000);
}

TEST_CASE( "MIPS64_instr: dsrav 0xffab000000000000 by 4")
{
    MIPS64Instr instr( "dsrav");
    instr.set_v_src( 0xffab000000000000, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfffab00000000000);
}

TEST_CASE( "MIPS64_instr: dsrav 1 by 64 (shift-variable overflow)")
{
    MIPS64Instr instr( "dsrav");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 64, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS64_instr: dsrlv 0xdeadbeef by 0")
{
    CHECK(MIPS64Instr(0x03298816).get_disasm() == "dsrlv $s1, $t1, $t9");
    
    MIPS64Instr instr( "dsrlv");
    instr.set_v_src( 0xdeadbeef, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xdeadbeef);
}

TEST_CASE( "MIPS64_instr: dsrlv 1 by 1")
{
    MIPS64Instr instr( "dsrlv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS64_instr: dsrlv 0x01a00000 by 8")
{
    MIPS64Instr instr( "dsrlv");
    instr.set_v_src( 0x01a00000, 0);
    instr.set_v_src( 8, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x0001a000);
}

TEST_CASE( "MIPS64_instr: dsrlv 0x8765432000000011 by 16")
{
    MIPS64Instr instr( "dsrlv");
    instr.set_v_src( 0x8765432000000011, 0);
    instr.set_v_src( 16, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x0000876543200000);
}

TEST_CASE( "MIPS64_instr: dsrlv 1 by 64 (shift-variable overflow)")
{
    MIPS64Instr instr( "dsrlv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 64, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: j to 4th instr")
{
    CHECK(MIPS32Instr(0x0bfffb2e).get_disasm() == "j 0x3fffb2e");
    CHECK(MIPS32Instr(0x080004d2).get_disasm() == "j 0x4d2");
    
    MIPS32Instr instr( "j", 4);
    instr.execute();
    CHECK( instr.get_new_PC() == 16);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: jal to 0xfff-th instr")
{
    CHECK(MIPS32Instr(0x0ffffb2e).get_disasm() == "jal 0x3fffb2e");
    CHECK(MIPS32Instr(0x0c0004d2).get_disasm() == "jal 0x4d2");
    
    MIPS32Instr instr( "jal", 0x0fff);
    instr.execute();
    CHECK( instr.get_new_PC() == 0xfff * 4);
    CHECK( instr.get_v_dst() == instr.get_PC() + 8);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: jalr to 1024th byte")
{
    CHECK(MIPS32Instr(0x01208809).get_disasm() == "jalr $s1, $t1");

    MIPS32Instr instr( "jalr");
    instr.set_v_src( 1024, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == 1024);
    CHECK( instr.get_v_dst() == instr.get_PC() + 8);
}

TEST_CASE( "MIPS32_instr: jalr to 2nd byte (round up to 4th)")
{
    MIPS32Instr instr( "jalr");
    instr.set_v_src( 2, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == 4);
    CHECK( instr.get_v_dst() == instr.get_PC() + 8);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: jr to 0xfffffff4-th byte")
{
    CHECK(MIPS32Instr(0x02200008).get_disasm() == "jr $s1");
    
    MIPS32Instr instr( "jr");
    instr.set_v_src( 0xfffffff4, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == 0xfffffff4);
}

TEST_CASE( "MIPS32_instr: jr to 2nd byte (round up to 4th)")
{
    MIPS32Instr instr( "jr");
    instr.set_v_src( 2, 0);
    instr.execute();
    CHECK( instr.get_new_PC() == 4);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: lui with 0x4d2")
{
    CHECK(MIPS32Instr(0x3c1104d2).get_disasm() == "lui $s1, 0x4d2");

    MIPS32Instr instr( "lui", 0x4d2);
    instr.execute();
    CHECK( instr.get_v_dst()  == 0x4d20000);
}

TEST_CASE( "MIPS32_instr: lui with 1")
{
    MIPS32Instr instr( "lui", 1);
    instr.execute();
    CHECK( instr.get_v_dst()  == 0x10000);
}

TEST_CASE( "MIPS32_instr: lui with 0")
{
    MIPS32Instr instr( "lui", 0);
    instr.execute();
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS32_instr: lui with 0xfee1")
{
    MIPS32Instr instr( "lui", 0xfee1);
    instr.execute();
    CHECK( instr.get_v_dst()  == 0xfee10000);
}
////////////////////////////////////////////////////////////////////////////////

static auto get_plain_memory_with_data()
{
    auto memory = FuncMemory::create_plain_memory(15);
    memory->write<uint32, Endian::little>( 0xABCD'1234, 0x1000);
    memory->write<uint32, Endian::little>( 0xBADC'5678, 0x1004);
    return memory;
}

TEST_CASE( "MIPS32_instr: lb (most significant bit is 0)")
{
    CHECK(MIPS32Instr(0x813104d2).get_disasm() == "lb $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0x8131fb2e).get_disasm() == "lb $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "lb", 0x0fff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst() == 0x34);
}

TEST_CASE( "MIPS32_instr: lb (most significant bit is 1)")
{
    MIPS32Instr instr( "lb", 0x0fff);
    instr.set_v_src( 4, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1003);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst() == 0xffffffab);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: lbu (most significant bit is 0)")
{
    CHECK(MIPS32Instr(0x913104d2).get_disasm() == "lbu $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0x9131fb2e).get_disasm() == "lbu $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "lbu", 0x0fff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst() == 0x34);
}

TEST_CASE( "MIPS32_instr: lbu (most significant bit is 1)")
{
    MIPS32Instr instr( "lbu", 0x0fff);
    instr.set_v_src( 4, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1003);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst() == 0xab);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: lh (most significant bit is 0)")
{
    CHECK(MIPS32Instr(0x853104d2).get_disasm() == "lh $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0x8531fb2e).get_disasm() == "lh $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "lh", 0x0fff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst() == 0x1234);
}

TEST_CASE( "MIPS32_instr: lh (most significant bit is 1)")
{
    MIPS32Instr instr( "lh", 0x0fff);
    instr.set_v_src( 3, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1002);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst() == 0xffff'ABCD);
}

TEST_CASE( "MIPS32_instr: lh unaligned address trap")
{
    MIPS32Instr instr( "lh", 0);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 1);
    CHECK( instr.trap_type() == Trap::UNALIGNED_ADDRESS);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: lhu (most significant bit is 0)")
{
    CHECK(MIPS32Instr(0x953104d2).get_disasm() == "lhu $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0x9531fb2e).get_disasm() == "lhu $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "lhu", 0x0fff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst() == 0x1234);
}

TEST_CASE( "MIPS32_instr: lhu (most significant bit is 1)")
{
    MIPS32Instr instr( "lhu", 0x0fff);
    instr.set_v_src( 3, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1002);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst() == 0xABCD);
}

TEST_CASE( "MIPS32_instr: lhu unaligned address trap")
{
    MIPS32Instr instr( "lhu", 0);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 1);
    CHECK( instr.trap_type() == Trap::UNALIGNED_ADDRESS);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: lwl (instr->mem_addr % 4 = 0)")
{
    CHECK(MIPS32Instr(0x893104d2).get_disasm() == "lwl $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0x8931fb2e).get_disasm() == "lwl $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "lwl", 0x0fff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x0ffd);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst() == 0x3400'0000);
}

TEST_CASE( "MIPS32_instr: lwl (instr->mem_addr % 4 = 3)")
{
    MIPS32Instr instr( "lwl", 0x0fff);
    instr.set_v_src( 4, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst() == 0xABCD'1234);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: lwr (instr->mem_addr % 4 = 0)")
{
    CHECK(MIPS32Instr(0x993104d2).get_disasm() == "lwr $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0x9931fb2e).get_disasm() == "lwr $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "lwr", 0x0fff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst() == 0xABCD'1234);
}

TEST_CASE( "MIPS32_instr: lwr (instr->mem_addr % 4 = 3)")
{
    MIPS32Instr instr( "lwr", 0x0fff);
    instr.set_v_src( 4, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1003);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst() == 0xDC56'78AB);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: ll (most significant bit is 0)")
{
    CHECK(MIPS32Instr(0xc13104d2).get_disasm() == "ll $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xc131fb2e).get_disasm() == "ll $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "ll", 0x0fff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst() == 0x1234);
}

TEST_CASE( "MIPS32_instr: ll (most significant bit is 1)")
{
    MIPS32Instr instr( "ll", 0x0fff);
    instr.set_v_src( 3, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1002);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst() == 0xffff'ABCD);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: ld")
{
    CHECK(MIPS64Instr(0xdd3104d2).get_disasm() == "ld $s1, 0x4d2($t1)");
    CHECK(MIPS64Instr(0xdd31fb2e).get_disasm() == "ld $s1, 0xfb2e($t1)");

    MIPS64Instr instr( "ld", 0x0fff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst() == 0xBADC'5678'ABCD'1234);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: ldl")
{
    CHECK(MIPS32Instr(0x693104d2).get_disasm() == "ldl $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0x6931fb2e).get_disasm() == "ldl $s1, 0xfb2e($t1)");

    MIPS64Instr instr( "ldl", 0x0ffd);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x0ffe);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst() == 0x5678'ABCD'1234'0000);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: ldr")
{
    CHECK(MIPS32Instr(0x6d3104d2).get_disasm() == "ldr $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0x6d31fb2e).get_disasm() == "ldr $s1, 0xfb2e($t1)");

    MIPS64Instr instr( "ldr", 0x0ffd);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x0ffe);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst() == 0x5678'ABCD'1234'0000);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: lw le")
{
    CHECK(MIPS32Instr(0x8d3104d2).get_disasm() == "lw $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0x8d31fb2e).get_disasm() == "lw $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "lw", 0x0fff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst() == 0xABCD'1234);
}

TEST_CASE( "MIPS32_instr: lw be")
{
    CHECK(MIPS32BEInstr(0x8d3104d2).get_disasm() == "lw $s1, 0x4d2($t1)");
    CHECK(MIPS32BEInstr(0x8d31fb2e).get_disasm() == "lw $s1, 0xfb2e($t1)");

    MIPS32BEInstr instr( "lw", 0x0fff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst() == 0x3412'CDAB);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: sb 0x12")
{
    CHECK(MIPS32Instr(0xa13104d2).get_disasm() == "sb $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xa131fb2e).get_disasm() == "sb $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "sb", 0x1000);
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0x12, 1);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    auto memory = get_plain_memory_with_data();
    memory->load_store( &instr);
    auto value = memory->read<uint8, Endian::little>( 0x1000);
    CHECK( value == 0x12);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: sh 0xdead")
{
    CHECK(MIPS32Instr(0xa53104d2).get_disasm() == "sh $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xa531fb2e).get_disasm() == "sh $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "sh", 0x1000);
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0xdead, 1);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    auto memory = get_plain_memory_with_data();
    memory->load_store( &instr);
    auto value = memory->read<uint16, Endian::little>( 0x1000);
    CHECK( value == 0xdead);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: sw 0xfee1'dead")
{
    CHECK(MIPS32Instr(0xad3104d2).get_disasm() == "sw $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xad31fb2e).get_disasm() == "sw $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "sw", 0x1000);
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0xfee1'dead, 1);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    auto memory = get_plain_memory_with_data();
    memory->load_store( &instr);
    auto value = memory->read<uint32, Endian::little>( 0x1000);
    CHECK( value == 0xfee1'dead);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: swl (instr->mem_addr % 4 = 2)")
{
    CHECK(MIPS32Instr(0xa93104d2).get_disasm() == "swl $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xa931fb2e).get_disasm() == "swl $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "swl", 0x1003);
    instr.set_v_src( 2, 0);
    instr.set_v_src( 0xfee1'dead, 1);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1002);

    auto memory = get_plain_memory_with_data();
    memory->load_store( &instr);
    auto value = memory->read<uint32, Endian::little>( 0x1002);
    CHECK( value == 0xfee1'abcd);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: swr (instr->mem_addr % 4 = 2)")
{
    CHECK(MIPS32Instr(0xb93104d2).get_disasm() == "swr $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xb931fb2e).get_disasm() == "swr $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "swr", 0x1000);
    instr.set_v_src( 2, 0);
    instr.set_v_src( 0xfee1'dead, 1);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1002);

    auto memory = get_plain_memory_with_data();
    memory->load_store( &instr);
    auto value = memory->read<uint32, Endian::little>( 0x1002);
    CHECK( value == 0x5678'dead);
}
////////////////////////////////////////////////////////////////////////////////

//Data cache model is not implemeted so sc is tested like sh
TEST_CASE( "MIPS32_instr: sc 0xdead")
{
    CHECK(MIPS32Instr(0xe13104d2).get_disasm() == "sc $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xe131fb2e).get_disasm() == "sc $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "sc", 0x1000);
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xdead, 1);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1001);

    auto memory = get_plain_memory_with_data();
    memory->load_store( &instr);
    auto value = memory->read<uint32, Endian::little>( 0x1001);
    CHECK( value == 0x78ab'dead);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: sd 0xdead'beef'fee1'dead")
{
    CHECK(MIPS64Instr(0xfd3104d2).get_disasm() == "sd $s1, 0x4d2($t1)");
    CHECK(MIPS64Instr(0xfd31fb2e).get_disasm() == "sd $s1, 0xfb2e($t1)");

    MIPS64Instr instr( "sd", 0x1000);
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0xdead'beef'fee1'dead, 1);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    auto memory = get_plain_memory_with_data();
    memory->load_store( &instr);
    auto value = memory->read<uint64, Endian::little>( 0x1000);
    CHECK( value == 0xdead'beef'fee1'dead);
}
////////////////////////////////////////////////////////////////////////////////

//Instructions sdl and sdr are not implemented
TEST_CASE( "MIPS64_instr: sdl 0xdead'beef'fee1'dead")
{
    CHECK(MIPS32Instr(0xb13104d2).get_disasm() == "sdl $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xb131fb2e).get_disasm() == "sdl $s1, 0xfb2e($t1)");

    MIPS64Instr instr( "sdl", 0x1000);
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0xdead'beef'fee1'dead, 1);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    auto memory = get_plain_memory_with_data();
    memory->load_store( &instr);
    auto value = memory->read<uint64, Endian::little>( 0x1000);
    CHECK( value == 0xdead'beef'fee1'dead);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: sdr 0xdead'beef'fee1'dead")
{
    CHECK(MIPS32Instr(0xb53104d2).get_disasm() == "sdr $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xb531fb2e).get_disasm() == "sdr $s1, 0xfb2e($t1)");

    MIPS64Instr instr( "sdr", 0x1000);
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0xdead'beef'fee1'dead, 1);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    auto memory = get_plain_memory_with_data();
    memory->load_store( &instr);
    auto value = memory->read<uint64, Endian::little>( 0x1000);
    CHECK( value == 0xdead'beef'fee1'dead);
}
////////////////////////////////////////////////////////////////////////////////

static void check_move( std::string_view name)
{
    MIPS32Instr instr( name);
    instr.set_v_src( 0x12345678, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x12345678);
}

static void mult_check_0_by_0( std::string_view name)
{
    MIPS32Instr instr( name);
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0);
}

static void mult_check_1_by_1( std::string_view name)
{
    MIPS32Instr instr( name);
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 1);
}

static void mult_check_minus1_by_minus1( std::string_view name)
{
    MIPS32Instr instr( name);
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 1);
}

static void mult_check_minus1_by_1( std::string_view name)
{
    MIPS32Instr instr( name);
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0xffffffff);
    CHECK( instr.get_v_dst()  == 0xffffffff);
}

static void mult_check_10000_by_10000( std::string_view name)
{
    MIPS32Instr instr( name);
    instr.set_v_src( 0x10000, 0);
    instr.set_v_src( 0x10000, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 1);
    CHECK( instr.get_v_dst()  == 0);
}

static void mult_check_min_int_by_min_int( std::string_view name)
{
    MIPS32Instr instr( name);
    instr.set_v_src( 0x80000000, 0);
    instr.set_v_src( 0x80000000, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0x40000000);
    CHECK( instr.get_v_dst()  == 0);
}

static void mult_unsigned_check_minus1_by_minus1( std::string_view name)
{
    MIPS32Instr instr( name);
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0xfffffffe);
    CHECK( instr.get_v_dst()  == 1);
}

static void mult_unsigned_check_minus1_by_1( std::string_view name)
{
    MIPS32Instr instr( name);
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0xffffffff);
}

static void mult_unsigned_check_min_int_by_min_int( std::string_view name)
{
    MIPS32Instr instr( name);
    instr.set_v_src( 0x80000000, 0);
    instr.set_v_src( 0x80000000, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0x40000000);
    CHECK( instr.get_v_dst()  == 0);
}

static void mult_unsigned_check_random( std::string_view name)
{
    MIPS32Instr instr( name);
    instr.set_v_src( 0xcecb8f27, 0);
    instr.set_v_src( 0xfd87b5f2, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0xcccccccb);
    CHECK( instr.get_v_dst()  == 0x7134e5de);
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

////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: mfc0 0x12345678")
{
    CHECK(MIPS32Instr(0x408d2000).get_disasm() == "mfc0 $a0, $Cause");
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
    CHECK( instr.get_v_dst() == 1);
    CHECK( instr.get_mask() == all_ones<uint32>());
}

TEST_CASE( "MIPS32_instr: movn 3 if 2 isn't equal to 0")
{
    MIPS32Instr instr( "movn");
    instr.set_v_src( 3, 0);
    instr.set_v_src( 2, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 3);
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
    CHECK( instr.get_v_dst() == 0);
    CHECK( instr.get_mask() == all_ones<uint32>());
}

TEST_CASE( "MIPS32_instr: movz 3 if 0 is equal to 0")
{
    MIPS32Instr instr( "movz");
    instr.set_v_src( 3, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 3);
    CHECK( instr.get_mask() == all_ones<uint32>());
}
////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: mtc0 0x12345678")
{
    CHECK(MIPS32Instr(0x40046800).get_disasm() == "mtc0 $Cause, $a0");
    check_move( "mtc0");
}

////////////////////////////////////////////////////////////////////////////////

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

TEST_CASE( "MIPS32_instr: mul 1 by 0")
{
    CHECK(MIPS32Instr(0x71398802).get_disasm() == "mul $s1, $t1, $t9");
    
    MIPS32Instr instr( "mul");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: mul 1 by 1")
{
    MIPS32Instr instr( "mul");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: mul 1 by 10")
{
    MIPS32Instr instr( "mul");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 10, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 10);
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
    CHECK( instr.get_v_dst() == 0xffffffff);
}

TEST_CASE( "MIPS32_instr: nor 1 and 1")
{
    MIPS32Instr instr( "nor");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfffffffe);
}

TEST_CASE( "MIPS32_instr: nor 1 and -1")
{
    MIPS32Instr instr( "nor");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}


TEST_CASE( "MIPS64_instr: nor 0 and 0")
{
    CHECK(MIPS64Instr(0x01398827).get_disasm() == "nor $s1, $t1, $t9");

    MIPS64Instr instr( "nor");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffffffffffffffff);
}

TEST_CASE( "MIPS64_instr: nor 1 and 1")
{
    MIPS64Instr instr( "nor");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfffffffffffffffe);
}

TEST_CASE( "MIPS64_instr: nor 1 and -1")
{
    MIPS64Instr instr( "nor");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffffffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: or 0 and 0")
{
    CHECK(MIPS32Instr(0x01398825).get_disasm() == "or $s1, $t1, $t9");
    
    MIPS32Instr instr( "or");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: or 1 and 1")
{
    MIPS32Instr instr( "or");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: or 1 and -1")
{
    MIPS32Instr instr( "or");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffffffff);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: ori 0 and 0")
{
    CHECK(MIPS32Instr(0x353104d2).get_disasm() == "ori $s1, $t1, 0x4d2");
    
    MIPS32Instr instr( "ori", 0);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: ori 1 and 1")
{
    MIPS32Instr instr( "ori", 1);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: ori 1 and -1")
{
    MIPS32Instr instr( "ori", 0x0000ffff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x0000ffff);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE ( "MIPS32_instr: sll 0x00098cc0 by 0")
{
    CHECK(MIPS32Instr(0x00098cc0).get_disasm() == "sll $s1, $t1, 19");
    
    MIPS32Instr instr( "sll", 0);
    instr.set_v_src( 0x00098cc0, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x00098cc0);
}

TEST_CASE ( "MIPS32_instr: sll 51 by 1")
{
    MIPS32Instr instr( "sll", 1);
    instr.set_v_src( 51, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 102);
}

TEST_CASE ( "MIPS32_instr: sll 0xaabbccdd by 8")
{
    MIPS32Instr instr( "sll", 8);
    instr.set_v_src( 0xaabbccdd, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xbbccdd00);
}

TEST_CASE ( "MIPS32_instr: sll 1 by 31")
{
    MIPS32Instr instr( "sll", 31);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x80000000);
}


TEST_CASE ( "MIPS64_instr: sll 100 by 0")
{
    CHECK(MIPS64Instr(0x00098cc0).get_disasm() == "sll $s1, $t1, 19");

    MIPS64Instr instr( "sll", 0);
    instr.set_v_src( 100, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 100);
}

TEST_CASE ( "MIPS64_instr: sll 3 by 2")
{
    MIPS64Instr instr( "sll", 2);
    instr.set_v_src( 3, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 12);
}

TEST_CASE ( "MIPS64_instr: sll 1 by 16")
{
    MIPS64Instr instr( "sll", 16);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x10000);
}

TEST_CASE ( "MIPS64_instr: sll 1 by 31")
{
    MIPS64Instr instr( "sll", 31);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffff'ffff'8000'0000);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32Instr: slt 0 < 0")
{
    CHECK(MIPS32Instr(0x0139882a).get_disasm() == "slt $s1, $t1, $t9");

    MIPS32Instr instr( "slt");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: slt 1 < 0")
{
    MIPS32Instr instr( "slt");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: slt 0 < 1")
{
    MIPS32Instr instr( "slt");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: slt -1 < 0")
{
    MIPS32Instr instr( "slt");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: slt 0 < -1")
{
    MIPS32Instr instr( "slt");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: slt 1 < -1")
{
    MIPS32Instr instr( "slt");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: slt -1 < 1")
{
    MIPS32Instr instr( "slt");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: sllv 1 by 0")
{
    CHECK(MIPS32Instr(0x03298804).get_disasm() == "sllv $s1, $t1, $t9");
    
    MIPS32Instr instr( "sllv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: sllv 1 by 1")
{
    MIPS32Instr instr( "sllv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 2);
}

TEST_CASE( "MIPS32_instr: sllv 1 by 32 + 8 (shift-variable overflow)")
{
    MIPS32Instr instr( "sllv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 32 + 8, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x100);
}

TEST_CASE( "MIPS32_instr: sllv 1 by 32 (shift-variable overflow)")
{
    MIPS32Instr instr( "sllv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 32, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: sllv by 64 (shift-variable overflow)")
{
    MIPS32Instr instr( "sllv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 64, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS64_instr: sllv by 64 (shift-variable ovreflow)")
{
    MIPS64Instr instr( "sllv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 64, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: slti 0 < 0")
{
    CHECK(MIPS32Instr(0x293104d2).get_disasm() == "slti $s1, $t1, 1234");
    CHECK(MIPS32Instr(0x2931fb2e).get_disasm() == "slti $s1, $t1, -1234");

    MIPS32Instr instr( "slti", 0);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: slti 1 < 0")
{
    MIPS32Instr instr( "slti", 0);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: slti 0 < 1")
{
    MIPS32Instr instr( "slti", 1);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: slti -1 < 0")
{
    MIPS32Instr instr( "slti", 0);
    instr.set_v_src( 0xffffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: slti 0 < -1")
{
    MIPS32Instr instr( "slti", 0xffff);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: slti 1 < -1")
{
    MIPS32Instr instr( "slti", 0xffff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: slti -1 < 1")
{
    MIPS32Instr instr( "slti", 1);
    instr.set_v_src( 0xffffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: sltiu 0 < 0")
{
    CHECK(MIPS32Instr(0x2d3104d2).get_disasm() == "sltiu $s1, $t1, 1234");
    CHECK(MIPS32Instr(0x2d31fb2e).get_disasm() == "sltiu $s1, $t1, -1234");
    
    MIPS32Instr instr( "sltiu", 0);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: sltiu 1 < 0")
{
    MIPS32Instr instr( "sltiu", 0);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: sltiu 0 < 1")
{
    MIPS32Instr instr( "sltiu", 1);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: sltiu -1 < 0")
{
    MIPS32Instr instr( "sltiu", 0);
    instr.set_v_src( 0xffffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: sltiu 0 < -1")
{
    MIPS32Instr instr( "sltiu", 0xffff);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: sltiu -1 < 1")
{
    MIPS32Instr instr( "sltiu", 1);
    instr.set_v_src( 0xffffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: sltiu 1 < -1")
{
    MIPS32Instr instr( "sltiu", 0xffff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: sltu 0 < 0")
{
    CHECK(MIPS32Instr(0x0139882b).get_disasm() == "sltu $s1, $t1, $t9");
    
    MIPS32Instr instr( "sltu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: sltu 1 < 0")
{
    MIPS32Instr instr( "sltu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: sltu 0 < 1")
{
    MIPS32Instr instr( "sltu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: sltu -1 < 0")
{
    MIPS32Instr instr( "sltu");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: sltu 0 < -1")
{
    MIPS32Instr instr( "sltu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: sltu -1 < 1")
{
    MIPS32Instr instr( "sltu");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: sra 0xabcd1234 by 0")
{
    CHECK(MIPS32Instr(0x00098cc3).get_disasm() == "sra $s1, $t1, 19");
    
    MIPS32Instr instr( "sra", 0);
    instr.set_v_src( 0xabcd1234, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xabcd1234);
}

TEST_CASE( "MIPS32_instr: sra 49 by 1")
{
    MIPS32Instr instr( "sra", 1);
    instr.set_v_src( 49, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 24);
}

TEST_CASE( "MIPS32_instr: sra 0x1000 by 4")
{
    MIPS32Instr instr( "sra", 4);
    instr.set_v_src( 0x1000, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x100);
}

TEST_CASE( "MIPS32_instr: sra 0xffa00000 by 8")
{
    MIPS32Instr instr( "sra", 8);
    instr.set_v_src( 0xffa00000, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffffa000);
}


TEST_CASE( "MIPS64_instr: sra 0xdeadc0dde by 0")
{
    CHECK(MIPS64Instr(0x00098cc3).get_disasm() == "sra $s1, $t1, 19");

    MIPS64Instr instr( "sra", 0);
    instr.set_v_src( 0xdeadc0de, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffff'ffff'dead'c0de);
}

TEST_CASE( "MIPS64_instr: sra 0x0fffffff by 2")
{
    MIPS64Instr instr( "sra", 2);
    instr.set_v_src( 0x0fffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x03ffffff);
}

TEST_CASE( "MIPS64_instr: sra 0xdead by 4")
{
    MIPS64Instr instr( "sra", 4);
    instr.set_v_src( 0xdead, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x0dea);
}

TEST_CASE( "MIPS64_instr: sra 0xf1234567 by 16")
{
    MIPS64Instr instr( "sra", 16);
    instr.set_v_src( 0xf1234567, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffff'ffff'ffff'f123);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: srav 0x321 by 0")
{
    CHECK(MIPS32Instr(0x03298807).get_disasm() == "srav $s1, $t1, $t9");
    
    MIPS32Instr instr( "srav");
    instr.set_v_src( 0x321, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x321);
}

TEST_CASE( "MIPS32_instr: srav 14 by 0xffffff03 shift-variable overflow")
{
    MIPS32Instr instr( "srav");
    instr.set_v_src( 14, 0);
    instr.set_v_src( 0xffffff03, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: srav 0x333311 by 4")
{
    MIPS32Instr instr( "srav");
    instr.set_v_src( 0x333311, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x33331);
}

TEST_CASE( "MIPS32_instr: srav 0xaabb0000 by 4")
{
    MIPS32Instr instr( "srav");
    instr.set_v_src( 0xaabb0000, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfaabb000);
}


TEST_CASE( "MIPS64_instr: srav 24 by 0")
{
    CHECK(MIPS64Instr(0x03298807).get_disasm() == "srav $s1, $t1, $t9");
    
    MIPS64Instr instr( "srav");
    instr.set_v_src( 24, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 24);
}

TEST_CASE( "MIPS64_instr: srav 10 by 1")
{
    MIPS64Instr instr( "srav");
    instr.set_v_src( 10, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 5);
}

TEST_CASE( "MIPS64_instr: srav 0x000a by 4")
{
    MIPS64Instr instr( "srav");
    instr.set_v_src( 0x000a, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS64_instr: srav 0xff000000 by 4")
{
    MIPS64Instr instr( "srav");
    instr.set_v_src( 0xff000000, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffff'ffff'fff0'0000);
}

TEST_CASE( "MIPS64_instr: srav 0xffff0000 by 32 (shift-variable overflow)")
{
    MIPS64Instr instr( "srav");
    instr.set_v_src( 0xffff0000, 0);
    instr.set_v_src( 32, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffff'ffff'ffff'0000);
}

TEST_CASE ( "MIPS32_instr: srl 0xdeadbeef by 0")
{
    CHECK(MIPS32Instr(0x00098cc2).get_disasm() == "srl $s1, $t1, 19");
    
    MIPS32Instr instr( "srl", 0);
    instr.set_v_src( 0xdeadbeef, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xdeadbeef);
}

TEST_CASE ( "MIPS32_instr: srl 0xabcd1234 by 5")
{
    MIPS32Instr instr( "srl", 5);
    instr.set_v_src( 0xabcd1234, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x055e6891);
}

TEST_CASE ( "MIPS32_instr: srl 0xc0dec0de by 4")
{
    MIPS32Instr instr( "srl", 4);
    instr.set_v_src( 0xc0dec0de, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x0c0dec0d);
}

TEST_CASE ( "MIPS32_instr: srl 0x80000000 by 16")
{
    MIPS32Instr instr( "srl", 16);
    instr.set_v_src( 0x80000000, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x00008000);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: srlv 0xa1 by 0")
{
    CHECK(MIPS32Instr(0x03298806).get_disasm() == "srlv $s1, $t1, $t9");
    
    MIPS32Instr instr( "srlv");
    instr.set_v_src( 0xa1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xa1);
}

TEST_CASE( "MIPS32_instr: srlv 153 by 3")
{
    MIPS32Instr instr( "srlv");
    instr.set_v_src( 153, 0);
    instr.set_v_src( 3, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 19);
}

TEST_CASE( "MIPS32_instr: srlv 0xfeed by 8")
{
    MIPS32Instr instr( "srlv");
    instr.set_v_src( 0xfeed, 0);
    instr.set_v_src( 8, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x00fe);
}

TEST_CASE( "MIPS32_instr: srlv 0xaaa00000 by 4")
{
    MIPS32Instr instr( "srlv");
    instr.set_v_src( 0xaaa00000, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x0aaa0000);
}

TEST_CASE( "MIPS32_instr: srlv 1 by 32 (shift-variable overflow)")
{
    MIPS32Instr instr( "srlv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 32, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS64_instr: srlv 0x11 by 0x00000a00 (shift-variable overflow)")
{
    MIPS64Instr instr( "srlv");
    instr.set_v_src( 0x11, 0);
    instr.set_v_src( 0x00000a00, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x11);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: sub 1 from 1")
{
    CHECK(MIPS32Instr(0x01398822).get_disasm() == "sub $s1, $t1, $t9");
    MIPS32Instr instr( "sub");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK(instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: sub 1 from 10")
{
    MIPS32Instr instr( "sub");
    instr.set_v_src( 10, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK(instr.get_v_dst() == 9);
}

TEST_CASE( "MIPS32_instr: sub 0 from 1")
{
    MIPS32Instr instr( "sub");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK(instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: sub overflow")
{
    MIPS32Instr instr( "sub");
    instr.set_v_src( 0x80000000, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK(instr.get_v_dst() == NO_VAL32);
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
    CHECK(instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: subu 1 from 10")
{ 
    MIPS32Instr instr( "subu");
    instr.set_v_src( 10, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK(instr.get_v_dst() == 9);
}

TEST_CASE( "MIPS32_instr: subu 0 from 1")
{   
    MIPS32Instr instr( "subu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK(instr.get_v_dst() == 1);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: syscall")
{
    CHECK(MIPS32Instr(0x0000000c).get_disasm() == "syscall");
    
    MIPS32Instr instr( "syscall");
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
    CHECK( instr.is_syscall());
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: teq if equal")
{
    CHECK(MIPS32Instr(0x02290034).get_disasm() == "teq $s1, $t1");
    
    MIPS32Instr instr( "teq");
    instr.set_v_src( 15, 0);
    instr.set_v_src( 15, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}

TEST_CASE( "MIPS32_instr: teq if not equal")
{
    MIPS32Instr instr( "teq");
    instr.set_v_src( 0xff, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: teqi if equal")
{
    CHECK(MIPS32Instr(0x062c04d2).get_disasm() == "teqi $s1, 0x4d2");
    CHECK(MIPS32Instr(0x062cfb2e).get_disasm() == "teqi $s1, 0xfb2e");
    
    MIPS32Instr instr( "teqi", 0);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}

TEST_CASE( "MIPS32_instr: teqi if not equal")
{
    MIPS32Instr instr( "teqi", 4);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: tge if less")
{
    CHECK(MIPS32Instr(0x02290030).get_disasm() == "tge $s1, $t1");
    
    MIPS32Instr instr( "tge");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 2, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}

TEST_CASE( "MIPS32_instr: tge if equal")
{
    MIPS32Instr instr( "tge");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}

TEST_CASE( "MIPS32_instr: tge if greater")
{
    MIPS32Instr instr( "tge");
    instr.set_v_src( 2, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: tgei if less")
{
    CHECK(MIPS32Instr(0x062804d2).get_disasm() == "tgei $s1, 0x4d2");
    CHECK(MIPS32Instr(0x0628fb2e).get_disasm() == "tgei $s1, 0xfb2e");
    
    MIPS32Instr instr( "tgei", 4);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}

TEST_CASE( "MIPS32_instr: tgei if equal")
{
    MIPS32Instr instr( "tgei", 0);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}

TEST_CASE( "MIPS32_instr: tgei if greater")
{
    MIPS32Instr instr( "tgei", 1);
    instr.set_v_src( 8, 0);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: tgeiu if less")
{
    CHECK(MIPS32Instr(0x062904d2).get_disasm() == "tgeiu $s1, 0x4d2");
    CHECK(MIPS32Instr(0x0629fb2e).get_disasm() == "tgeiu $s1, 0xfb2e");

    MIPS32Instr instr( "tgeiu", 4);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}

TEST_CASE( "MIPS32_instr: tgeiu if equal")
{
    MIPS32Instr instr( "tgeiu", 0);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}

TEST_CASE( "MIPS32_instr: tgeiu if greater-unsigned")
{
    MIPS32Instr instr( "tgeiu", 1);
    instr.set_v_src( 0xfffffffc, 0);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: tgeu if less-unsigned")
{
    CHECK(MIPS32Instr(0x02290031).get_disasm() == "tgeu $s1, $t1");

    MIPS32Instr instr( "tgeu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xfffffffa, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}

TEST_CASE( "MIPS32_instr: tgeu if equal")
{
    MIPS32Instr instr( "tgeu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}

TEST_CASE( "MIPS32_instr: tgeu if greater-unsigned")
{
    MIPS32Instr instr( "tgeu");
    instr.set_v_src( 0xfffffffa, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: tlt if less")
{
    CHECK(MIPS32Instr(0x02290032).get_disasm() == "tlt $s1, $t1");

    MIPS32Instr instr( "tlt");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 2, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}

TEST_CASE( "MIPS32_instr: tlt if equal")
{
    MIPS32Instr instr( "tlt");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}

TEST_CASE( "MIPS32_instr: tlt if greater")
{
    MIPS32Instr instr( "tlt");
    instr.set_v_src( 2, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: tlti if less")
{
    CHECK(MIPS32Instr(0x062a04d2).get_disasm() == "tlti $s1, 0x4d2");
    CHECK(MIPS32Instr(0x062afb2e).get_disasm() == "tlti $s1, 0xfb2e");

    MIPS32Instr instr( "tlti", 4);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}

TEST_CASE( "MIPS32_instr: tlti if equal")
{
    MIPS32Instr instr( "tlti", 0);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}

TEST_CASE( "MIPS32_instr: tlti if greater")
{
    MIPS32Instr instr( "tlti", 1);
    instr.set_v_src( 8, 0);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: tltiu if less")
{
    CHECK(MIPS32Instr(0x062b04d2).get_disasm() == "tltiu $s1, 0x4d2");
    CHECK(MIPS32Instr(0x062bfb2e).get_disasm() == "tltiu $s1, 0xfb2e");

    MIPS32Instr instr( "tltiu", 4);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}

TEST_CASE( "MIPS32_instr: tltiu if equal")
{
    MIPS32Instr instr( "tltiu", 0);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}

TEST_CASE( "MIPS32_instr: tltiu if greater-unsigned")
{
    MIPS32Instr instr( "tltiu", 1);
    instr.set_v_src( 0xfffffffc, 0);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: tltu if less-unsigned")
{
    CHECK(MIPS32Instr(0x02290033).get_disasm() == "tltu $s1, $t1");

    MIPS32Instr instr( "tltu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xfffffffa, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}

TEST_CASE( "MIPS32_instr: tltu if equal")
{
    MIPS32Instr instr( "tltu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}

TEST_CASE( "MIPS32_instr: tltu if greater-unsigned")
{
    MIPS32Instr instr( "tltu");
    instr.set_v_src( 0xfffffffa, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: tne if not equal")
{
    CHECK(MIPS32Instr(0x02290036).get_disasm() == "tne $s1, $t1");

    MIPS32Instr instr( "tne");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}

TEST_CASE( "MIPS32_instr: tne if equal")
{
    MIPS32Instr instr( "tne");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: tnei if not equal")
{
    CHECK(MIPS32Instr(0x062e04d2).get_disasm() == "tnei $s1, 0x4d2");
    CHECK(MIPS32Instr(0x062efb2e).get_disasm() == "tnei $s1, 0xfb2e");

    MIPS32Instr instr( "tnei", 4);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.trap_type() == Trap::EXPLICIT_TRAP);
}

TEST_CASE( "MIPS32_instr: tnei if equal")
{
    MIPS32Instr instr( "tnei", 0);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: xor 0 and 0")
{
    CHECK(MIPS32Instr(0x01398826).get_disasm() == "xor $s1, $t1, $t9");

    MIPS32Instr instr( "xor");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: xor -1 and -1")
{
    MIPS32Instr instr( "xor");
    instr.set_v_src( 0xffffffff, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: xor 1 and -1")
{
    MIPS32Instr instr( "xor");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xfffffffe);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: xori 0 with 0")
{
    CHECK(MIPS32Instr(0x393104d2).get_disasm() == "xori $s1, $t1, 0x4d2");
    MIPS32Instr instr( "xori", 0);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK(instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS32_instr: xori 0 with 1")
{
    MIPS32Instr instr( "xori", 1);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK(instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: xori 1 with 0")
{
    MIPS32Instr instr( "xori", 0);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK(instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS32_instr: xori 0xa with 0x5")
{   
    MIPS32Instr instr( "xori", 5);
    instr.set_v_src( 0xa, 0);
    instr.execute();
    CHECK(instr.get_v_dst() == 0xf);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: load dump")
{
    MIPS32Instr instr(0x8d310ff0);
    CHECK( instr.get_disasm() == "lw $s1, 0xff0($t1)");
    CHECK( instr.bytes_dump() == "Bytes: 0xf0 0x0f 0x31 0x8d");

    instr.set_v_src( 0x10, 0);
    instr.set_sequence_id( 0);
    instr.execute();
    CHECK( instr.string_dump() == "{0}\tlw $s1, 0xff0($t1)\t [ $ma = 0x1000 ]");

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.string_dump() == "{0}\tlw $s1, 0xff0($t1)\t [ $ma = 0x1000, $s1 = 0xabcd1234 ]" );
}

TEST_CASE( "MIPS32_instr: load dump with trap")
{
    MIPS32Instr instr(0x8d3104d0);
    CHECK( instr.get_disasm() == "lw $s1, 0x4d0($t1)");

    instr.set_v_src( 0x1, 0);
    instr.set_sequence_id( 0);
    instr.execute();
    CHECK( instr.string_dump() == "{0}\tlw $s1, 0x4d0($t1)\t [ $ma = 0x4d1 ]\t trap");
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
////////////////////////////////////////////////////////////////////////////////

//-------------------------MIPS64Instr unit-tests-----------------------------//

static bool not_a_mips32_instruction( std::string_view name)
{
    MIPS32Instr instr( name);
    instr.execute(); 
    return instr.trap_type() == Trap::UNKNOWN_INSTRUCTION;
}

TEST_CASE ( "MIPS64_instr: dadd two zeroes")
{
    CHECK(MIPS32Instr(0x0139882C).get_disasm() == "dadd $s1, $t1, $t9");
    CHECK( not_a_mips32_instruction("dadd"));
    
    MIPS64Instr instr( "dadd");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE ( "MIPS64_instr: dadd 0 and 1")
{
    MIPS64Instr instr( "dadd");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE ( "MIPS64_instr: dadd 1 and -1")
{
    MIPS64Instr instr( "dadd");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffffffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}


TEST_CASE ( "MIPS64_instr: dadd overflow")
{
    MIPS64Instr instr( "dadd");
    instr.set_v_src( 0x7fffffffffffffff, 0);
    instr.set_v_src( 0x7fffffffffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == NO_VAL32);
    CHECK( instr.trap_type() != Trap::NO_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: daddi two zeroes")
{
    CHECK(MIPS64Instr(0x613104d2).get_disasm() == "daddi $s1, $t1, 1234");
    CHECK(MIPS64Instr(0x6131fb2e).get_disasm() == "daddi $s1, $t1, -1234");
    CHECK( not_a_mips32_instruction("daddi"));

    MIPS64Instr instr( "daddi", 0);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS64_instr: daddi 0 and 1")
{
    MIPS64Instr instr( "daddi", 1);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS64_instr: daddi 1 and -1")
{
    MIPS64Instr instr( "daddi", 0xffff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}
    

TEST_CASE( "MIPS64_instr: daddi overflow")
{
    MIPS64Instr instr( "daddi", 1);
    instr.set_v_src( 0x7fffffffffffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == NO_VAL32);
    CHECK( instr.trap_type() != Trap::NO_TRAP);
}
////////////////////////////////////////////////////////////////////////////////
    
TEST_CASE( "MIPS64_instr: daddiu two zeroes")
{
    CHECK(MIPS32Instr(0x653104d2).get_disasm() == "daddiu $s1, $t1, 1234");
    CHECK(MIPS32Instr(0x6531fb2e).get_disasm() == "daddiu $s1, $t1, -1234");
    CHECK( not_a_mips32_instruction("daddiu"));

    MIPS64Instr instr( "daddiu", 0);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS64_instr: daddiu 0 and 1")
{
    MIPS64Instr instr( "daddiu", 1);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS64_instr: daddiu 1 and -1")
{
    MIPS64Instr instr( "daddiu", 0xffff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}


TEST_CASE( "MIPS64_instr: daddiu overflow")
{
    MIPS64Instr instr( "daddiu", 1);
    instr.set_v_src( 0x7fffffffffffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x8000000000000000);
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE ( "MIPS64_instr: daddu two zeroes")
{
    CHECK(MIPS64Instr(0x0139882D).get_disasm() == "daddu $s1, $t1, $t9");
    CHECK( not_a_mips32_instruction("daddu"));

    MIPS64Instr instr( "daddu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE ( "MIPS64_instr: daddu 0 and 1")
{
    MIPS64Instr instr( "daddu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE ( "MIPS64_instr: daddu 1 and -1")
{
    MIPS64Instr instr( "daddu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0);
}

TEST_CASE ( "MIPS64_instr: daddu overflow")
{
    MIPS64Instr instr( "daddu");
    instr.set_v_src( 0x7fff'ffff'ffff'ffff, 0);
    instr.set_v_src( 0x7fff'ffff'ffff'ffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffff'ffff'ffff'fffe);
    CHECK( instr.has_trap() == false);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: ddiv 1 by 1")
{
    CHECK(MIPS32Instr(0x0229001e).get_disasm() == "ddiv $s1, $t1");
    CHECK(MIPS64Instr(0x0229001e).is_divmult());
    CHECK(MIPS64Instr( "ddiv").is_divmult());
    CHECK( not_a_mips32_instruction( "ddiv"));

    MIPS64Instr instr( "ddiv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 1);
}

TEST_CASE( "MIPS64_instr: ddiv -1 by 1")
{
    MIPS64Instr instr( "ddiv");
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0xffff'ffff'ffff'ffff);
}

TEST_CASE( "MIPS64_instr: ddiv -1 by -1")
{
    MIPS64Instr instr( "ddiv");
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 0);
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 1);
}

TEST_CASE( "MIPS64_instr: ddiv 1 by -1")
{
    MIPS64Instr instr( "ddiv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0xffff'ffff'ffff'ffff);
}

TEST_CASE( "MIPS64_instr: ddiv 0 by 1")
{
    MIPS64Instr instr( "ddiv");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS64_instr: ddiv 1 by 0")
{
    MIPS64Instr instr( "ddiv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS64_instr: ddiv 0x8000'0000'0000'0000 by -1")
{
    MIPS64Instr instr( "ddiv");
    instr.set_v_src( 0x8000'0000'0000'0000, 0);
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS64_instr: ddiv 0x4c4b'4000'0000'0000 by 0x1dcd'6500'0000'0000")
{
    MIPS64Instr instr( "ddiv");
    instr.set_v_src( 0x4c4b'4000'0000'0000, 0);
    instr.set_v_src( 0x1dcd'6500'0000'0000, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0x10b0'7600'0000'0000);
    CHECK( instr.get_v_dst()  == 2);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: ddivu 1 by 1")
{
    CHECK(MIPS32Instr(0x0229001f).get_disasm() == "ddivu $s1, $t1");
    CHECK(MIPS32Instr(0x0229001f).is_divmult());
    CHECK(MIPS32Instr( "ddivu").is_divmult());
    CHECK( not_a_mips32_instruction( "ddivu"));

    MIPS64Instr instr( "ddivu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 1);
}

TEST_CASE( "MIPS64_instr: ddivu -1 by 1")
{
    MIPS64Instr instr( "ddivu");
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0xffff'ffff'ffff'ffff);
}

TEST_CASE( "MIPS64_instr: ddivu -1 by -1")
{
    MIPS64Instr instr( "ddivu");
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 0);
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 1);
}

TEST_CASE( "MIPS64_instr: ddivu 1 by -1")
{
    MIPS64Instr instr( "ddivu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 1);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS64_instr: ddivu 0 by 1")
{
    MIPS64Instr instr( "ddivu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS64_instr: ddivu 1 by 0")
{
    MIPS64Instr instr( "ddivu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS64_instr: ddivu 0x8000'0000'0000'0000 by -1")
{
    MIPS64Instr instr( "ddivu");
    instr.set_v_src( 0x8000'0000'0000'0000, 0);
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0x8000'0000'0000'0000);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS64_instr: ddivu 0x4c4b'4000'0000'0000 by 0x1dcd'6500'0000'0000")
{
    MIPS64Instr instr( "ddivu");
    instr.set_v_src( 0x4c4b'4000'0000'0000, 0);
    instr.set_v_src( 0x1dcd'6500'0000'0000, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0x10b0'7600'0000'0000);
    CHECK( instr.get_v_dst()  == 2);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dmult 0 by 0")
{
    CHECK(MIPS64Instr(0x0229001c).get_disasm() == "dmult $s1, $t1");
    CHECK(MIPS64Instr(0x0229001c).is_divmult());
    CHECK(MIPS64Instr("dmult").is_divmult());
    CHECK( not_a_mips32_instruction("dmult"));

    MIPS64Instr instr( "dmult");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS64_instr: dmult 1 by 1")
{
    MIPS64Instr instr( "dmult");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 1);
}

TEST_CASE( "MIPS64_instr: dmult -1 by -1")
{
    MIPS64Instr instr( "dmult");
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 0);
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 1);
}

TEST_CASE( "MIPS64_instr: dmult -1 by 1")
{
    MIPS64Instr instr( "dmult");
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0xffff'ffff'ffff'ffff);
    CHECK( instr.get_v_dst()  == 0xffff'ffff'ffff'ffff);
}

TEST_CASE( "MIPS64_instr: dmult 0x100000000 by 0x100000000")
{
    MIPS64Instr instr( "dmult");
    instr.set_v_src( 0x100000000, 0);
    instr.set_v_src( 0x100000000, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 1);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS64_instr: dmult 0x8000'0000'0000'0000 by 0x8000'0000'0000'0000")
{
    MIPS64Instr instr( "dmult");
    instr.set_v_src( 0x8000'0000'0000'0000, 0);
    instr.set_v_src( 0x8000'0000'0000'0000, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0x4000'0000'0000'0000);
    CHECK( instr.get_v_dst()  == 0);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dmultu 0 by 0")
{
    CHECK(MIPS64Instr(0x0229001d).get_disasm() == "dmultu $s1, $t1");
    CHECK(MIPS64Instr(0x0229001d).is_divmult());
    CHECK(MIPS64Instr("dmultu").is_divmult());
    CHECK( not_a_mips32_instruction("dmultu"));

    MIPS64Instr instr( "dmultu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS64_instr: dmultu 1 by 1")
{
    MIPS64Instr instr( "dmultu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 1);
}

TEST_CASE( "MIPS64_instr: dmultu -1 by -1")
{
    MIPS64Instr instr( "dmultu");    
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 0);
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0xffff'ffff'ffff'fffe);
    CHECK( instr.get_v_dst()  == 1);
}

TEST_CASE( "MIPS64_instr: dmultu -1 by 0")
{
    MIPS64Instr instr( "dmultu");
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS64_instr: dmultu -1 by 1")
{
    MIPS64Instr instr( "dmultu");
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0);
    CHECK( instr.get_v_dst()  == 0xffff'ffff'ffff'ffff);
}

TEST_CASE( "MIPS64_instr: dmultu 0x100000000 by 0x100000000")
{
    MIPS64Instr instr( "dmultu");
    instr.set_v_src( 0x100000000, 0);
    instr.set_v_src( 0x100000000, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 1);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS64_instr: dmultu 0x8000'0000'0000'0000 by 0x8000'0000'0000'0000")
{
    MIPS64Instr instr( "dmultu");
    instr.set_v_src( 0x8000'0000'0000'0000, 0);
    instr.set_v_src( 0x8000'0000'0000'0000, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0x4000'0000'0000'0000);
    CHECK( instr.get_v_dst()  == 0);
}

TEST_CASE( "MIPS64_instr: dmultu 0xcecb'8f27'0000'0000 by 0xfd87'b5f2'0000'0000")
{
    MIPS64Instr instr( "dmultu");
    instr.set_v_src( 0xcecb'8f27'0000'0000, 0);
    instr.set_v_src( 0xfd87'b5f2'0000'0000, 1);
    instr.execute();
    CHECK( instr.get_v_dst2() == 0xcccc'cccb'7134'e5de);
    CHECK( instr.get_v_dst()  == 0);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE ( "MIPS64_instr: dsll 0xaaaa'aaaa'0009'8cc0 by 0")
{
    CHECK(MIPS32Instr(0x00098cf8).get_disasm() == "dsll $s1, $t1, 19");
    CHECK( not_a_mips32_instruction("dsll"));

    MIPS64Instr instr( "dsll", 0);
    instr.set_v_src( 0xaaaa'aaaa'0009'8cc0, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xaaaa'aaaa'0009'8cc0);
}

TEST_CASE ( "MIPS64_instr: dsll 51 by 1")
{
    MIPS64Instr instr( "dsll", 1);
    instr.set_v_src( 51, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 102);
}

TEST_CASE ( "MIPS64_instr: dsll 0x8899'aabb'ccdd'eeff by 8")
{
    MIPS64Instr instr( "dsll", 8);
    instr.set_v_src( 0x8899'aabb'ccdd'eeff, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x99aa'bbcc'ddee'ff00);
}

TEST_CASE ( "MIPS64_instr: dsll 1 by 63")
{
    MIPS64Instr instr( "dsll", 63);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x8000'0000'0000'0000);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dsll32 0xaaaa'aaaa'0009'8ccf by 0")
{
    CHECK(MIPS32Instr(0x00098cfc).get_disasm() == "dsll32 $s1, $t1, 19");
    CHECK( not_a_mips32_instruction("dsll32"));

    MIPS64Instr instr( "dsll32", 0);
    instr.set_v_src( 0xaaaa'aaaa'0009'8ccf, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x0009'8ccf'0000'0000);
}

TEST_CASE ( "MIPS64_instr: dsll32 0x8899'aabb'ccdd'eeff by 8")
{
    MIPS64Instr instr( "dsll32", 8);
    instr.set_v_src( 0x8899'aabb'ccdd'eeff, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xddee'ff00'0000'0000);
}

TEST_CASE( "MIPS64_instr: dsll32 1 by 31")
{
    MIPS64Instr instr( "dsll32", 31);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x8000'0000'0000'0000);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dsra 0xabcd'1234'abcd'1234 by 0")
{
    CHECK(MIPS64Instr(0x00098cfb).get_disasm() == "dsra $s1, $t1, 19");
    CHECK( not_a_mips32_instruction("dsra"));
    
    MIPS64Instr instr( "dsra", 0);
    instr.set_v_src( 0xabcd'1234'abcd'1234, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xabcd'1234'abcd'1234);
}

TEST_CASE( "MIPS64_instr: dsra 49 by 1")
{
    MIPS64Instr instr( "dsra", 1);
    instr.set_v_src( 49, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 24);
}

TEST_CASE( "MIPS64_instr: dsra 0x1000 by 4")
{
    MIPS64Instr instr( "dsra", 4);
    instr.set_v_src( 0x1000, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x100);
}

TEST_CASE( "MIPS64_instr: dsra 0xffa0'0000'0000'0000 by 16")
{
    MIPS64Instr instr( "dsra", 16);
    instr.set_v_src( 0xffa0'0000'0000'0000, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffff'ffa0'0000'0000);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dsra32 0xabcd'1234'abcd'1234 by 0")
{
    CHECK(MIPS64Instr(0x00098cff).get_disasm() == "dsra32 $s1, $t1, 19");
    CHECK( not_a_mips32_instruction("dsra32"));
    
    MIPS64Instr instr( "dsra32", 0);
    instr.set_v_src( 0xabcd'1234'abcd'1234, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffff'ffff'abcd'1234);
}

TEST_CASE( "MIPS64_instr: dsra32 0x1000'0000'0000 by 4")
{
    MIPS64Instr instr( "dsra32", 4);
    instr.set_v_src( 0x1000'0000'0000, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x100);
}

TEST_CASE( "MIPS64_instr: dsra32 0xffa0'0000'0000'0000 by 16")
{
    MIPS64Instr instr( "dsra32", 16);
    instr.set_v_src( 0xffa0'0000'0000'0000, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffff'ffff'ffff'ffa0);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dsrl 0xabcd'1234'abcd'1234 by 0")
{
    CHECK(MIPS32Instr(0x00098cfa).get_disasm() == "dsrl $s1, $t1, 19");
    CHECK( not_a_mips32_instruction("dsrl"));
    
    MIPS64Instr instr( "dsrl", 0);
    instr.set_v_src( 0xabcd'1234'abcd'1234, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xabcd'1234'abcd'1234);
}

TEST_CASE( "MIPS64_instr: dsrl 49 by 1")
{
    MIPS64Instr instr( "dsrl", 1);
    instr.set_v_src( 49, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 24);
}

TEST_CASE( "MIPS64_instr: dsrl 0x1000 by 4")
{
    MIPS64Instr instr( "dsrl", 4);
    instr.set_v_src( 0x1000, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x100);
}

TEST_CASE( "MIPS64_instr: dsrl 0xffa0'0000'0000'0000 by 16")
{
    MIPS64Instr instr( "dsrl", 16);
    instr.set_v_src( 0xffa0'0000'0000'0000, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffa0'0000'0000);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dsrl32 0xabcd'1234'abcd'1234 by 0")
{
    CHECK(MIPS32Instr(0x00098cfe).get_disasm() == "dsrl32 $s1, $t1, 19");
    CHECK( not_a_mips32_instruction("dsrl32"));
    
    MIPS64Instr instr( "dsrl32", 0);
    instr.set_v_src( 0xabcd'1234'abcd'1234, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xabcd1234);
}

TEST_CASE( "MIPS64_instr: dsrl32 0x1000'0000'0000 by 4")
{
    MIPS64Instr instr( "dsrl32", 4);
    instr.set_v_src( 0x1000'0000'0000, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x100);
}

TEST_CASE( "MIPS64_instr: dsrl32 0xffa0'0000'0000'0000 by 16")
{
    MIPS64Instr instr( "dsrl32", 16);
    instr.set_v_src( 0xffa0'0000'0000'0000, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffa0);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: dsub 1 from 1")
{
    CHECK(MIPS64Instr(0x0139882e).get_disasm() == "dsub $s1, $t1, $t9");
    CHECK( not_a_mips32_instruction("dsub"));

    MIPS64Instr instr( "dsub");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK(instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS64_instr: dsub 1 from 10")
{
    MIPS64Instr instr( "dsub");
    instr.set_v_src( 10, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK(instr.get_v_dst() == 9);
}

TEST_CASE( "MIPS64_instr: dsub 0 from 1")
{
    MIPS64Instr instr( "dsub");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK(instr.get_v_dst() == 1);
}

TEST_CASE( "MIPS64_instr: dsub overflow")
{
    MIPS64Instr instr( "dsub");
    instr.set_v_src( 0x8000000000000000, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK(instr.get_v_dst() == NO_VAL32);
    CHECK(instr.trap_type() == Trap::INTEGER_OVERFLOW);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dsubu 1 from 1")
{
    CHECK(MIPS32Instr(0x0139882f).get_disasm() == "dsubu $s1, $t1, $t9");
    CHECK( not_a_mips32_instruction("dsubu"));

    MIPS64Instr instr( "dsubu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK(instr.get_v_dst() == 0);
}

TEST_CASE( "MIPS64_instr: dsubu 1 from 10")
{ 
    MIPS64Instr instr( "dsubu");
    instr.set_v_src( 10, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK(instr.get_v_dst() == 9);
}

TEST_CASE( "MIPS64_instr: dsubu 0 from 1")
{   
    MIPS64Instr instr( "dsubu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK(instr.get_v_dst() == 1);
}
////////////////////////////////////////////////////////////////////////////////
