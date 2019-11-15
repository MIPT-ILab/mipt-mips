/*
 * RISC-V instruction unit tests
 * Author pavel.kryukov@phystech.edu
 * Copyright 2019 MIPT-MIPS
 */

#include "../riscv_instr.h"
 
#include <catch.hpp>
#include <memory/memory.h>

TEST_CASE("RISCV disassembly")
{
    CHECK( RISCVInstr<uint32>(0x00000597).get_disasm() == "auipc $a1, 0x0" );
    CHECK( RISCVInstr<uint32>(0x00f70463).get_disasm() == "beq $a4, $a5, 8");
    CHECK( RISCVInstr<uint32>(0x00052783).get_disasm() == "lw $a5, 0x0($a0)");
    CHECK( RISCVInstr<uint32>(0x0020a023).get_disasm() == "sw $sp, 0x0($ra)");
    CHECK( RISCVInstr<uint32>(0xf95ff06f).get_disasm() == "jal $zero, -108");
    CHECK( RISCVInstr<uint32>(0x30529073).get_disasm() == "csrrw $mtvec, $zero, $t0");
    CHECK( RISCVInstr<uint32>(0x10200073).get_disasm() == "sret");
    CHECK( RISCVInstr<uint32>(0x30200073).get_disasm() == "mret");
    CHECK( RISCVInstr<uint32>(0x30202373).get_disasm() == "csrrs $medeleg, $t1, $zero");
    CHECK( RISCVInstr<uint32>(0x30205073).get_disasm() == "csrrwi $medeleg, $zero, 0x0");
    CHECK( RISCVInstr<uint32>    (0x4082).get_disasm() == "c_lwsp $ra, 0x0($sp)");
    CHECK( RISCVInstr<uint32>    (0xdf86).get_disasm() == "c_swsp $ra, 0xfc($sp)");
    CHECK( RISCVInstr<uint64>    (0x6082).get_disasm() == "c_ldsp $ra, 0x0($sp)");
    CHECK( RISCVInstr<uint64>    (0xff86).get_disasm() == "c_sdsp $ra, 0x1f8($sp)");
    CHECK( RISCVInstr<uint128>   (0x30fe).get_disasm() == "c_lqsp $ra, 0x3f0($sp)");
    CHECK( RISCVInstr<uint128>   (0xb686).get_disasm() == "c_sqsp $ra, 0x360($sp)");
    CHECK( RISCVInstr<uint32>    (0x4110).get_disasm() == "c_lw $a2, 0x0($a0)");
    CHECK( RISCVInstr<uint64>    (0x6298).get_disasm() == "c_ld $a4, 0x0($a3)");
    CHECK( RISCVInstr<uint64>    (0xfefc).get_disasm() == "c_sd $a5, 0xf8($a3)");
    CHECK( RISCVInstr<uint32>    (0xdefc).get_disasm() == "c_sw $a5, 0x7c($a3)");
    CHECK( RISCVInstr<uint128>   (0x36d8).get_disasm() == "c_lq $a4, 0x1a0($a3)");
    CHECK( RISCVInstr<uint128>   (0xaebc).get_disasm() == "c_sq $a5, 0x150($a3)");
    CHECK( RISCVInstr<uint32>    (0xb001).get_disasm() == "c_j -2048");
    CHECK( RISCVInstr<uint32>    (0x2ffd).get_disasm() == "c_jal 2046");
    CHECK( RISCVInstr<uint32>    (0x8882).get_disasm() == "c_jr $a7");
    CHECK( RISCVInstr<uint32>    (0x9582).get_disasm() == "c_jalr $a1");
    CHECK( RISCVInstr<uint32>    (0xd281).get_disasm() == "c_beqz $a3, -256");
    CHECK( RISCVInstr<uint32>    (0xeffd).get_disasm() == "c_bnez $a5, 254");
    CHECK( RISCVInstr<uint32>    (0x48fd).get_disasm() == "c_li $a7, 31");
    CHECK( RISCVInstr<uint32>    (0x6405).get_disasm() == "c_lui $s0, 0x1");
    CHECK( RISCVInstr<uint32>    (0x647d).get_disasm() == "c_lui $s0, 0x1f");
    CHECK( RISCVInstr<uint32>    (0x7401).get_disasm() == "c_lui $s0, 0xffffffe0");
    CHECK( RISCVInstr<uint32>    (0x747d).get_disasm() == "c_lui $s0, 0xffffffff");
    CHECK( RISCVInstr<uint32>    (0x1681).get_disasm() == "c_addi $a3, -32");
    CHECK( RISCVInstr<uint64>    (0x3681).get_disasm() == "c_addiw $a3, -32");
    CHECK( RISCVInstr<uint32>    (0x7101).get_disasm() == "c_addi16sp $sp, -512");
    CHECK( RISCVInstr<uint32>    (0x617d).get_disasm() == "c_addi16sp $sp, 496");
    CHECK( RISCVInstr<uint32>    (0x1ff4).get_disasm() == "c_addi4spn $a3, $sp, 1020");
    CHECK( RISCVInstr<uint32>    (0x0054).get_disasm() == "c_addi4spn $a3, $sp, 4");
    CHECK( RISCVInstr<uint32>    (0x0586).get_disasm() == "c_slli $a1, 1");
    CHECK( RISCVInstr<uint32>    (0x82fd).get_disasm() == "c_srli $a3, 31");
    CHECK( RISCVInstr<uint32>    (0x8709).get_disasm() == "c_srai $a4, 2");
    CHECK( RISCVInstr<uint32>    (0x8bbd).get_disasm() == "c_andi $a5, 15");
    CHECK( RISCVInstr<uint32>    (0x88a2).get_disasm() == "c_mv $a7, $s0");
    CHECK( RISCVInstr<uint32>    (0x8df1).get_disasm() == "c_and $a1, $a2");
    CHECK( RISCVInstr<uint32>    (0x8e55).get_disasm() == "c_or $a2, $a3");
    CHECK( RISCVInstr<uint32>    (0x8eb9).get_disasm() == "c_xor $a3, $a4");
    CHECK( RISCVInstr<uint32>    (0x8f1d).get_disasm() == "c_sub $a4, $a5");
    CHECK( RISCVInstr<uint64>    (0x9d31).get_disasm() == "c_addw $a0, $a2");
    CHECK( RISCVInstr<uint64>    (0x9e99).get_disasm() == "c_subw $a3, $a4");
    CHECK( RISCVInstr<uint32>    (0x9002).get_disasm() == "c_ebreak");
    CHECK( RISCVInstr<uint32>    (0x0001).get_disasm() == "c_nop");
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

    std::ostringstream oss;
    oss << instr;
    CHECK( oss.str() == "{80}\tsub $a5, $a5, $a4\t [ $a5 = 0x1 ]");
}

TEST_CASE( "RISCV lq/sq")
{
    RISCVInstr<uint128> load( "c_lq", 0x1008);
    RISCVInstr<uint128> store( "c_sq", 0x1000);
    store.set_v_src( bitmask<uint128>( 68), 1);
    auto memory = FuncMemory::create_default_hierarchied_memory();
    for (auto* instr : { &load, &store }) {
        instr->set_v_src( 0x1000, 0);
        instr->execute();
    }
    memory->load_store( &store);
    memory->load_store( &load);
    CHECK( narrow_cast<uint64>( load.get_v_dst()) == 0xf);
}

template<typename T>
struct TestData {
    T src1, src2,dst;
};

TEST_CASE( "RISCV slo32")
{
    CHECK( RISCVInstr<uint32>( 0x20E797B3).get_disasm() == "slo $a5, $a5, $a4");
    std::vector<TestData<uint32>> cases = {
        {0x1C, 2, 0x73},
        {all_ones<uint32>(), 0xAA, all_ones<uint32>()},
        {0xAA, 0xFF, bitmask<uint32>(31)},
        {0xAB, 0xFF, all_ones<uint32>()},
    };
    for (std::size_t i = 0; i < cases.size(); i++) {
        RISCVInstr<uint32> instr( "slo", 0);
        instr.set_v_src( cases[i].src1, 0);
        instr.set_v_src( cases[i].src2, 1);
        instr.execute();
        INFO( "Iteration: " << i);
        CHECK( instr.get_v_dst() == cases[i].dst);
    }
}

TEST_CASE ("RISCV slo64") {
    std::vector<TestData<uint64>> cases = {
        {0x1C, 2, 0x73},
        {all_ones<uint32>(), 5, bitmask<uint64>(37)},
        {all_ones<uint64>(), 0xFF, all_ones<uint64>()},
        {0xAA, 0xFF, bitmask<uint64>(63)},
        {0xAB, 0xFF, all_ones<uint64>()},
    };
    for (std::size_t i = 0; i < cases.size(); i++) {
        RISCVInstr<uint64> instr( "slo", 0);
        instr.set_v_src( cases[i].src1, 0);
        instr.set_v_src( cases[i].src2, 1);
        instr.execute();
        INFO( "Iteration: " << i);
        CHECK( instr.get_v_dst() == cases[i].dst);
    }
}
