/**
 * mips.h - all the aliases to MIPS ISA
 * @author Aleksandr Misevich
 * Copyright 2018 MIPT-MIPS
 */ 

#ifndef MIPS_H_
#define MIPS_H_

#include <infra/instrcache/instr_cache_memory.h>
#include "mips_instr.h"
#include "mips_rf.h"

struct MIPS
{
    using FuncInstr = MIPSInstr;
    using RF = MIPSRF;
    using Memory = InstrMemory<MIPSInstr>;
};

#endif // MIPS_H_
