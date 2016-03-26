/*
 * func_sim.cpp - mips single-cycle simulator
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS 
 */

#ifndef FUNC_SIM_H
#define FUNC_SIM_H

#include <func_instr.h>
#include <func_memory.h>
#include <rf.h>

class MIPS
{
    private:
        RF* rf;
        uint32 PC;
        FuncMemory* mem;

        uint32 fetch() const { return mem->read(PC); }
        void read_src(FuncInstr& instr) const {
            rf->read_src1(instr); 
            rf->read_src2(instr); 
	    }

        void load(FuncInstr& instr) const {
            instr.set_v_dst(mem->read(instr.get_mem_addr(), instr.get_mem_size()));
        }

        void store(const FuncInstr& instr) {
            mem->write(instr.get_v_src2(), instr.get_mem_addr(), instr.get_mem_size());
        }

	    void load_store(FuncInstr& instr) {
            if (instr.is_load())
                load(instr);
            else if (instr.is_store())
                store(instr);
        }

        void wb(const FuncInstr& instr) {
            rf->write_dst(instr);
        }
   public:
        MIPS();
        void run(const std::string& tr, uint32 instrs_to_run);
        ~MIPS();
};
            
#endif
 
