/**
 * mips.h - all the aliases to MIPS ISA
 * @author Aleksandr Misevich
 * Copyright 2018 MIPT-MIPS
 */

#ifndef MIPS_H_
#define MIPS_H_

#include <infra/instrcache/instr_cache_memory.h>
#include "mips_instr.h"

struct MIPS32
{
    using FuncInstr = MIPS32Instr;
    using Register = MIPSRegister;
    using Memory = InstrMemory<MIPS32Instr>;
    using RegisterUInt = uint32;
};

struct MIPS64
{
    using FuncInstr = MIPS64Instr;
    using Register = MIPSRegister;
    using Memory = InstrMemory<MIPS64Instr>;
    using RegisterUInt = uint64;
};

#endif // MIPS_H_
