/**
 * risc_v.h - all the aliases to RISC-V ISA
 * @author Pavel Kryukov
 * Copyright 2018 MIPT-MIPS
 */ 

#ifndef RISC_V_H_
#define RISC_V_H_

#include <infra/instrcache/instr_cache_memory.h>
#include "riscv_instr.h"

class MyType { }; // Dummy type to treat the file as C++

template <typename T>
struct RISCV
{
    using FuncInstr = RISCVInstr;
    using Register = RISCVRegister;
    using Memory = InstrMemory<RISCVInstr>;
    using RegisterUInt = T;
};

using RISCV32 = RISCV<uint32>;
using RISCV64 = RISCV<uint64>;
using RISCV128 = RISCV<uint128>;

#endif // RISC_V_H_
