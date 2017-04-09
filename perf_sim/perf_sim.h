/*
 * perf_sim.cpp - mips performance simulator
 * Copyright 2015 MIPT-PerfMIPS 
 */

#ifndef PERF_SIM_H
#define PERF_SIM_H

#include <iostream>
#include <sstream>
#include <iomanip>

#include <common/log.h>
#include <func_sim/func_instr/func_instr.h>
#include <func_sim/func_memory/func_memory.h>
#include "perf_sim_rf.h"
#include "ports.h"

class PerfMIPS : protected Log
{
    private:
        uint64 executed_instrs = 0;
	uint64 last_writeback_cycle = 0; // to handle possible deadlocks

        uint32 decode_data;
        bool decode_next_time;

        RF* rf;
        uint32 PC;
        bool PC_is_valid;
        FuncMemory* mem;

        WritePort<uint32>* wp_fetch_2_decode;
        ReadPort<uint32>* rp_fetch_2_decode;
        WritePort<bool>* wp_decode_2_fetch_stall;
        ReadPort<bool>* rp_decode_2_fetch_stall;
       
        WritePort<FuncInstr>* wp_decode_2_execute;
        ReadPort<FuncInstr>* rp_decode_2_execute;
        WritePort<bool>* wp_execute_2_decode_stall;
        ReadPort<bool>* rp_execute_2_decode_stall;
        
        WritePort<FuncInstr>* wp_execute_2_memory;
        ReadPort<FuncInstr>* rp_execute_2_memory;
        WritePort<bool>* wp_memory_2_execute_stall;
        ReadPort<bool>* rp_memory_2_execute_stall;

        WritePort<FuncInstr>* wp_memory_2_writeback;
        ReadPort<FuncInstr>* rp_memory_2_writeback;
        WritePort<bool>* wp_writeback_2_memory_stall;
        ReadPort<bool>* rp_writeback_2_memory_stall;

        void clock_fetch( int cycle);
        void clock_decode( int cycle);
        void clock_execute( int cycle);
        void clock_memory( int cycle);
        void clock_writeback( int cycle);

        void load( FuncInstr& instr) const {
            instr.set_v_dst(mem->read(instr.get_mem_addr(), instr.get_mem_size()));
        }

        void store( const FuncInstr& instr) {
            mem->write( instr.get_v_src2(), instr.get_mem_addr(), instr.get_mem_size());
        }

        void load_store( FuncInstr& instr) {
            if ( instr.is_load())
                load( instr);
            else if ( instr.is_store())
                store( instr);
        }

   public:
        PerfMIPS( bool log);
        void run( const std::string& tr, uint32 instrs_to_run);
        ~PerfMIPS();
};
            
#endif

