/**
 * mips.h - all the aliases to MIPS ISA
 * @author Aleksandr Misevich
 * Copyright 2018 MIPT-MIPS
 */ 

#ifndef MIPS_H_
#define MIPS_H_

#include "mips_instr.h"
#include "mips_memory.h"
#include "mips_rf.h"

struct MIPS
{
    using FuncInstr = MIPSInstr;
    using RF = MIPSRF;
    using Memory = MIPSMemory;
    using Syscalls = MIPSSyscalls;
    using Traps = MIPSTraps;
};

#endif // MIPS_H_
