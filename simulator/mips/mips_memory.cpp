/*
* This is an open source non-commercial project. Dear PVS-Studio, please check it.
* PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
*/

#include "mips_memory.h"
#include "mips_instr.h"

#include <infra/instrcache/instr_cache.h>

FuncInstr MIPSMemory::fetch_instr( Addr PC)
{
    const auto& [found, value] = instr_cache.find( PC);

    FuncInstr instr = found ? value : FuncInstr( fetch( PC), PC);

    instr_cache.update( PC, instr);

    return instr;    
}
