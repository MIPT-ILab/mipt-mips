/*
* This is an open source non-commercial project. Dear PVS-Studio, please check it.
* PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
*/

#include "mips_memory.h"
#include "mips_instr.h"

#include <infra/instrcache/instr_cache.h>

#include <iostream>
FuncInstr MIPSMemory::fetch_instr( Addr PC)
{
    auto it = instr_cache.find( PC);

    FuncInstr instr = ( it != instr_cache.end())
                      ? FuncInstr( it->second)
                      : FuncInstr( read( PC), PC);

    if ( it != instr_cache.end())
        instr_cache.update( PC, instr);
        
    return instr;    
}
