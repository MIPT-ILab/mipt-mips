/*
 * perf_sim.cpp - mips performance simulator
 * Copyright 2015 MIPT-MIPS
 */

#ifndef PERF_SIM_H
#define PERF_SIM_H

#include <iostream>
#include <sstream>
#include <iomanip>

#include <infra/log.h>
#include <infra/ports/ports.h>

#include "func_sim/func_sim.h"
#include "mips/mips_instr.h"
#include "mips/mips_rf.h"

class PerfMIPS : protected Log
{
    private:
        uint64 executed_instrs = 0;
        uint64 last_writeback_cycle = 0; // to handle possible deadlocks

        uint32 decode_data = 0;
        bool decode_next_time = false;

        std::unique_ptr<RF> rf;
        Addr PC = NO_VAL32;
        bool PC_is_valid = false;
        MIPSMemory* mem = nullptr;

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

        void check( const FuncInstr& instr);

        PerfMIPS& operator=( const PerfMIPS&) = delete;
        PerfMIPS( const PerfMIPS&) = delete;
   public:
        explicit PerfMIPS( bool log);
        void run( const std::string& tr, uint64 instrs_to_run);
        ~PerfMIPS() final = default;
};

#endif

