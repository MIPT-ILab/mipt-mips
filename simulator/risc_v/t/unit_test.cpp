/*
 * RISC-V instruction unit tests
 * Author pavel.kryukov@phystech.edu
 * Copyright 2019 MIPT-MIPS
 */

#include "riscv_test_wrapper.h"

#include <catch.hpp>
#include <memory/memory.h>
#include <risc_v/riscv_instr.h>

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

TEST_CASE("RISCV srai disassembly")
{
    CHECK( RISCVInstr<uint32>( 0x4028d713).get_disasm()  == "srai $a4, $a7, 2");
    CHECK( RISCVInstr<uint64>( 0x4028d713).get_disasm()  == "srai $a4, $a7, 2");
    CHECK( RISCVInstr<uint64>( 0x4070df1b).get_disasm()  == "sraiw $t5, $ra, 7");
    CHECK( RISCVInstr<uint128>( 0x4070df5b).get_disasm() == "sraid $t5, $ra, 7");
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

TEST_CASE( "RISCV slo32")
{
    CHECK( RISCVInstr<uint32>( 0x20E797B3).get_disasm() == "slo $a5, $a5, $a4");
    std::vector<TestData<uint32>> cases = {
        TestData<uint32>( 0x1C, 2, 0x73),
        TestData<uint32>( all_ones<uint32>(), 0xAA, all_ones<uint32>()),
        TestData<uint32>( 0xAA, 0xFF, bitmask<uint32>(31)),
        TestData<uint32>( 0xAB, 0xFF, all_ones<uint32>()),
    };
    for (std::size_t i = 0; i < cases.size(); i++) {
        INFO( "Iteration: " << i);
        cases[i].make_test("slo");
    }
}

TEST_CASE ( "RISCV slo64")
{
    std::vector<TestData<uint64>> cases = {
        TestData<uint64>( 0x1C, 2, 0x73),
        TestData<uint64>( all_ones<uint32>(), 5, bitmask<uint64>(37)),
        TestData<uint64>( all_ones<uint64>(), 0xFF, all_ones<uint64>()),
        TestData<uint64>( 0xAA, 0xFF, bitmask<uint64>(63)),
        TestData<uint64>( 0xAB, 0xFF, all_ones<uint64>()),
    };
    for (std::size_t i = 0; i < cases.size(); i++) {
        INFO( "Iteration: " << i);
        cases[i].make_test("slo");
    }
}

TEST_CASE("RISCV RV32 orn")
{
    CHECK( RISCVInstr<uint32>(0x411865b3).get_disasm() == "orn $a1, $a6, $a7");
    RISCVInstr<uint32> instr( "orn", 0);
    instr.set_v_src( 0xf7, 0);
    instr.set_v_src( 0xffffef77, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x10ff);
}

TEST_CASE("RISCV RV64 orn")
{
    RISCVInstr<uint64> instr( "orn", 0);
    instr.set_v_src( 0xf7, 0);
    instr.set_v_src( 0xbfff'ffff'ffff'ef77, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x4000'0000'0000'10ff);
}

TEST_CASE ("RISCV sbext32")
{
    CHECK( RISCVInstr<uint32>( 0x48e7d7b3).get_disasm() == "sbext $a5, $a5, $a4");
    std::vector<TestData<uint32>> cases {
        TestData<uint32>( 0xf, 1, 0x1),
        TestData<uint32>( all_ones<uint32>(), 31, 0x1),
        TestData<uint32>( 0x6eda, 4, 0x1),
        TestData<uint32>( 0x6eca, 4, 0x0),
        TestData<uint32>( 0xD00, 8, 0x1),
    };
    for (std::size_t i = 0; i < cases.size(); i++) {
        INFO( "Iteration: " << i);
        cases[i].make_test("sbext");
    }
}

TEST_CASE ("RISCV sbext64")
{
    CHECK( RISCVInstr<uint64>( 0x48e7d7b3).get_disasm() == "sbext $a5, $a5, $a4");
    std::vector<TestData<uint64>> cases {
        TestData<uint64>( 0xf, 1, 0x1),
        TestData<uint64>( all_ones<uint64>(), 56, 0x1),
        TestData<uint64>( 0x6eda, 4, 0x1),
        TestData<uint64>( 0x6eca, 4, 0x0),
        TestData<uint64>( 0xD00, 8, 0x1),
    };
    for (std::size_t i = 0; i < cases.size(); i++) {
        INFO( "Iteration: " << i);
        cases[i].make_test("sbext");
    }
}

TEST_CASE( "RISV RV32 pack")
{
    CHECK( RISCVInstr<uint32>(0x44d60533).get_disasm() == "pack $a0, $a2, $a3");
    RISCVInstr<uint32> instr( "pack", 0);
    instr.set_v_src( 0xffff'2222, 0);
    instr.set_v_src( 0x1111'3333, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x3333'2222);
}

TEST_CASE( "RISV RV64 pack")
{
    RISCVInstr<uint64> instr( "pack", 0);
    instr.set_v_src( 0xffff'ffff'2222'2222, 0);
    instr.set_v_src( 0x1111'1111'3333'3333, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x3333'3333'2222'2222);
}

TEST_CASE( "RISV RV32 xnor")
{
    CHECK( RISCVInstr<uint32>(0x40e6c633).get_disasm() == "xnor $a2, $a3, $a4");
    RISCVInstr<uint32> instr ( "xnor", 0);
    instr.set_v_src( 0x3000'0000, 0);
    instr.set_v_src( 0xa000'0000, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x6fff'ffff);
}

TEST_CASE( "RISV RV64 xnor")
{
    RISCVInstr<uint64> instr ( "xnor", 0);
    instr.set_v_src( 0x3000'0000'3000'0000, 0);
    instr.set_v_src( 0xa000'0000'a000'0000, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x6fff'ffff'6fff'ffff);
}

TEST_CASE( "RISCV sro32")
{
    CHECK( RISCVInstr<uint32>( 0x20d65733).get_disasm() == "sro $a4, $a2, $a3");
    RISCVInstr<uint32> instr( "sro", 0);
    instr.set_v_src( 0x8000'c000U, 0);
    instr.set_v_src( 0xf, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffff'0001);
}

TEST_CASE( "RISCV sro32 overflow")
{
    RISCVInstr<uint32> instr( "sro", 0);
    instr.set_v_src( 0x8000'c000U, 0);
    instr.set_v_src( 0xffU, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffff'ffffU);
}

TEST_CASE( "RISCV sro64")
{
    RISCVInstr<uint64> instr( "sro", 0);
    instr.set_v_src( 0x8000'0000'c000'0000ULL, 0);
    instr.set_v_src( 0x1fU, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffff'ffff'0000'0001ULL);
}

TEST_CASE( "RISCV sro64 overflow")
{
    RISCVInstr<uint64> instr( "sro", 0);
    instr.set_v_src( 0x8000'0000'c000'0000ULL, 0);
    instr.set_v_src( 0xffU, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xffff'ffff'ffff'ffffULL);
}

TEST_CASE("RISCV RV32 bfp")
{
    CHECK( RISCVInstr<uint32>(0x08F77833).get_disasm() == "bfp $a6, $a4, $a5");
    RISCVInstr<uint32> instr( "bfp", 0);
    instr.set_v_src( 0x5555, 0);
    instr.set_v_src( 0x080400C0, 1); //len = 8, off = 4, data = 1100'0000
    instr.execute();
    CHECK( instr.get_v_dst() == 0x5C05);
}

TEST_CASE("RISCV RV64 bfp")
{
    RISCVInstr<uint64> instr( "bfp", 0);
    instr.set_v_src( 0x0000'5555'CCCC'0000, 0);
    instr.set_v_src( 0x081C00C5, 1); //len = 8, off = 28, data = 1100'0101
    instr.execute();
    CHECK( instr.get_v_dst() == 0x0000'555C'5CCC'0000);
}

TEST_CASE("RISCV RV32 grev")
{
    CHECK( RISCVInstr<uint32>(0x68D6'55B3).get_disasm() == "grev $a1, $a2, $a3");
    std::vector<TestData<uint32>> cases = {
            {0x5555'5555,  0x1, 0xAAAA'AAAA},
            {0x3333'3333,  0x2, 0xCCCC'CCCC},
            {0x1111'1111,  0x3, 0x8888'8888},
            {0x0123'4567,  0x7, 0x80C4'A2E6},
            {0x3333'3333,  0xF, 0xCCCC'CCCC},
            {0x0123'4567, 0x1F, 0xE6A2'C480},
    };
    for (std::size_t i = 0; i < cases.size(); ++i) {
        RISCVInstr<uint32> instr( "grev", 0);
        instr.set_v_src( cases[i].src1, 0);
        instr.set_v_src( cases[i].src2, 1);
        instr.execute();
        INFO( "Iteration: " << i);
        CHECK( instr.get_v_dst() == cases[i].dst);
    }
}

TEST_CASE("RISCV RV64 grev")
{
    std::vector<TestData<uint64>> cases = {
            {0x5555'5555'5555'5555ULL,  0x1, 0xAAAA'AAAA'AAAA'AAAAULL},
            {0x3333'3333'3333'3333ULL,  0x2, 0xCCCC'CCCC'CCCC'CCCCULL},
            {0x1111'1111'1111'1111ULL,  0x3, 0x8888'8888'8888'8888ULL},
            {0x0123'4567'89AB'CDEFULL,  0x7, 0x80C4'A2E6'91D5'B3F7ULL},
            {0x0123'4567'89AB'CDEFULL,  0xF, 0xC480'E6A2'D591'F7B3ULL},
            {0x0123'4567'89AB'CDEFULL, 0x1F, 0xE6A2'C480'F7B3'D591ULL},
            {0x0123'4567'89AB'CDEFULL, 0x3F, 0xF7B3'D591'E6A2'C480ULL},
    };
    for (std::size_t i = 0; i < cases.size(); ++i) {
        RISCVInstr<uint64> instr( "grev", 0);
        instr.set_v_src( cases[i].src1, 0);
        instr.set_v_src( cases[i].src2, 1);
        instr.execute();
        INFO( "Iteration: " << i);
        CHECK( instr.get_v_dst() == cases[i].dst);
    }
}

TEST_CASE("RISCV RV32 pcnt")
{
    CHECK( RISCVInstr<uint32>(0x60281593).get_disasm() == "pcnt $a1, $a6, $sp");
    RISCVInstr<uint32> instr( "pcnt", 0);
    instr.set_v_src( 0xfff, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xc);
}

TEST_CASE("RISCV RV64 pcnt")
{
    RISCVInstr<uint64> instr( "pcnt", 0);
    instr.set_v_src( 0xfafb'fcfd'feff, 0);
    instr.execute();
    CHECK( instr.get_v_dst() == 41);
}

TEST_CASE("RISCV RV128 pcnt")
{
    uint128 value = 0xfafb'fcfd'feffULL;
    value = (value << 64) | value;
    RISCVInstr<uint128> instr( "pcnt", 0);
    instr.set_v_src( value, 0);
    instr.execute();
    CHECK( narrow_cast<size_t>( instr.get_v_dst()) == 82);
}

TEST_CASE ("RISCV RV32 clz")
{
    CHECK ( RISCVInstr<uint32>(0x60079793).get_disasm() == "clz $a5, $a5");
    std::vector<TestData<uint32>> cases = {
        TestData<uint32>( 0 , 0 , 32),
        TestData<uint32>( all_ones<uint32>(), 0, 0),
        TestData<uint32>( 0x0000ffff, 0 , 16),
    };
    for(std::size_t i = 0; i < cases.size(); i++) {
        INFO( "Iteration: " << i);
        cases[i].make_test("clz");
    }
}


TEST_CASE ("RISCV RV64 clz")
{
    std::vector<TestData<uint64>> cases = {
        TestData<uint64>( 0 , 0 , 64),
        TestData<uint64>( all_ones<uint64>(), 0, 0),
        TestData<uint64>( 0x0000000ffffffff, 0 , 32),
    };
    for(std::size_t i = 0; i < cases.size(); i++) {
        INFO( "Iteration: " << i);
        cases[i].make_test("clz");
    }
}

TEST_CASE ("RISCV RV32 ctz")
{
    CHECK ( RISCVInstr<uint32>(0x60179793).get_disasm() == "ctz $a5, $a5");
    std::vector<TestData<uint32>> cases = {
        TestData<uint32>( 0 , 0 , 32),
        TestData<uint32>( all_ones<uint32>(), 0, 0),
        TestData<uint32>( 0xffff0000, 0 , 16),
    };
    for(std::size_t i = 0; i < cases.size(); i++) {
        INFO( "Iteration: " << i);
        cases[i].make_test("ctz");
    }
}


TEST_CASE ("RISCV RV64 ctz")
{
    std::vector<TestData<uint64>> cases = {
        TestData<uint64>( 0 , 0 , 64),
        TestData<uint64>( all_ones<uint64>(), 0, 0),
        TestData<uint64>( 0xffffffff00000000, 0 , 32),
    };
    for(std::size_t i = 0; i < cases.size(); i++) {
        INFO( "Iteration: " << i);
        cases[i].make_test("ctz");
    }
}

TEST_CASE("RISCV RV32 rol")
{
    CHECK( RISCVInstr<uint32>( 0x60E797B3).get_disasm() == "rol $a5, $a5, $a4");
    std::vector<TestData<uint32>> cases {
        TestData<uint32>( 0x01234567, 0,  0x01234567),
        TestData<uint32>( 0x01234567, 4,  0x12345670),
        TestData<uint32>( 0x01234567, 8,  0x23456701),
	TestData<uint32>( 0x01234567, 32, 0x01234567),
    };
    for (std::size_t i = 0; i < cases.size(); i++) {
        INFO( "Iteration: " << i);
        cases[i].make_test("rol");
    }
}

TEST_CASE("RISCV RV64 rol")
{
    CHECK( RISCVInstr<uint64>( 0x60E797B3).get_disasm() == "rol $a5, $a5, $a4");
    std::vector<TestData<uint64>> cases {
        TestData<uint64>( 0x0123'4567'89ab'cdef, 0,  0x0123'4567'89ab'cdef),
        TestData<uint64>( 0x0123'4567'89ab'cdef, 4,  0x1234'5678'9abc'def0),
        TestData<uint64>( 0x0123'4567'89ab'cdef, 8,  0x2345'6789'abcd'ef01),
        TestData<uint64>( 0x0123'4567'89ab'cdef, 64, 0x0123'4567'89ab'cdef),
    };
    for (std::size_t i = 0; i < cases.size(); i++) {
        INFO( "Iteration: " << i);
        cases[i].make_test("rol");
    }
}

TEST_CASE("RISCV RV32 clmul")
{
    std::vector<TestData<uint32>> cases {
        TestData<uint32>( 0, 0, 0),
        TestData<uint32>( 1, 0, 0),
        TestData<uint32>( 0, 1, 0),
        TestData<uint32>( 1, 1, 1),
        TestData<uint32>( all_ones<uint32>(), 0, 0),
        TestData<uint32>( all_ones<uint32>(), 1, all_ones<uint32>()),
        TestData<uint32>( 0, all_ones<uint32>(), 0),
        TestData<uint32>( 0xA2, 0x96, 0x58EC),
        TestData<uint32>( 0xA328B534, 0x923ACD6E, 0xDA9AB898),
        TestData<uint32>( 0xABCDEF45, 0xEEE111BB, 0x3093E097),
    };
    for (std::size_t i = 0; i < cases.size(); i++) {
        INFO( "Iteration: " << i);
        cases[i].make_test("clmul");
    }
}

TEST_CASE("RISCV RV64 clmul")
{
    std::vector<TestData<uint64>> cases {
        TestData<uint64>( 0, 0, 0),
        TestData<uint64>( 1, 0, 0),
        TestData<uint64>( 0, 1, 0),
        TestData<uint64>( 1, 1, 1),
        TestData<uint64>( 0xA2, 0x96, 0x58EC),
        TestData<uint64>( all_ones<uint64>(), 0, 0),
        TestData<uint64>( all_ones<uint64>(), 1, all_ones<uint64>()),
        TestData<uint64>( 0, all_ones<uint64>(), 0),
        TestData<uint64>( 0xEEE111BBABCDEF45, 0xEEE1CC34FFAA11BB, 0xA18865DFBF34E097),
        TestData<uint64>( 0xEE3333333BCDEF45, 0x0055552222000000, 0x271E511C2A000000),
    };
    for (std::size_t i = 0; i < cases.size(); i++) {
        INFO( "Iteration: " << i);
        cases[i].make_test("clmul");
    }
}

TEST_CASE("RISCV RV32 gorc")
{
    CHECK( RISCVInstr<uint32>(0x28D6'55B3).get_disasm() == "gorc $a1, $a2, $a3");
    std::vector<TestData<uint32>> cases = {
            {0x1111'0000,  0x1, 0x3333'0000},
            {0x0022'0033,  0x2, 0x00AA'00FF},
            {0x0C0C'0F0F,  0x5, 0xCCCC'FFFF},
            {0x8712'4789,  0xC, all_ones<uint32>()},
            {1,  0xFF, all_ones<uint32>()},
            {0x1252'0391,  0x3, 0xFFFF'0FFF},
    };
    for(std::size_t i = 0; i < cases.size(); i++) {
        INFO( "Iteration: " << i);
        cases[i].make_test("gorc");
    }
}

TEST_CASE("RISCV RV64 gorc")
{
    std::vector<TestData<uint64>> cases = {
            {0x1111'1111'1111'1111ULL,  0x1, 0x3333'3333'3333'3333ULL},
            {0x0022'0033'0044'0055ULL,  0x2, 0x00AA'00FF'0055'0055ULL},
            {0x0C0C'0F0F'0A0A'0B0BULL,  0x5, 0xCCCC'FFFF'FFFF'FFFFULL},
            {0x8712'4789'1244'1231ULL,  0xC, 0xFFFF'FFFF'7777'3333ULL},
            {1,  0xFF, all_ones<uint64>()},
            {0x1252'0391'1234'0987ULL,  0x3, 0xFFFF'0FFF'FFFF'0FFFULL},
    };
    for(std::size_t i = 0; i < cases.size(); i++) {
        INFO( "Iteration: " << i);
        cases[i].make_test("gorc");
    }
}

TEST_CASE("RISCV RV32 unshfl")
{
    CHECK( RISCVInstr<uint32>(0x091855b3).get_disasm() == "unshfl $a1, $a6, $a7");
    RISCVInstr<uint32> instr("unshfl", 0);
    instr.set_v_src( 0xbf534fdeU, 0);
    instr.set_v_src( 0x07, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0xf17d3bbeU);
}

TEST_CASE("RISCV RV64 unshfl")
{
    RISCVInstr<uint64> instr("unshfl", 0);
    instr.set_v_src( 0x0138'745b'ffff'acd1ULL, 0);
    instr.set_v_src( 0x16, 1);
    instr.execute();
    CHECK( instr.get_v_dst() == 0x021c'ffff'56c7'bc85ULL);
}

TEST_CASE("RISCV RV128 unshfl")
{
    std::vector<TestData<uint128>> cases = {
            { 0x3146'2351'2464'6245ULL,  0x11, 0x5126'4262'4531'6423ULL},
            { 0xf14c'82a6'ac81'2410ULL,  0x36, ( uint128{ 0xc7d0b881U} << 64) + uint128{ 0x892a1084U}},
            { 0x1234'9abc'5678'def0ULL,  0x0, 0x1234'9abc'5678'def0ULL},
            { ( uint128{ 0x7fff'0fe3'3630'0001ULL} << 64) + uint128{ 0x7f3d'ff39'5400'6401ULL},  0x0f,
                ( uint128{ 0x7f3d'ff39'5400'6401ULL} << 64) + uint128{ 0x76f6'f7f5'0040'e0a1ULL}},
            { ( uint128{ 0x9846'5982'6593'659fULL} << 64) + uint128{ 0x8534'9dab'c820'f892ULL},  0x3f,
                ( uint128{ 0xa129'494b'84af'a4e9ULL} << 64) + uint128{ 0x4ad0'b5b7'3671'80c4ULL}},
    };
    for( std::size_t i = 0; i < cases.size(); i++) {
        INFO( "Iteration: " << i);
        cases[i].make_test( "unshfl");
    }
}

TEST_CASE("RISCV ecall")
{
    RISCVInstr<uint32> instr( "ecall", 0);
    instr.execute();
    CHECK( instr.trap_type() == Trap::SYSCALL);
}
