/*
 * RISC-V instruction unit tests
 * Author pavel.kryukov@phystech.edu
 * Copyright 2019 MIPT-MIPS
 */
 
#include "../riscv_instr.h"
 
#include <catch.hpp>

TEST_CASE("RISCV disassembly")
{
//    CHECK( RISCVInstr<uint32>(0x401).get_disasm() == "addi $s0, $s0, 0" );
//    CHECK( RISCVInstr<uint32>(0x404).get_disasm() == "addi $s0, $s0, 1" );
//    CHECK( RISCVInstr<uint32>(0x405).get_disasm() == "addi $s1, $sp, 512" );
    CHECK( RISCVInstr<uint32>(0x597).get_disasm() == "auipc $a1, 0x0" );
    CHECK( RISCVInstr<uint32>(0x204002b7).get_disasm() == "lui $t0, 0x20400");    
}
