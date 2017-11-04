/*
* This is an open source non-commercial project. Dear PVS-Studio, please check it.
* PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
*/

#include "mips_memory.h"
#include "mips_instr.h"

#include <infra/instrcache/instr_cache.h>


FuncInstr MIPSMemory::fetch_instr( Addr PC)
{
    auto it = instr_cache.find( PC);
    bool is_cached_instr_ok = it != instr_cache.end() && it -> second.first;

    FuncInstr instr = ( is_cached_instr_ok)
                      ? FuncInstr( it -> second.second)
                      : FuncInstr( read( PC), PC);

    if ( !is_cached_instr_ok)
        instr_cache.update( PC, instr);
        
    return instr;    
}
