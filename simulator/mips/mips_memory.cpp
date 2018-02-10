
#include "mips_memory.h"
#include "mips_instr.h"

#include <infra/instrcache/instr_cache.h>

MIPSInstr MIPSMemory::fetch_instr( Addr PC)
{
    const auto& [found, value] = instr_cache.find( PC); // NOLINT https://bugs.llvm.org/show_bug.cgi?id=36283

    MIPSInstr instr = found ? value : MIPSInstr( fetch( PC), PC);

    instr_cache.update( PC, instr);

    return instr;
}
