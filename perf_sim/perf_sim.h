/*
 * perf_sim.cpp - mips performance simulator
 * Copyright 2015 MIPT-MIPS 
 */

#ifndef PERF_SIM_H
#define PERF_SIM_H

#define PORT_LATENCY 1
#define PORT_FANOUT 1
#define PORT_BW 1

#include <iostream>
#include <sstream>
#include <iomanip>

#include <func_instr.h>
#include <func_memory.h>
#include <rf.h>
#include <ports.h>

class MIPS
{
    private:
        int executed_instrs;
        bool silent;

        uint32 decode_data;
        bool decode_next_time;

        RF* rf;
        uint32 PC;
        bool PC_is_valid;
        FuncMemory* mem;

        std::string fetch_dump;
        std::string decode_dump;
        std::string execute_dump;
        std::string memory_dump;
        std::string writeback_dump;

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
            std::ostringstream oss;
            oss << "fetch   cycle " << cycle << ":";

            bool is_stall;
            rp_decode_2_fetch_stall->read( &is_stall, cycle);
            if ( is_stall) 
            {
                oss << "bubble\n";
                fetch_dump = oss.str();
                return;
            }
           
            if (PC_is_valid)
            {
                uint32 module_data = mem->read(PC);
                wp_fetch_2_decode->write( module_data, cycle);
                
                oss << std::hex << "0x" << module_data << endl;
            }
            else
            {
                oss << "bubble\n";
            }

            fetch_dump = oss.str();
        }

        void clock_decode( int cycle) {
            std::ostringstream oss;
            oss << "decode  cycle " << cycle << ":";
            
            bool is_stall;
            rp_execute_2_decode_stall->read( &is_stall, cycle);
            if ( is_stall) {
                wp_decode_2_fetch_stall->write( true, cycle);
           
                oss << "bubble\n";
                decode_dump = oss.str();
                return;
            }

            bool is_anything_from_fetch = rp_fetch_2_decode->read( &decode_data, cycle);
            
            FuncInstr instr( decode_data, PC);
           
            if ( instr.is_jump() && is_anything_from_fetch)
                PC_is_valid = false;

            if ( !is_anything_from_fetch && !decode_next_time)
            {
                oss << "bubble\n";
                decode_dump = oss.str();
                return;
            }

            if ( rf->check( instr.get_src1_num()) && rf->check( instr.get_src2_num()))
            {
                rf->read_src1( instr);
                rf->read_src2( instr);
                rf->invalidate( instr.get_dst_num());
                wp_decode_2_execute->write( instr, cycle);
                
                decode_next_time = false;
                
                if (!instr.is_jump())
                    PC += 4;

                oss << instr << endl;
            }   
            else
            {
                wp_decode_2_fetch_stall->write( true, cycle);
                decode_next_time = true;
                oss << "bubble\n";
            }
                
            decode_dump = oss.str();
        }

        void clock_execute( int cycle)
        {
            std::ostringstream oss;
            oss << "execute cycle " << cycle << ":";

            bool is_stall;
            rp_memory_2_execute_stall->read( &is_stall, cycle);
            if ( is_stall)
            {
                wp_execute_2_decode_stall->write( true, cycle);
                
                oss << "bubble\n";
                execute_dump = oss.str();
                return;
            }

            FuncInstr instr;
            if ( !rp_decode_2_execute->read( &instr, cycle))
            {
                oss << "bubble\n";
                execute_dump = oss.str();
                return;
            }

            instr.execute();
            wp_execute_2_memory->write( instr, cycle);

            oss << instr << endl;
            execute_dump =  oss.str();
        }

        void clock_memory( int cycle)
        {
            std::ostringstream oss;
            oss << "memory  cycle " << cycle << ":"; 

            bool is_stall;
            rp_writeback_2_memory_stall->read( &is_stall, cycle);
            if ( is_stall)
            {
                wp_memory_2_execute_stall->write( true, cycle);
                oss << "bubble\n";
                memory_dump = oss.str();
                return;
            }

            FuncInstr instr;
            if ( !rp_execute_2_memory->read( &instr, cycle))
            {
                oss << "bubble\n";
                memory_dump = oss.str();
                return;
            }

            load_store(instr);
            wp_memory_2_writeback->write( instr, cycle);

            oss << instr << endl;
            memory_dump = oss.str();
        }

        void clock_writeback( int cycle)
        {
            std::ostringstream oss;
            if ( !silent)
                oss << "wb      cycle " << cycle << ":"; 

            FuncInstr instr;
            if ( !rp_memory_2_writeback->read( &instr, cycle))
            {
                if (!silent)
                    oss << "bubble\n";
                writeback_dump = oss.str();
                return;
            }

            if ( instr.is_jump())
            {
                PC_is_valid = true;
                PC = instr.get_new_PC();
            }

            rf->write_dst( instr);

            oss << instr << endl;
            writeback_dump = oss.str();

            ++executed_instrs;
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
        MIPS();
        void run( const std::string& tr, uint32 instrs_to_run, bool silent);
        ~MIPS();
};
            
#endif 
