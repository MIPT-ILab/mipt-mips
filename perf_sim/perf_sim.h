/*
 * perf_sim.h - header for mips pipeline simulator
 * @author Mikhail Lyubogoschev lyubogoshchev@phystech.edu
 * Copyright 2015 MIPT-MIPS 
 */


#ifndef PERF_SIM_H
#define PERF_SIM_H

#include <func_instr.h>
#include <func_memory.h>
#include <rf.h>
#include <ports.h>

#define PORT_LATENCY 1
#define PORT_FANOUT 1
#define PORT_BW 1

class PerfMIPS
{
        RF* rf;
        uint32 PC;
        bool PC_is_valid; //value, needed to solve control hazards
        /*struct Pc {
            uint32 value;
            bool is_valid;
            Pc(): PC(0ull), is_valid(true) {}
        } PC;*/
        FuncMemory* mem;

    //Ports between modules
        WritePort<uint32>*    wp_fetch_2_decode;
        ReadPort <uint32>*    rp_fetch_2_decode;
        WritePort<FuncInstr>* wp_decode_2_execute;
        ReadPort <FuncInstr>* rp_decode_2_execute;
        WritePort<FuncInstr>* wp_execute_2_memory;
        ReadPort <FuncInstr>* rp_execute_2_memory;
        WritePort<FuncInstr>* wp_memory_2_wback;
        ReadPort <FuncInstr>* rp_memory_2_wback;
    //Stall ports
        ReadPort <bool>* rp_wback_2_memory_stall;
        WritePort<bool>* wp_wback_2_memory_stall;
        ReadPort <bool>* rp_memory_2_execute_stall;
        WritePort<bool>* wp_memory_2_execute_stall;
        ReadPort <bool>* rp_execute_2_decode_stall;
        WritePort<bool>* wp_execute_2_decode_stall;
        ReadPort <bool>* rp_decode_2_fetch_stall;
        WritePort<bool>* wp_decode_2_fetch_stall;

        
        uint32 fetch() const { return mem->read(PC); }
        bool read_src( FuncInstr& instr) const {
            return ( rf->read_src1( instr)) && ( rf->read_src2( instr) && rf->check( instr.get_dst_num()));
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
        //Variables for current module_values
        uint32 fetch_data;
        FuncInstr decode_data;
        uint32 decode_int;
        FuncInstr execute_data;
        FuncInstr memory_data;
        
        uint64 executed_instr;
        bool decode_failed; //Variable needed for solving control hazards
        uint64 cycle; 
        bool silent_mode; //Variable, describing if silent mode is on

        void clock_fetch  ( uint64 cycle);
        void clock_decode ( uint64 cycle);
        void clock_execute( uint64 cycle);
        void clock_memory ( uint64 cycle);
        void clock_wback  ( uint64 cycle);

   public:
        PerfMIPS();
        void run(const std::string& tr, uint32 instrs_to_run, bool is_silent = true);
        ~PerfMIPS();
};
            
#endif
 
