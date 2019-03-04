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
    CHECK( RISCVInstr<uint32>(0x204002b7).get_disasm() == "lui $t0, 0x20400");
    CHECK( RISCVInstr<uint32>(0x00f70463).get_disasm() == "beq $a4, $a5, 8");
    CHECK( RISCVInstr<uint32>(0x00052783).get_disasm() == "lw $a5, 0x0($a0)");
    CHECK( RISCVInstr<uint32>(0x40e787b3).get_disasm() == "sub $a5, $a5, $a4");
    CHECK( RISCVInstr<uint32>(0xf95ff06f).get_disasm() == "jal $zero, -108");
}
