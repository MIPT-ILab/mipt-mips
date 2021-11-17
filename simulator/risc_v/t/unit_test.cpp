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
    TEST_RV32_DISASM  ( 0x00000597, "auipc $a1, 0x0");
    TEST_RV32_DISASM  ( 0x00f70463, "beq $a4, $a5, 8");
    TEST_RV32_DISASM  ( 0x00052783, "lw $a5, 0x0($a0)");
    TEST_RV32_DISASM  ( 0x0020a023, "sw $sp, 0x0($ra)");
    TEST_RV32_DISASM  ( 0xf95ff06f, "jal $zero, -108");
    TEST_RV32_DISASM  ( 0x30529073, "csrrw $mtvec, $zero, $t0");
    TEST_RV32_DISASM  ( 0x10200073, "sret");
    TEST_RV32_DISASM  ( 0x30200073, "mret");
    TEST_RV32_DISASM  ( 0x30202373, "csrrs $medeleg, $t1, $zero");
    TEST_RV32_DISASM  ( 0x30205073, "csrrwi $medeleg, $zero, 0x0");
    TEST_RV32_DISASM  ( 0x4082,     "c_lwsp $ra, 0x0($sp)");
    TEST_RV32_DISASM  ( 0xdf86,     "c_swsp $ra, 0xfc($sp)");
    TEST_RV64_DISASM  ( 0x6082,     "c_ldsp $ra, 0x0($sp)");
    TEST_RV64_DISASM  ( 0xff86,     "c_sdsp $ra, 0x1f8($sp)");
    TEST_RV128_DISASM ( 0x30fe,     "c_lqsp $ra, 0x3f0($sp)");
    TEST_RV128_DISASM ( 0xb686,     "c_sqsp $ra, 0x360($sp)");
    TEST_RV32_DISASM  ( 0x4110,     "c_lw $a2, 0x0($a0)");
    TEST_RV64_DISASM  ( 0x6298,     "c_ld $a4, 0x0($a3)");
    TEST_RV64_DISASM  ( 0xfefc,     "c_sd $a5, 0xf8($a3)");
    TEST_RV32_DISASM  ( 0xdefc,     "c_sw $a5, 0x7c($a3)");
    TEST_RV128_DISASM ( 0x36d8,     "c_lq $a4, 0x1a0($a3)");
    TEST_RV128_DISASM ( 0xaebc,     "c_sq $a5, 0x150($a3)");
    TEST_RV32_DISASM  ( 0xb001,     "c_j -2048");
    TEST_RV32_DISASM  ( 0x2ffd,     "c_jal 2046");
    TEST_RV32_DISASM  ( 0x8882,     "c_jr $a7");
    TEST_RV32_DISASM  ( 0x9582,     "c_jalr $a1");
    TEST_RV32_DISASM  ( 0xd281,     "c_beqz $a3, -256");
    TEST_RV32_DISASM  ( 0xeffd,     "c_bnez $a5, 254");
    TEST_RV32_DISASM  ( 0x48fd,     "c_li $a7, 31");
    TEST_RV32_DISASM  ( 0x6405,     "c_lui $s0, 0x1");
    TEST_RV32_DISASM  ( 0x647d,     "c_lui $s0, 0x1f");
    TEST_RV32_DISASM  ( 0x7401,     "c_lui $s0, 0xffffffe0");
    TEST_RV32_DISASM  ( 0x747d,     "c_lui $s0, 0xffffffff");
    TEST_RV32_DISASM  ( 0x1681,     "c_addi $a3, -32");
    TEST_RV64_DISASM  ( 0x3681,     "c_addiw $a3, -32");
    TEST_RV32_DISASM  ( 0x7101,     "c_addi16sp $sp, -512");
    TEST_RV32_DISASM  ( 0x617d,     "c_addi16sp $sp, 496");
    TEST_RV32_DISASM  ( 0x1ff4,     "c_addi4spn $a3, $sp, 1020");
    TEST_RV32_DISASM  ( 0x0054,     "c_addi4spn $a3, $sp, 4");
    TEST_RV32_DISASM  ( 0x0586,     "c_slli $a1, 1");
    TEST_RV32_DISASM  ( 0x82fd,     "c_srli $a3, 31");
    TEST_RV32_DISASM  ( 0x8709,     "c_srai $a4, 2");
    TEST_RV32_DISASM  ( 0x8bbd,     "c_andi $a5, 15");
    TEST_RV32_DISASM  ( 0x88a2,     "c_mv $a7, $s0");
    TEST_RV32_DISASM  ( 0x8df1,     "c_and $a1, $a2");
    TEST_RV32_DISASM  ( 0x8e55,     "c_or $a2, $a3");
    TEST_RV32_DISASM  ( 0x8eb9,     "c_xor $a3, $a4");
    TEST_RV32_DISASM  ( 0x8f1d,     "c_sub $a4, $a5");
    TEST_RV64_DISASM  ( 0x9d31,     "c_addw $a0, $a2");
    TEST_RV64_DISASM  ( 0x9e99,     "c_subw $a3, $a4");
    TEST_RV32_DISASM  ( 0x9002,     "c_ebreak");
    TEST_RV32_DISASM  ( 0x0001,     "c_nop");
    TEST_RV32_DISASM  ( 0x4028d713, "srai $a4, $a7, 2");
    TEST_RV64_DISASM  ( 0x4028d713, "srai $a4, $a7, 2");
    TEST_RV64_DISASM  ( 0x4070df1b, "sraiw $t5, $ra, 7");
    TEST_RV128_DISASM ( 0x4070df5b, "sraid $t5, $ra, 7");    
    TEST_RV32_DISASM  ( 0x00b505b3, "add $a1, $a0, $a1");
    TEST_RV32_DISASM  ( 0x204002b7, "lui $t0, 0x20400");
    TEST_RV32_DISASM  ( 0x40e787b3, "sub $a5, $a5, $a4");
    TEST_RV32_DISASM  ( 0x20E797B3, "slo $a5, $a5, $a4");
    TEST_RV32_DISASM  ( 0x20E79793, "sloi $a5, $a5, 14");
    TEST_RV32_DISASM  ( 0x411865b3, "orn $a1, $a6, $a7");
    TEST_RV32_DISASM  ( 0x68e797b3, "binv $a5, $a5, $a4");
    TEST_RV32_DISASM  ( 0x4028d713, "srai $a4, $a7, 2");
    TEST_RV64_DISASM  ( 0x4028d713, "srai $a4, $a7, 2");
    TEST_RV64_DISASM  ( 0x4070df1b, "sraiw $t5, $ra, 7");
    TEST_RV128_DISASM ( 0x4070df5b, "sraid $t5, $ra, 7");    
    TEST_RV32_DISASM  ( 0x00b505b3, "add $a1, $a0, $a1");
    TEST_RV32_DISASM  ( 0x204002b7, "lui $t0, 0x20400");
    TEST_RV32_DISASM  ( 0x40e787b3, "sub $a5, $a5, $a4");
    TEST_RV32_DISASM  ( 0x20E797B3, "slo $a5, $a5, $a4");
    TEST_RV32_DISASM  ( 0x20E79793, "sloi $a5, $a5, 14");
    TEST_RV32_DISASM  ( 0x411865b3, "orn $a1, $a6, $a7");
    TEST_RV32_DISASM  ( 0x68e797b3, "binv $a5, $a5, $a4");        
    TEST_RV32_DISASM  ( 0x68e797b3, "binv $a5, $a5, $a4");
    TEST_RV32_DISASM  ( 0x48e7d7b3, "bext $a5, $a5, $a4");
    TEST_RV64_DISASM  ( 0x48e7d7b3, "bext $a5, $a5, $a4");
    TEST_RV32_DISASM  ( 0x08d64533, "pack $a0, $a2, $a3");
    TEST_RV32_DISASM  ( 0x40e6c633, "xnor $a2, $a3, $a4");
    TEST_RV32_DISASM  ( 0xAE6E633,  "max $a2, $a3, $a4");
    TEST_RV32_DISASM  ( 0x0ae7f7b3, "maxu $a5, $a5, $a4");
    TEST_RV32_DISASM  ( 0x20d65733, "sro $a4, $a2, $a3");
    TEST_RV32_DISASM  ( 0x20D65593, "sroi $a1, $a2, 13");
    TEST_RV32_DISASM  ( 0x48F77833, "bfp $a6, $a4, $a5");
    TEST_RV32_DISASM  ( 0x68D655B3, "grev $a1, $a2, $a3");
    TEST_RV32_DISASM  ( 0x60281593, "cpop $a1, $a6, $sp");
    TEST_RV32_DISASM  ( 0x60079793, "clz $a5, $a5");
    TEST_RV32_DISASM  ( 0x60179793, "ctz $a5, $a5");
    TEST_RV32_DISASM  ( 0x60E797B3, "rol $a5, $a5, $a4");
    TEST_RV64_DISASM  ( 0x60E797B3, "rol $a5, $a5, $a4");
    TEST_RV32_DISASM  ( 0x60E7D7B3, "ror $a5, $a5, $a4");
    TEST_RV32_DISASM  ( 0x60E7D7B3, "ror $a5, $a5, $a4");
    TEST_RV32_DISASM  ( 0x6007D793, "rori $a5, $a5, 0");  // 01100 | 0000000 (0)  | 01111 ($a5) | 101 | 01111 ($a5) | 0010011
    TEST_RV32_DISASM  ( 0x60F7D793, "rori $a5, $a5, 15"); // 01100 | 0001111 (15) | 01111 ($a5) | 101 | 01111 ($a5) | 0010011
    TEST_RV32_DISASM  ( 0x61F7D793, "rori $a5, $a5, 31"); // 01100 | 0011111 (31) | 01111 ($a5) | 101 | 01111 ($a5) | 0010011
    TEST_RV32_DISASM  ( 0x65F7D793, "rori $a5, $a5, 31"); // this test should not be passed because shamt[5] bit is 1 (for more info see specification for rori)
                                                          // issue is #1507
    TEST_RV64_DISASM  ( 0x63F7D793, "rori $a5, $a5, 63"); // 01100 | 0111111 (63) | 01111 ($a5) | 101 | 01111 ($a5) | 0010011
    TEST_RV32_DISASM  ( 0x28D655B3, "gorc $a1, $a2, $a3");
    TEST_RV32_DISASM  ( 0x484F9D93, "bclri $s11, $t6, 4");
    TEST_RV64_DISASM  ( 0x4A4F9D93, "bclri $s11, $t6, 36");
    TEST_RV32_DISASM  ( 0x2878d513, "orc_b $a0, $a7");
    TEST_RV32_DISASM  ( 0x091815b3, "shfl $a1, $a6, $a7");
    TEST_RV32_DISASM  ( 0x091855b3, "unshfl $a1, $a6, $a7");
    TEST_RV32_DISASM  ( 0xAE6C633,  "min $a2, $a3, $a4");
    TEST_RV32_DISASM  ( 0x0ae7d7b3, "minu $a5, $a5, $a4");
    TEST_RV32_DISASM  ( 0x48D747B3, "packu $a5, $a4, $a3");
    TEST_RV64_DISASM  ( 0x8D707BB,  "add_uw $a5, $a4, $a3"); // 0000100 | 01101 ($a3) | 01110 ($a4) | 000 | 01111 ($a5) | 0111011
    TEST_RV32_DISASM  ( 0x6986D713, "rev8 $a3, $a4"); // 011010011000 | 01101 ($a3) | 101 | 01110 ($a4) | 0010011
    TEST_RV64_DISASM  ( 0x6B86D713, "rev8 $a3, $a4"); // 011010111000 | 01101 ($a3) | 101 | 01110 ($a4) | 0010011

    TEST_RV64_DISASM  ( 0x28179713, "bseti $a4, $a5, 1");
    TEST_RV32_DISASM  ( 0x28269793, "bseti $a5, $a3, 2");
    TEST_RV32_DISASM  ( 0x2A269793, "bseti $a5, $a3, 2"); // this test should not be passed because shamt[5] bit is 1 (for more info see specification for bseti)
                                                          // isssue is #1508 
    TEST_RV64_DISASM  ( 0x48D717B3,  "bclr $a5, $a4, $a3"); // 0100100 | 01101 ($a3) | 01110 ($a4) | 001 | 01111 ($a5) | 0110011

    TEST_RV32_DISASM  ( 0x28F716B3,  "bset $a3, $a4, $a5");
    TEST_RV64_DISASM  ( 0x28D717B3,  "bset $a5, $a4, $a3");

    SECTION ("RISCV invalid instruction") {
        TEST_RV32_DISASM ( 0x0, "unknown" );
        TEST_RV32_DISASM ( 0xf6000053, "unknown" );
        CHECK( RISCVInstr<uint32>("qwerty", 0x0).get_disasm() == "unknown" );       
    }
}

TEST_RV32_RR_OP( 1, add, 0x1f, 0x10, 0xf)

TEST_RV32_IMM_OP( 1, lui, 0x1000, 0, 0x1)
TEST_RV32_IMM_OP( 2, lui, 0xfffff000, 0, 0xfffff)
TEST_RV64_IMM_OP( 1, lui, 0xffffffff80000000, 0, 0x80000)
TEST_RV64_IMM_OP( 2, lui, ~0xfffULL, 0, 0xfffff)

TEST_RV32_RR_OP( 1, sub, 1, 0x10, 0xf)

TEST_RV32_RR_OP( 1, slo, 0x73, 0x1C, 2)
TEST_RV32_RR_OP( 2, slo, all_ones<uint32>(), all_ones<uint32>(), 0xAA)
TEST_RV32_RR_OP( 3, slo, bitmask<uint32>(31), 0xAA, 0xFF)
TEST_RV32_RR_OP( 4, slo, all_ones<uint32>(), 0xAB, 0xFF)

TEST_RV64_RR_OP( 1, slo, 0x73, 0x1C, 2)
TEST_RV64_RR_OP( 2, slo, bitmask<uint64>(37), all_ones<uint32>(), 5)
TEST_RV64_RR_OP( 3, slo, all_ones<uint64>(), all_ones<uint64>(), 0xFF)
TEST_RV64_RR_OP( 4, slo, bitmask<uint64>(63), 0xAA, 0xFF)
TEST_RV64_RR_OP( 5, slo, all_ones<uint64>(), 0xAB, 0xFF)

TEST_RV32_IMM_OP( 1, sloi, 0x1F, 0x1F, 0x0)
TEST_RV32_IMM_OP( 2, sloi, 0x73, 0x1C, 0x2)
TEST_RV32_IMM_OP( 3, sloi, all_ones<uint32>(), all_ones<uint32>(), 0x0)
TEST_RV32_IMM_OP( 4, sloi, bitmask<uint32>(31), 0xAA, 0xFF)
TEST_RV32_IMM_OP( 5, sloi, 0x1FDF5, 0xFEFA, 0x1)

TEST_RV64_IMM_OP( 1, sloi, 0x1F, 0x1F, 0x0)
TEST_RV64_IMM_OP( 2, sloi, 0x5F, 0x17, 0x2)
TEST_RV64_IMM_OP( 3, sloi, bitmask<uint64>(37), all_ones<uint32>(), 0x5)
TEST_RV64_IMM_OP( 4, sloi, all_ones<uint64>(), all_ones<uint64>(), 0xFF)
TEST_RV64_IMM_OP( 5, sloi, 0x8000000000007, 0x0001000000000000, 0x3)

TEST_RV32_RR_OP( 1, orn, 0x10ff, 0xf7, 0xffffef77)
TEST_RV64_RR_OP( 1, orn, 0x40000000000010ff, 0xf7, 0xbfffffffffffef77)

TEST_RV32_RR_OP( 1, binv, 0x0, 0x1, 0)
TEST_RV32_RR_OP( 2, binv, 0x0, 0x1, 32)
TEST_RV32_RR_OP( 3, binv, 0x8, 0x0, 35)
TEST_RV32_RR_OP( 4, binv, 0x8f, 0xcf, 6)
TEST_RV32_RR_OP( 5, binv, 0xcf, 0x8f, 6)

TEST_RV64_RR_OP( 1, binv, 0x0, 0x1, 0)
TEST_RV64_RR_OP( 2, binv, 0x100000001, 0x1, 32)
TEST_RV64_RR_OP( 3, binv, 0x0, 0x1, 64)
TEST_RV64_RR_OP( 4, binv, 0x8, 0x0, 67)
TEST_RV64_RR_OP( 5, binv, 0x8f, 0xcf, 6)
TEST_RV64_RR_OP( 6, binv, 0xcf, 0x8f, 6)

TEST_RV32_RR_OP( 1, bext, 0x1, 0xf, 1)
TEST_RV32_RR_OP( 2, bext, 0x1, all_ones<uint32>(), 31)
TEST_RV32_RR_OP( 3, bext, 0x1, 0x6eda, 4)
TEST_RV32_RR_OP( 4, bext, 0x0, 0x6eca, 4)
TEST_RV32_RR_OP( 5, bext, 0x1, 0xD00, 8)

TEST_RV64_RR_OP( 1, bext, 0x1, 0xf, 1)
TEST_RV64_RR_OP( 2, bext, 0x1, all_ones<uint64>(), 56)
TEST_RV64_RR_OP( 3, bext, 0x1, 0x6eda, 4)
TEST_RV64_RR_OP( 4, bext, 0x0, 0x6eca, 4)
TEST_RV64_RR_OP( 5, bext, 0x1, 0xD00, 8)

TEST_RV32_RR_OP( 1, pack, 0x33332222, 0xffff2222, 0x11113333)
TEST_RV64_RR_OP( 1, pack, 0x3333333322222222, 0xffffffff22222222, 0x1111111133333333)

TEST_RV32_RR_OP( 1, xnor, 0x6fffffff, 0x30000000, 0xa0000000)
TEST_RV64_RR_OP( 1, xnor, 0x6fffffff6fffffff, 0x3000000030000000, 0xa0000000a0000000)

TEST_RV32_RR_OP( 1, max, 0x11111111, 0x11111111, 0xffffffff)
TEST_RV32_RR_OP( 1, maxu, 0xffffffff, 0x11111111, 0xffffffff)

TEST_RV32_RR_OP( 1, sro, 0xffff0001, 0x8000c000, 0xf)
TEST_RV32_RR_OP( 2, sro, 0xffffffff, 0x8000c000, 0xffU) // sro overflow
TEST_RV64_RR_OP( 1, sro, 0xffffffff00000001, 0x80000000c0000000, 0x1fU)
TEST_RV64_RR_OP( 2, sro, 0xffffffffffffffff, 0x80000000c0000000, 0xff) // sro overflow

TEST_RV32_IMM_OP( 1, sroi, 0x80000000, 0x0, 0x1)
TEST_RV32_IMM_OP( 2, sroi, 0x0, 0x0, 0x0)
TEST_RV32_IMM_OP( 3, sroi, 0xffff0001, 0x80008000, 0xf)
TEST_RV32_IMM_OP( 4, sroi, 0xffffC000, 0x0, 0x12)

TEST_RV64_IMM_OP( 1, sroi, 0x1F, 0x1F, 0x0)
TEST_RV64_IMM_OP( 2, sroi, 0xe000000000000000, 0x0, 0x3)
TEST_RV64_IMM_OP( 3, sroi, 0xFFFFFFFE00000000, 0x000000001000ABCF, 0x1F)

TEST_RV32_RR_OP( 1, bfp, 0x5C05, 0x5555, 0x080400C0)
TEST_RV64_RR_OP( 1, bfp, 0x0000555C5CCC0000, 0x00005555CCCC0000, 0x081C00C5)

TEST_RV32_RR_OP( 1, grev, 0xAAAAAAAA, 0x55555555, 0x1)
TEST_RV32_RR_OP( 2, grev, 0xCCCCCCCC, 0x33333333, 0x2)
TEST_RV32_RR_OP( 3, grev, 0x88888888, 0x11111111, 0x3)
TEST_RV32_RR_OP( 4, grev, 0x80C4A2E6, 0x01234567, 0x7)
TEST_RV32_RR_OP( 5, grev, 0xCCCCCCCC, 0x33333333,  0xF)
TEST_RV32_RR_OP( 6, grev, 0xE6A2C480, 0x01234567, 0x1F)

TEST_RV64_RR_OP( 1, grev, 0xAAAAAAAAAAAAAAAA, 0x5555555555555555, 0x1)
TEST_RV64_RR_OP( 2, grev, 0xCCCCCCCCCCCCCCCC, 0x3333333333333333, 0x2)
TEST_RV64_RR_OP( 3, grev, 0x8888888888888888, 0x1111111111111111, 0x3)
TEST_RV64_RR_OP( 4, grev, 0x80C4A2E691D5B3F7, 0x0123456789ABCDEF, 0x7)
TEST_RV64_RR_OP( 5, grev, 0xC480E6A2D591F7B3, 0x0123456789ABCDEF, 0xF)
TEST_RV64_RR_OP( 6, grev, 0xE6A2C480F7B3D591, 0x0123456789ABCDEF, 0x1F)
TEST_RV64_RR_OP( 7, grev, 0xF7B3D591E6A2C480, 0x0123456789ABCDEF, 0x3F)

TEST_RV32_RR_OP( 1, cpop, 0xc, 0xfff, 0)
TEST_RV64_RR_OP( 1, cpop, 41, 0xfafbfcfdfeff, 0)
TEST_RV128_RR_OP( 1, cpop, 82, ( ( static_cast<uint128>(0xfafbfcfdfeff) << 64) | static_cast<uint128>(0xfafbfcfdfeff)), 0)

TEST_RV32_RR_OP( 1, clz, 32, 0, 0)
TEST_RV32_RR_OP( 2, clz, 0, all_ones<uint32>(), 0)
TEST_RV32_RR_OP( 3, clz, 16, 0x0000ffff, 0)

TEST_RV64_RR_OP( 1, clz, 64, 0, 0)
TEST_RV64_RR_OP( 2, clz, 0, all_ones<uint64>(), 0)
TEST_RV64_RR_OP( 3, clz, 32, 0x0000000ffffffff, 0)

TEST_RV32_RR_OP( 1, ctz, 32, 0, 0)
TEST_RV32_RR_OP( 2, ctz, 0, all_ones<uint32>(), 0)
TEST_RV32_RR_OP( 3, ctz, 16, 0xffff0000, 0)

TEST_RV64_RR_OP( 1, ctz, 64, 0, 0)
TEST_RV64_RR_OP( 2, ctz, 0, all_ones<uint64>(), 0)
TEST_RV64_RR_OP( 3, ctz, 32, 0xffffffff00000000, 0)

TEST_RV32_RR_OP( 1, rol, 0x01234567, 0x01234567, 0)
TEST_RV32_RR_OP( 2, rol, 0x12345670, 0x01234567, 4)
TEST_RV32_RR_OP( 3, rol, 0x23456701, 0x01234567, 8)
TEST_RV32_RR_OP( 4, rol, 0x01234567, 0x01234567, 32)

TEST_RV64_RR_OP( 1, rol, 0x0123456789abcdef, 0x0123456789abcdef, 0)
TEST_RV64_RR_OP( 2, rol, 0x123456789abcdef0, 0x0123456789abcdef, 4)
TEST_RV64_RR_OP( 3, rol, 0x23456789abcdef01, 0x0123456789abcdef, 8)
TEST_RV64_RR_OP( 4, rol, 0x0123456789abcdef, 0x0123456789abcdef, 64)

TEST_RV32_RR_OP( 1, ror, 0x01234567, 0x01234567, 0)
TEST_RV32_RR_OP( 2, ror, 0x70123456, 0x01234567, 4)
TEST_RV32_RR_OP( 3, ror, 0x67012345, 0x01234567, 8)
TEST_RV32_RR_OP( 4, ror, 0x01234567, 0x01234567, 32)

TEST_RV64_RR_OP( 1, ror, 0x0123456789abcdef, 0x0123456789abcdef, 0)
TEST_RV64_RR_OP( 2, ror, 0xf0123456789abcde, 0x0123456789abcdef, 4)
TEST_RV64_RR_OP( 3, ror, 0xef0123456789abcd, 0x0123456789abcdef, 8)
TEST_RV64_RR_OP( 4, ror, 0x0123456789abcdef, 0x0123456789abcdef, 64)

TEST_RV32_IMM_OP( 1, rori, 0x01234567, 0x01234567, 0)
TEST_RV32_IMM_OP( 2, rori, 0x70123456, 0x01234567, 4)
TEST_RV32_IMM_OP( 3, rori, 0x67012345, 0x01234567, 8)
TEST_RV32_IMM_OP( 4, rori, 0x02468ace, 0x01234567, 31)
TEST_RV32_IMM_OP( 5, rori, 0x01234567, 0x01234567, 32)

TEST_RV64_IMM_OP( 1, rori, 0x0123456789abcdef, 0x0123456789abcdef, 0)
TEST_RV64_IMM_OP( 2, rori, 0xf0123456789abcde, 0x0123456789abcdef, 4)
TEST_RV64_IMM_OP( 3, rori, 0xef0123456789abcd, 0x0123456789abcdef, 8)
TEST_RV64_IMM_OP( 4, rori, 0x02468acf13579bde, 0x0123456789abcdef, 63)
TEST_RV64_IMM_OP( 5, rori, 0x0123456789abcdef, 0x0123456789abcdef, 64)

TEST_RV32_RR_OP( 1, clmul, 0, 0, 0)
TEST_RV32_RR_OP( 2, clmul, 0, 1, 0)
TEST_RV32_RR_OP( 3, clmul, 0, 0, 1)
TEST_RV32_RR_OP( 4, clmul, 1, 1, 1)
TEST_RV32_RR_OP( 5, clmul, 0, all_ones<uint32>(), 0)
TEST_RV32_RR_OP( 6, clmul, all_ones<uint32>(), all_ones<uint32>(), 1)
TEST_RV32_RR_OP( 7, clmul, 0, 0, all_ones<uint32>())
TEST_RV32_RR_OP( 8, clmul, 0x58EC, 0xA2, 0x96)
TEST_RV32_RR_OP( 9, clmul, 0xDA9AB898, 0xA328B534, 0x923ACD6E)
TEST_RV32_RR_OP( 10, clmul, 0x3093E097, 0xABCDEF45, 0xEEE111BB)

TEST_RV64_RR_OP( 1, clmul, 0, 0, 0)
TEST_RV64_RR_OP( 2, clmul, 0, 1, 0)
TEST_RV64_RR_OP( 3, clmul, 0, 0, 1)
TEST_RV64_RR_OP( 4, clmul, 1, 1, 1)
TEST_RV64_RR_OP( 5, clmul, 0, all_ones<uint32>(), 0)
TEST_RV64_RR_OP( 6, clmul, all_ones<uint32>(), all_ones<uint32>(), 1)
TEST_RV64_RR_OP( 7, clmul, 0, 0, all_ones<uint32>())
TEST_RV64_RR_OP( 8, clmul, 0x58EC, 0xA2, 0x96)
TEST_RV64_RR_OP( 9, clmul, 0xA18865DFBF34E097, 0xEEE111BBABCDEF45, 0xEEE1CC34FFAA11BB)
TEST_RV64_RR_OP( 10, clmul, 0x271E511C2A000000, 0xEE3333333BCDEF45, 0x0055552222000000)

TEST_RV32_RR_OP( 1, gorc, 0x33330000, 0x11110000, 0x1)
TEST_RV32_RR_OP( 2, gorc, 0x00AA00FF, 0x00220033, 0x2)
TEST_RV32_RR_OP( 3, gorc, 0xCCCCFFFF, 0x0C0C0F0F, 0x5)
TEST_RV32_RR_OP( 4, gorc, all_ones<uint32>(), 0x87124789, 0xC)
TEST_RV32_RR_OP( 5, gorc, all_ones<uint32>(), 1,  0xFF)
TEST_RV32_RR_OP( 6, gorc, 0xFFFF0FFF, 0x12520391,  0x3)

TEST_RV64_RR_OP( 1, gorc, 0x3333333333333333, 0x1111111111111111,  0x1)
TEST_RV64_RR_OP( 2, gorc, 0x00AA00FF00550055, 0x0022003300440055,  0x2)
TEST_RV64_RR_OP( 3, gorc, 0xCCCCFFFFFFFFFFFF, 0x0C0C0F0F0A0A0B0B,  0x5)
TEST_RV64_RR_OP( 4, gorc, 0xFFFFFFFF77773333, 0x8712478912441231,  0xC)
TEST_RV64_RR_OP( 5, gorc, all_ones<uint64>(), 1,  0xFF)
TEST_RV64_RR_OP( 6, gorc, 0xFFFF0FFFFFFF0FFF, 0x1252039112340987,  0x3)

TEST_RV32_RR_OP( 1,  orc_b, 0x00000000,         0x00000000,         0)
TEST_RV32_RR_OP( 2,  orc_b, 0x000000FF,         0x00000001,         0)
TEST_RV32_RR_OP( 3,  orc_b, 0x000000FF,         0x00000002,         0)
TEST_RV32_RR_OP( 4,  orc_b, 0x000000FF,         0x00000004,         0)
TEST_RV32_RR_OP( 5,  orc_b, 0x000000FF,         0x00000008,         0)
TEST_RV32_RR_OP( 6,  orc_b, 0x000000FF,         0x00000010,         0)
TEST_RV32_RR_OP( 7,  orc_b, 0x000000FF,         0x00000020,         0)
TEST_RV32_RR_OP( 8,  orc_b, 0x000000FF,         0x00000040,         0)
TEST_RV32_RR_OP( 9,  orc_b, 0x000000FF,         0x00000080,         0)
TEST_RV32_RR_OP( 10, orc_b, 0x0000FF00,         0x00000100,         0)
TEST_RV32_RR_OP( 11, orc_b, 0x00FF0000,         0x00020000,         0)
TEST_RV32_RR_OP( 12, orc_b, 0xFF000000,         0x03000000,         0)
TEST_RV32_RR_OP( 13, orc_b, all_ones<uint32>(), 0x11111111,         0)
TEST_RV32_RR_OP( 14, orc_b, all_ones<uint32>(), all_ones<uint32>(), 0)

TEST_RV64_RR_OP( 1,  orc_b, 0x0000000000000000, 0x0000000000000000, 0)
TEST_RV64_RR_OP( 2,  orc_b, 0x00000000000000FF, 0x0000000000000001, 0)
TEST_RV64_RR_OP( 3,  orc_b, 0x00000000000000FF, 0x0000000000000002, 0)
TEST_RV64_RR_OP( 4,  orc_b, 0x00000000000000FF, 0x0000000000000004, 0)
TEST_RV64_RR_OP( 5,  orc_b, 0x00000000000000FF, 0x0000000000000008, 0)
TEST_RV64_RR_OP( 6,  orc_b, 0x00000000000000FF, 0x0000000000000010, 0)
TEST_RV64_RR_OP( 7,  orc_b, 0x00000000000000FF, 0x0000000000000020, 0)
TEST_RV64_RR_OP( 8,  orc_b, 0x00000000000000FF, 0x0000000000000040, 0)
TEST_RV64_RR_OP( 9,  orc_b, 0x00000000000000FF, 0x0000000000000080, 0)
TEST_RV64_RR_OP( 10, orc_b, 0xFF000000000000FF, 0x2000000000000001, 0)
TEST_RV64_RR_OP( 11, orc_b, 0x00FF00000000FF00, 0x0040000000000300, 0)
TEST_RV64_RR_OP( 12, orc_b, 0x0000FF0000FF0000, 0x0000600000050000, 0)
TEST_RV64_RR_OP( 13, orc_b, 0x000000FFFF000000, 0x0000008007000000, 0)
TEST_RV64_RR_OP( 14, orc_b, all_ones<uint64>(), 0x1111111111111111, 0)
TEST_RV64_RR_OP( 15, orc_b, all_ones<uint64>(), all_ones<uint64>(), 0)

TEST_RV32_IMM_OP( 1, bclri, 0x7FFFFFFF, all_ones<uint32>(), 0x1F) // 31 bit cleared (max bit)
TEST_RV32_IMM_OP( 2, bclri, 0xFFFFFFFE, all_ones<uint32>(), 0x00) // 0 bit cleared (min bit)
TEST_RV32_IMM_OP( 3, bclri, 0xFFFDFFFF, all_ones<uint32>(), 0x11) // 17 bit cleared
TEST_RV32_IMM_OP( 4, bclri, 0xFFFDFFFF, 0xFFFDFFFF, 0x11) // Cleared already zero bit.
// Same checks.
TEST_RV64_IMM_OP( 1, bclri, 0x7FFFFFFFFFFFFFFF, all_ones<uint64>(), 0x3F)
TEST_RV64_IMM_OP( 2, bclri, 0xFFFFFFFFFFFFFFFE, all_ones<uint64>(), 0x00)
TEST_RV64_IMM_OP( 3, bclri, 0xFFFFFFFFFFFDFFFF, all_ones<uint64>(), 0x11)
TEST_RV64_IMM_OP( 4, bclri, 0xFFFFFFFFFFFDFFFF, 0xFFFFFFFFFFFDFFFF, 0x11)
// Same checks.
TEST_RV128_IMM_OP( 1, bclri, (uint128{ 0x7FFFFFFFFFFFFFFF} << 64), (uint128{ all_ones<uint64>()} << 64), 0x7F)
TEST_RV128_IMM_OP( 2, bclri, uint128{ 0xFFFFFFFFFFFFFFFE}, uint128{ all_ones<uint64>()}, 0x00)
TEST_RV128_IMM_OP( 3, bclri, (uint128{ 0xFFFFFFFFFFFDFFFF} << 64), (uint128{ all_ones<uint64>()} << 64), 0x51)
TEST_RV128_IMM_OP( 4, bclri, uint128{ 0x7FFFFFFFFFFFFFFF}, uint128{ 0x7FFFFFFFFFFFFFFF}, 0x3F)


TEST_RV32_RR_OP( 1, shfl, 0x12563478, 0x12345678, 0x08)
TEST_RV64_RR_OP( 1, shfl, 0x021346578a9bcedf, 0x0123456789abcdef, 0x04)

TEST_RV32_RR_OP( 1, unshfl, 0xf17d3bbeU, 0xbf534fdeU, 0x07)
TEST_RV64_RR_OP( 1, unshfl, 0x021cffff56c7bc85, 0x0138745bffffacd1, 0x16)

TEST_RV128_RR_OP( 1, unshfl, 0x5126426245316423, 0x3146235124646245ULL, 0x11)
TEST_RV128_RR_OP( 2, unshfl, ( uint128{ 0xc7d0b881U} << 64) + uint128{ 0x892a1084U}, 0xf14c82a6ac812410, 0x36)
TEST_RV128_RR_OP( 3, unshfl, 0x12349abc5678def0, 0x12349abc5678def0, 0x0)
TEST_RV128_RR_OP( 4, unshfl, ( uint128{ 0x7f3dff3954006401} << 64) + uint128{ 0x76f6f7f50040e0a1},
    ( uint128{ 0x7fff0fe336300001} << 64) + uint128{ 0x7f3dff3954006401},  0x0f)
TEST_RV128_RR_OP( 5, unshfl, ( uint128{ 0xa129494b84afa4e9} << 64) + uint128{ 0x4ad0b5b7367180c4},
    ( uint128{ 0x984659826593659f} << 64) + uint128{ 0x85349dabc820f892},  0x3f)

TEST_RV32_RR_OP( 1, min, 0x01234567, 0x01234567, 0x02222222)
TEST_RV32_RR_OP( 2, min, 0xffffffff, 0x11111111, 0xffffffff)
TEST_RV32_RR_OP( 3, min, 0xbbbbbbbb, 0xbbbbbbbb, 0x22222222)

TEST_RV32_RR_OP( 1, minu, 0x01234567, 0x01234567, 0x02222222)
TEST_RV32_RR_OP( 2, minu, 0x11111111, 0x11111111, 0xffffffff)
TEST_RV32_RR_OP( 3, minu, 0x22222222, 0xbbbbbbbb, 0x22222222)

TEST_RV32_RR_OP( 1, packu, 0x1111ffff, 0xffff2222, 0x11113333)
TEST_RV64_RR_OP( 1, packu, 0x11111111ffffffff, 0xffffffff22222222, 0x1111111133333333)

TEST_RV64_RR_OP( 1, add_uw, 0x12344321b5a69788, 0xabababab82736455, 0x1234432133333333)
TEST_RV64_RR_OP( 2, add_uw, 0x1111111233333332, 0x11111111ffffffff, 0x1111111133333333) // 32 bits overflow
TEST_RV64_RR_OP( 3, add_uw, 0x01010102222221ce, 0xffffffffffffffac, 0x0101010122222222) // 64 bits overflow

<<<<<<< HEAD
TEST_RV32_RR_OP(1, rev8, 0x02000000, 0x2, 0x0);
TEST_RV32_RR_OP(2, rev8, 0x78563412, 0x12345678, 0x0);

TEST_RV64_RR_OP(1, rev8, 0x7856341278563412, 0x1234567812345678, 0x0);
TEST_RV32_RR_OP(2, rev8, 0x0200000000000000, 0x2, 0x0);
=======
TEST_RV64_IMM_OP ( 1, bseti, 0x4, 0x0, 0x2)
TEST_RV64_IMM_OP ( 2, bseti, 0x100000000, 0x0, 0x20)
TEST_RV64_IMM_OP ( 3, bseti, 0x200000000000000, 0x0, 0x39)
TEST_RV64_IMM_OP ( 4, bseti, 0x8000000000000000, 0x0, 0x7F) //overflow test

TEST_RV32_IMM_OP ( 1, bseti, 0x8, 0x0, 0x3)
TEST_RV32_IMM_OP ( 2, bseti, 0x800000, 0x0, 0x17)
TEST_RV32_IMM_OP ( 3, bseti, 0x20, 0x0, 0x5)
TEST_RV32_IMM_OP ( 4, bseti, 0x80000000, 0x0, 0x3F) //overflow test

TEST_RV32_RR_OP(  1, bclr, 0xfffffff0, 0xfffffff1, 0x00000000)
TEST_RV32_RR_OP(  2, bclr, 0xfffffff1, 0xfffffff1, 0x00000002)
TEST_RV32_RR_OP(  3, bclr, 0xffdfffff, 0xffffffff, 0x00000015)
TEST_RV32_RR_OP(  4, bclr, 0xffbfffff, 0xffffffff, 0x00000016)
TEST_RV32_RR_OP(  5, bclr, 0x00000000, 0x00200000, 0x00000015)
TEST_RV32_RR_OP(  6, bclr, 0x00000000, 0x00400000, 0x00000016)
TEST_RV32_RR_OP(  7, bclr, 0xffdfffff, 0xffffffff, 0x00000035) // overflow test
TEST_RV32_RR_OP(  8, bclr, 0x00000000, 0x00200000, 0x00000035) // overflow test
TEST_RV32_RR_OP(  9, bclr, 0x7fffffff, 0xffffffff, 0xffffffff) // overflow test
TEST_RV32_RR_OP( 10, bclr, 0x00000000, 0x80000000, 0xffffffff) // overflow test

TEST_RV64_RR_OP(  1, bclr, 0xfffffffffffffff0, 0xfffffffffffffff1, 0x0000000000000000)
TEST_RV64_RR_OP(  2, bclr, 0xfffffffffffffff1, 0xfffffffffffffff1, 0x0000000000000002)
TEST_RV64_RR_OP(  3, bclr, 0xffdfffffffffffff, 0xffffffffffffffff, 0x0000000000000035)
TEST_RV64_RR_OP(  4, bclr, 0xffbfffffffffffff, 0xffffffffffffffff, 0x0000000000000036)
TEST_RV64_RR_OP(  5, bclr, 0x0000000000000000, 0x0020000000000000, 0x0000000000000035)
TEST_RV64_RR_OP(  6, bclr, 0x0000000000000000, 0x0040000000000000, 0x0000000000000036)
TEST_RV64_RR_OP(  7, bclr, 0xffbfffffffffffff, 0xffffffffffffffff, 0x0000000000000076) // overflow test
TEST_RV64_RR_OP(  8, bclr, 0x0000000000000000, 0x0040000000000000, 0x0000000000000076) // overflow test
TEST_RV64_RR_OP(  9, bclr, 0x7fffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff) // overflow test
TEST_RV64_RR_OP( 10, bclr, 0x0000000000000000, 0x8000000000000000, 0xffffffffffffffff) // overflow test

TEST_RV32_RR_OP( 1, bset, 0x00000010, 0x00000000, 0x4)
TEST_RV32_RR_OP( 2, bset, 0xFFFFFFF1, 0xFFFFFFF0, 0x0)
TEST_RV32_RR_OP( 3, bset, 0xFFFFFFF3, 0xFFFFFFF1, 0x1)
TEST_RV32_RR_OP( 4, bset, 0x00000002, 0x00000000, 0xFFFFFF01) // overflow test
TEST_RV32_RR_OP( 5, bset, 0x00010000, 0x00000000, 0xFFFFFFF0) // overflow test

TEST_RV64_RR_OP( 1, bset, 0x0000000000000100, 0x0000000000000000, 0x8)
TEST_RV64_RR_OP( 2, bset, 0x0000000000008000, 0x0000000000000000, 0xF)
TEST_RV64_RR_OP( 3, bset, 0xFFFFFFFFFFFFFFF4, 0xFFFFFFFFFFFFFFF0, 0x2)
TEST_RV64_RR_OP( 4, bset, 0xAAAAAAAAAAAAAAA2, 0xAAAAAAAAAAAAAAA0, 0xFFFFFFFFFFFFFF01) // overflow test
TEST_RV64_RR_OP( 5, bset, 0x1111111111111100, 0x1111111111111000, 0xFFFFFFFFFFFFFF08) // overflow test
>>>>>>> 5c91d7e1dfbef3349edf0f905553e7d0a640e4d9

TEST_CASE("RISCV bytes dump")
{
    CHECK( RISCVInstr<uint32>(0x204002b7).bytes_dump() == "Bytes: 0xb7 0x02 0x40 0x20");
}

TEST_CASE("RISCV sub print")
{
    RISCVInstr<uint32> instr(0x40e787b3);
    instr.set_v_src( 0x10, 0);
    instr.set_v_src( 0xf, 1);
    instr.set_sequence_id( 80);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
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
    CHECK( narrow_cast<uint64>( load.get_v_dst( 0)) == 0xf);
}

TEST_CASE("RISCV ecall")
{
    RISCVInstr<uint32> instr( "ecall", 0);
    instr.execute();
    CHECK( instr.trap_type() == Trap::SYSCALL);
}
