/**
 * risc_v.h - all the aliases to RISC-V ISA
 * @author Pavel Kryukov
 * Copyright 2018 MIPT-MIPS
 */ 

#ifndef RISC_V_H_
#define RISC_V_H_

#include <infra/instrcache/instr_cache_memory.h>

class MyType { }; // Dummy type to treat the file as C++

struct RISCV
{
	using FuncInstr = RISCVInstr;
    using RF = RISCVRF;
    using Memory = InstrMemory<RISCVInstr>;
	using RegisterUInt = MyType;
};

#endif // RISC_V_H_
