/*
 * mips_memory.h - memory with interfaces to MIPS instruction
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2017 MIPT-MIPS
 */

#ifndef MIPS_MEMORY_H
#define MIPS_MEMORY_H

#include <func_sim/func_memory/func_memory.h>
#include <func_sim/func_instr/func_instr.h>

class MIPSMemory : private FuncMemory
{
public:
    MIPSMemory( const std::string& tr) : FuncMemory( tr) { }

    using FuncMemory::startPC;

    uint32 fetch( Addr pc) const { return read( pc); }

    void load( FuncInstr& instr) const {
        instr.set_v_dst(read(instr.get_mem_addr(), instr.get_mem_size()));
    }

    void store( const FuncInstr& instr) {
        write(instr.get_v_src2(), instr.get_mem_addr(), instr.get_mem_size());
    }

    void load_store(FuncInstr& instr) {
        if (instr.is_load())
            load(instr);
        else if (instr.is_store())
            store(instr);
    }
};

#endif

