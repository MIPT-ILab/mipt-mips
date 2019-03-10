/*
 * RISC-V instruction unit tests
 * Author pavel.kryukov@phystech.edu
 * Copyright 2019 MIPT-MIPS
 */
 
#include "../riscv_instr.h"
 
#include <catch.hpp>

TEST_CASE("RISCV disassembly")
{
    CHECK( RISCVInstr<uint32>(0x597).get_disasm() == "auipc $a1, 0x0" );
    CHECK( RISCVInstr<uint32>(0x00f70463).get_disasm() == "beq $a4, $a5, 8");
    CHECK( RISCVInstr<uint32>(0x00052783).get_disasm() == "lw $a5, 0x0($a0)");
    CHECK( RISCVInstr<uint32>(0xf95ff06f).get_disasm() == "jal $zero, -108");
    CHECK( RISCVInstr<uint32>(0x30529073).get_disasm() == "csrrw $mtvec, $zero, $t0");
    CHECK( RISCVInstr<uint32>(0x10200073).get_disasm() == "sret");
    CHECK( RISCVInstr<uint32>(0x30200073).get_disasm() == "mret");
    CHECK( RISCVInstr<uint32>(0x30202373).get_disasm() == "csrrs $medeleg, $t1, $zero");
    CHECK( RISCVInstr<uint32>(0x30205073).get_disasm() == "csrrwi $medeleg, $zero, 0x0");
}

TEST_CASE("RISCV invalid instruction")
{
    CHECK( RISCVInstr<uint32>(0x0).get_disasm() == "unknown" );
    CHECK( RISCVInstr<uint32>("qwerty").get_disasm() == "unknown" );
}

TEST_CASE("RISCV bytes dump")
{
    CHECK( RISCVInstr<uint32>(0x204002b7).bytes_dump() == "Bytes: 0xb7 0x02 0x40 0x20");
}

TEST_CASE("RISCV add")
{
    CHECK( RISCVInstr<uint32>(0x00b505b3).get_disasm() == "add $a1, $a0, $a1");
    RISCVInstr<uint32> instr( "add");
    instr.set_v_src( 0x10, 0);
    instr.set_v_src( 0xf, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x1f);
}

template<typename T>
static bool test_imm_instr( std::string_view name, T dst, T src, T imm)
{
    RISCVInstr<T> instr( name);
    instr.set_v_src( src, 0);
    instr.set_v_imm( imm);
    instr.execute();
    return instr.get_v_dst() == dst;
}

TEST_CASE("RISCV addiw")
{
    CHECK( test_imm_instr<uint64>( "addiw", 0x00000000, 0x00000000, 0x000 ) );
    CHECK( test_imm_instr<uint64>( "addiw", 0x00000002, 0x00000001, 0x001 ) );
    CHECK( test_imm_instr<uint64>( "addiw", 0x0000000a, 0x00000003, 0x007 ) );
    CHECK( test_imm_instr<uint64>( "addiw", 0xfffffffffffff800, 0x0000000000000000, 0x800 ) );
    CHECK( test_imm_instr<uint64>( "addiw", 0xffffffff80000000, 0xffffffff80000000, 0x000 ) );
    CHECK( test_imm_instr<uint64>( "addiw", 0x000000007ffff800, 0xffffffff80000000, 0x800 ) );
    CHECK( test_imm_instr<uint64>( "addiw", 0x00000000000007ff, 0x00000000, 0x7ff ) );
    CHECK( test_imm_instr<uint64>( "addiw", 0x000000007fffffff, 0x7fffffff, 0x000 ) );
    CHECK( test_imm_instr<uint64>( "addiw", 0xffffffff800007fe, 0x7fffffff, 0x7ff ) );
    CHECK( test_imm_instr<uint64>( "addiw", 0xffffffff800007ff, 0xffffffff80000000, 0x7ff ) );
    CHECK( test_imm_instr<uint64>( "addiw", 0x000000007ffff7ff, 0x000000007fffffff, 0x800 ) );
    CHECK( test_imm_instr<uint64>( "addiw", 0xffffffffffffffff, 0x0000000000000000, 0xfff ) );
    CHECK( test_imm_instr<uint64>( "addiw", 0x0000000000000000, 0xffffffffffffffff, 0x001 ) );
    CHECK( test_imm_instr<uint64>( "addiw", 0xfffffffffffffffe, 0xffffffffffffffff, 0xfff ) );
    CHECK( test_imm_instr<uint64>( "addiw", 0xffffffff80000000, 0x7fffffff, 0x001 ) );
}

TEST_CASE("RISCV lui 1")
{
    CHECK( RISCVInstr<uint32>(0x204002b7).get_disasm() == "lui $t0, 0x20400");

    RISCVInstr<uint32> instr("lui");
    instr.set_v_imm(0x1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x1000);
}

TEST_CASE("RISCV lui all fs")
{
    RISCVInstr<uint32> instr("lui");
    instr.set_v_imm(0xfffff);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffff'f000ULL);
}

TEST_CASE("RISCV lui 80000")
{
    RISCVInstr<uint64> instr(0x800007b7);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffff'ffff'8000'0000ULL);
}

TEST_CASE("RISCV-128 lui all fs")
{
    RISCVInstr<uint64> instr("lui");
    instr.set_v_imm(0xfffff);
    instr.execute();
    CHECK( ~instr.get_v_dst() == 0xfff);
}

TEST_CASE("RISCV sub")
{
    CHECK( RISCVInstr<uint32>(0x40e787b3).get_disasm() == "sub $a5, $a5, $a4");
    RISCVInstr<uint32> instr( "sub");
    instr.set_v_src( 0x10, 0);
    instr.set_v_src( 0xf, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
}

TEST_CASE("RISCV sub print")
{
    RISCVInstr<uint32> instr(0x40e787b3);
    instr.set_v_src( 0x10, 0);
    instr.set_v_src( 0xf, 1);
    instr.set_sequence_id( 80);
    instr.execute();
    CHECK( instr.get_v_dst() == 1);
    CHECK( instr.string_dump() == "{80}\tsub $a5, $a5, $a4\t [ $a5 = 0x1 ]" );
}
