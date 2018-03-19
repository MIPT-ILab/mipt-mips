/**
 * mips.h - all the aliases to MIPS ISA
 * @author Aleksandr Misevich
 * Copyright 2018 MIPT-MIPS
 */ 

#ifndef MIPS_H_
#define MIPS_H_

#include <infra/instrcache/instr_cache_memory.h>
#include "mips_instr.h"

struct MIPS
{
    using FuncInstr = MIPSInstr;
    using Register = MIPSRegister;
    using Memory = InstrMemory<MIPSInstr>;
	using RegisterUInt = uint32;
};

#endif // MIPS_H_
