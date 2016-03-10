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

        void clock_fetch( int cycle) {
            sout << "fetch   cycle " << std::dec << cycle << ":";

            bool is_stall = false;
            rp_decode_2_fetch_stall->read( &is_stall, cycle);
            if ( is_stall) 
            {
                sout << "bubble\n";
                return;
            }
           
            if (PC_is_valid)
            {
                uint32 module_data = mem->read(PC);
                wp_fetch_2_decode->write( module_data, cycle);
                
                sout << std::hex << "0x" << module_data << std::endl;
            }
            else
            {
                sout << "bubble\n";
            }
        }

        void clock_decode( int cycle) {
            sout << "decode  cycle " << std::dec << cycle << ":";
            
            bool is_stall = false;
            rp_execute_2_decode_stall->read( &is_stall, cycle);
            if ( is_stall) {
                wp_decode_2_fetch_stall->write( true, cycle);
           
                sout << "bubble\n";
                return;
            }

            bool is_anything_from_fetch = rp_fetch_2_decode->read( &decode_data, cycle);
            
            FuncInstr instr( decode_data, PC);
           
            if ( instr.isJump() && is_anything_from_fetch)
                PC_is_valid = false;

            if ( !is_anything_from_fetch && !decode_next_time)
            {
                sout << "bubble\n";
                return;
            }

            if ( rf->check( instr.get_src1_num()) && rf->check( instr.get_src2_num()))
            {
                rf->read_src1( instr);
                rf->read_src2( instr);
                rf->invalidate( instr.get_dst_num());
                wp_decode_2_execute->write( instr, cycle);
                
                decode_next_time = false;
                
                if (!instr.isJump())
                    PC += 4;

                sout << instr << std::endl;
            }   
            else
            {
                wp_decode_2_fetch_stall->write( true, cycle);
                decode_next_time = true;
                sout << "bubble\n";
            }
        }

        void clock_execute( int cycle)
        {
            std::ostringstream oss;
            sout << "execute cycle " << std::dec << cycle << ":";

            bool is_stall = false;
            rp_memory_2_execute_stall->read( &is_stall, cycle);
            if ( is_stall)
            {
                wp_execute_2_decode_stall->write( true, cycle);
                
                sout << "bubble\n";
                return;
            }

            FuncInstr instr;
            if ( !rp_decode_2_execute->read( &instr, cycle))
            {
                sout << "bubble\n";
                return;
            }

            instr.execute();
            wp_execute_2_memory->write( instr, cycle);

            sout << instr << std::endl;
        }

        void clock_memory( int cycle)
        {
            sout << "memory  cycle " << std::dec << cycle << ":"; 

            bool is_stall = false;
            rp_writeback_2_memory_stall->read( &is_stall, cycle);
            if ( is_stall)
            {
                wp_memory_2_execute_stall->write( true, cycle);
                sout << "bubble\n";
                return;
            }

            FuncInstr instr;
            if ( !rp_execute_2_memory->read( &instr, cycle))
            {
                sout << "bubble\n";
                return;
            }

            load_store(instr);
            wp_memory_2_writeback->write( instr, cycle);

            sout << instr << std::endl;
        }

        void clock_writeback( int cycle)
        {
            sout << "wb      cycle " << std::dec << cycle << ":"; 

            FuncInstr instr;
            if ( !rp_memory_2_writeback->read( &instr, cycle))
            {
                sout << "bubble\n";
                return;
            }

            if ( instr.isJump())
            {
                PC_is_valid = true;
                PC = instr.get_new_PC();
            }

            rf->write_dst( instr);

            sout << instr << std::endl;
            std::cout << instr << std::endl;

            ++executed_instrs;
            last_writeback_cycle = cycle;
        }

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

