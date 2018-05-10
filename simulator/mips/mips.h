/**
 * mips.h - all the aliases to MIPS ISA
 * @author Aleksandr Misevich
 * Copyright 2018 MIPT-MIPS
 */

#ifndef MIPS_H_
#define MIPS_H_

#include <infra/instrcache/instr_cache_memory.h>
#include "mips_instr.h"

template<typename T>
struct MIPS
{
    using Register = MIPSRegister;
    using RegisterUInt = T;
    using FuncInstr = MIPSInstr<T>;
    using Memory = InstrMemory<FuncInstr>;
};

using MIPS32 = MIPS<uint32>;
using MIPS64 = MIPS<uint64>;

static_assert( std::is_same_v<MIPS32Instr, MIPS32::FuncInstr>);
static_assert( std::is_same_v<MIPS64Instr, MIPS64::FuncInstr>);

#endif // MIPS_H_
