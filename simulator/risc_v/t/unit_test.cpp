/*
 * RISC-V instruction unit tests
 * Author pavel.kryukov@phystech.edu
 * Copyright 2019 MIPT-MIPS
 */
 
#include "../riscv_instr.h"
 
#include <catch.hpp>

TEST_CASE("RISCV disassembly")
{
    CHECK( RISCVInstr<uint32>(0x00000597).get_disasm() == "auipc $a1, 0x0" );
    CHECK( RISCVInstr<uint32>(0x00f70463).get_disasm() == "beq $a4, $a5, 8");
    CHECK( RISCVInstr<uint32>(0x00052783).get_disasm() == "lw $a5, 0x0($a0)");
    CHECK( RISCVInstr<uint32>(0xf95ff06f).get_disasm() == "jal $zero, -108");
    CHECK( RISCVInstr<uint32>(0x30529073).get_disasm() == "csrrw $mtvec, $zero, $t0");
    CHECK( RISCVInstr<uint32>(0x10200073).get_disasm() == "sret");
    CHECK( RISCVInstr<uint32>(0x30200073).get_disasm() == "mret");
    CHECK( RISCVInstr<uint32>(0x30202373).get_disasm() == "csrrs $medeleg, $t1, $zero");
    CHECK( RISCVInstr<uint32>(0x30205073).get_disasm() == "csrrwi $medeleg, $zero, 0x0");
    CHECK( RISCVInstr<uint32>(0x00004082).get_disasm() == "c_lwsp $ra, 0x0($sp)");
    CHECK( RISCVInstr<uint32>(0x0000df86).get_disasm() == "c_swsp $ra, 0xfc($sp)");
    CHECK( RISCVInstr<uint32>(0x00006082).get_disasm() == "c_ldsp $ra, 0x0($sp)");
    CHECK( RISCVInstr<uint32>(0x0000ff86).get_disasm() == "c_sdsp $ra, 0x1f8($sp)");
    CHECK( RISCVInstr<uint32>(0x00004110).get_disasm() == "c_lw $a2, 0x0($a0)");
    CHECK( RISCVInstr<uint32>(0x00006298).get_disasm() == "c_ld $a4, 0x0($a3)");
    CHECK( RISCVInstr<uint32>(0x0000b001).get_disasm() == "c_j -2048");
    CHECK( RISCVInstr<uint32>(0x00002ffd).get_disasm() == "c_jal 2046");
    CHECK( RISCVInstr<uint32>(0x00008882).get_disasm() == "c_jr $a7");
    CHECK( RISCVInstr<uint32>(0x00009582).get_disasm() == "c_jalr $a1");
    CHECK( RISCVInstr<uint32>(0x0000d281).get_disasm() == "c_beqz $a3, -256");
    CHECK( RISCVInstr<uint32>(0x0000effd).get_disasm() == "c_bnez $a5, 254");
    CHECK( RISCVInstr<uint32>(0x000048fd).get_disasm() == "c_li $a7, 31");
    CHECK( RISCVInstr<uint32>(0x00006405).get_disasm() == "c_lui $s0, 1");
    CHECK( RISCVInstr<uint32>(0x0000647d).get_disasm() == "c_lui $s0, 31");
    CHECK( RISCVInstr<uint32>(0x00007401).get_disasm() == "c_lui $s0, 0xfffe0");
    CHECK( RISCVInstr<uint32>(0x0000747d).get_disasm() == "c_lui $s0, 0xfffff");
    CHECK( RISCVInstr<uint32>(0x00001681).get_disasm() == "c_addi $a3, -32");
    CHECK( RISCVInstr<uint32>(0x00003681).get_disasm() == "c_addiw $a3, -32");
    CHECK( RISCVInstr<uint32>(0x00007101).get_disasm() == "c_addi16sp $sp, -512");
    CHECK( RISCVInstr<uint32>(0x0000617d).get_disasm() == "c_addi16sp $sp, 496");
    CHECK( RISCVInstr<uint32>(0x00001ff4).get_disasm() == "c_addi4spn $a3, $sp, 1020");
    CHECK( RISCVInstr<uint32>(0x00000054).get_disasm() == "c_addi4spn $a3, $sp, 4");
    CHECK( RISCVInstr<uint32>(0x00000586).get_disasm() == "c_slli $a1, 1");
    CHECK( RISCVInstr<uint32>(0x000082fd).get_disasm() == "c_srli $a3, 31");
    CHECK( RISCVInstr<uint32>(0x00008709).get_disasm() == "c_srai $a4, 2");
    CHECK( RISCVInstr<uint32>(0x00008bbd).get_disasm() == "c_andi $a5, 15");
    CHECK( RISCVInstr<uint32>(0x000088a2).get_disasm() == "c_mv $a7, $s0");
    CHECK( RISCVInstr<uint32>(0x00007d31).get_disasm() == "c_add $a0, $a2");
    CHECK( RISCVInstr<uint32>(0x00008df1).get_disasm() == "c_and $a1, $a2");
    CHECK( RISCVInstr<uint32>(0x00008ef5).get_disasm() == "c_or $a2, $a3");
    CHECK( RISCVInstr<uint32>(0x00008eb9).get_disasm() == "c_xor $a3, $a4");
    CHECK( RISCVInstr<uint32>(0x00008f1d).get_disasm() == "c_sub $a4, $a5");
    CHECK( RISCVInstr<uint32>(0x00009d31).get_disasm() == "c_addw $a0, $a2");
    CHECK( RISCVInstr<uint32>(0x00009e99).get_disasm() == "c_subw $a3, $a4");
    CHECK( RISCVInstr<uint32>(0x00009002).get_disasm() == "c_ebreak");
}

TEST_CASE("RISCV invalid instruction")
{
    CHECK( RISCVInstr<uint32>(0x0).get_disasm() == "unknown" );
    CHECK( RISCVInstr<uint32>("qwerty", 0x0).get_disasm() == "unknown" );
}

TEST_CASE("RISCV bytes dump")
{
    CHECK( RISCVInstr<uint32>(0x204002b7).bytes_dump() == "Bytes: 0xb7 0x02 0x40 0x20");
}

TEST_CASE("RISCV add")
{
    CHECK( RISCVInstr<uint32>(0x00b505b3).get_disasm() == "add $a1, $a0, $a1");
    RISCVInstr<uint32> instr( "add", 0);
    instr.set_v_src( 0x10, 0);
    instr.set_v_src( 0xf, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x1f);
}

TEST_CASE("RISCV lui 1")
{
    CHECK( RISCVInstr<uint32>(0x204002b7).get_disasm() == "lui $t0, 0x20400");

    RISCVInstr<uint32> instr("lui", 0x1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x1000);
}

TEST_CASE("RISCV lui all fs")
{
    RISCVInstr<uint32> instr("lui", 0xfffff);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffff'f000ULL);
}

TEST_CASE("RISCV lui 80000")
{
    RISCVInstr<uint64> instr("lui", 0x80000);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffff'ffff'8000'0000ULL);
}

TEST_CASE("RISCV-128 lui all fs")
{
    RISCVInstr<uint64> instr("lui", 0xfffff);
    instr.execute();
    CHECK( ~instr.get_v_dst() == 0xfff);
}

TEST_CASE("RISCV sub")
{
    CHECK( RISCVInstr<uint32>(0x40e787b3).get_disasm() == "sub $a5, $a5, $a4");
    RISCVInstr<uint32> instr( "sub", 0);
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
