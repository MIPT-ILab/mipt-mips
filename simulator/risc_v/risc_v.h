/**
 * risc_v.h - all the aliases to RISC-V ISA
 * @author Pavel Kryukov
 * Copyright 2018 MIPT-MIPS
 */ 

#ifndef RISC_V_H_
#define RISC_V_H_

#include "riscv_instr.h"

#include <infra/instrcache/instr_cache_memory.h>
#include <infra/types.h>

template <typename T>
struct RISCV
{
    using FuncInstr = RISCVInstr<T>;
    using Register = RISCVRegister;
    using Memory = InstrMemoryCached<RISCVInstr<T>>;
    using RegisterUInt = T;
};

using RISCV32 = RISCV<uint32>;
using RISCV64 = RISCV<uint64>;
using RISCV128 = RISCV<uint128>;

#endif // RISC_V_H_
