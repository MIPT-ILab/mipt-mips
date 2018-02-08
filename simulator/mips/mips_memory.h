/*
 * mips_memory.h - memory with interfaces to MIPS instruction
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2017 MIPT-MIPS
 */

#ifndef MIPS_MEMORY_H
#define MIPS_MEMORY_H

#include <infra/instrcache/instr_cache.h>
#include <infra/memory/memory.h>
#include "mips_instr.h"

class MIPSMemory : private FuncMemory
{
    private:
        InstrCache<MIPSInstr> instr_cache{};

    public:
        explicit MIPSMemory( const std::string& tr) : FuncMemory( tr) { }

        using FuncMemory::startPC;

        uint32 fetch( Addr pc) const { return read( pc); }

        MIPSInstr fetch_instr( Addr PC);

        void load( MIPSInstr* instr) const
        {
            instr->set_v_dst(read(instr->get_mem_addr(), instr->get_mem_size()));
        }

        void store( const MIPSInstr& instr)
        {
            instr_cache.erase( instr.get_mem_addr());
            write(instr.get_v_src2(), instr.get_mem_addr(), instr.get_mem_size());
        }

        void load_store(MIPSInstr* instr)
        {
            if (instr->is_load())
                load(instr);
            else if (instr->is_store())
                store(*instr);
        }
};

#endif

