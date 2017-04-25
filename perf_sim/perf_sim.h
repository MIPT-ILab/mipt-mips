/*
 * perf_sim.cpp - mips performance simulator
 * Copyright 2015 MIPT-MIPS
 */

#ifndef PERF_SIM_H
#define PERF_SIM_H

#include <iostream>
#include <sstream>
#include <iomanip>

#include <common/log.h>
#include <common/ports/ports.h>

#include <func_sim/func_instr/func_instr.h>
#include <func_sim/func_memory/func_memory.h>
#include <func_sim/func_sim.h>

#include "perf_sim_rf.h"

class PerfMIPS : protected Log
{
    private:
        uint64 executed_instrs = 0;
        uint64 last_writeback_cycle = 0; // to handle possible deadlocks

        uint32 decode_data = 0;
        bool decode_next_time = false;

        RF rf;
        Addr PC = NO_VAL32;
        bool PC_is_valid = false;
        FuncMemory* mem = nullptr;

        MIPS checker;

        std::unique_ptr<WritePort<uint32>> wp_fetch_2_decode = nullptr;
        std::unique_ptr<ReadPort<uint32>> rp_fetch_2_decode = nullptr;
        std::unique_ptr<WritePort<bool>> wp_decode_2_fetch_stall = nullptr;
        std::unique_ptr<ReadPort<bool>> rp_decode_2_fetch_stall = nullptr;

        std::unique_ptr<WritePort<FuncInstr>> wp_decode_2_execute = nullptr;
        std::unique_ptr<ReadPort<FuncInstr>> rp_decode_2_execute = nullptr;
        std::unique_ptr<WritePort<bool>> wp_execute_2_decode_stall = nullptr;
        std::unique_ptr<ReadPort<bool>> rp_execute_2_decode_stall = nullptr;

        std::unique_ptr<WritePort<FuncInstr>> wp_execute_2_memory = nullptr;
        std::unique_ptr<ReadPort<FuncInstr>> rp_execute_2_memory = nullptr;
        std::unique_ptr<WritePort<bool>> wp_memory_2_execute_stall = nullptr;
        std::unique_ptr<ReadPort<bool>> rp_memory_2_execute_stall = nullptr;

        std::unique_ptr<WritePort<FuncInstr>> wp_memory_2_writeback = nullptr;
        std::unique_ptr<ReadPort<FuncInstr>> rp_memory_2_writeback = nullptr;
        std::unique_ptr<WritePort<bool>> wp_writeback_2_memory_stall = nullptr;
        std::unique_ptr<ReadPort<bool>> rp_writeback_2_memory_stall = nullptr;

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

        void check( const FuncInstr& instr);

        PerfMIPS& operator=( const PerfMIPS&) = delete;
        PerfMIPS( const PerfMIPS&) = delete;
   public:
        PerfMIPS( bool log);
        void run( const std::string& tr, uint64 instrs_to_run);
        ~PerfMIPS();
};

#endif

