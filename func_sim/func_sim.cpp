#include <iostream>

#include <func_sim.h>

MIPS::MIPS()
{
    rf = new RF();
}

void MIPS::run(const std::string& tr, uint32 instrs_to_run)
{
    mem = new FuncMemory(tr.c_str());
    PC = mem->startPC();
    for (uint32 i = 0; i < instrs_to_run; ++i) {
        // fetch
        uint32 instr_bytes = fetch();
   
        // decode
        FuncInstr instr(instr_bytes, PC);

        // read sources
        read_src(instr);

        // execute
        instr.execute();

        // load/store
        load_store(instr);

        // writeback
        wb(instr);
        
        // PC update
        PC = instr.get_new_PC();
        
        // dump
        std::cout << instr << std::endl;
    }
    delete mem;
}

MIPS::~MIPS() {
    delete rf;
}
