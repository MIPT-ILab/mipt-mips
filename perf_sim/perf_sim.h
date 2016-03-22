/**
 * perf_sim.h
 * Header for scalar MIPS CPU simulator.
 * MIPT-MIPS Assignment 4.
 * Ladin Oleg.
 */

/* Protection from multi-including. */
#ifndef PERF_SIM_H
#define PERF_SIM_H

/* Simulator modules. */
#include <func_memory.h>
#include <perf_sim_rf.h>
#include <ports.h>

class PerfMIPS
{
    private:
        /** Functional simulator components. */
        RF* rf;
        uint32 PC;
        FuncMemory* mem;

        uint32 fetch() const { return mem->read( PC); }
        void read_src( FuncInstr& instr) const
        {
            rf->read_src1( instr);
            rf->read_src2( instr);
        }
        void load( FuncInstr& instr) const
        {
            instr.set_v_dst( mem->read( instr.get_mem_addr(), instr.get_mem_size()));
        }
        void store( const FuncInstr& instr)
        {
            mem->write( instr.get_v_src2(), instr.get_mem_addr(), instr.get_mem_size());
        }
        void load_store( FuncInstr& instr)
        {
            if ( instr.is_load())
            {
                load( instr);
            } else if ( instr.is_store())
            {
                store( instr);
            }
        }
        void wb( const FuncInstr& instr) { rf->write_dst( instr); }


        /** Performance simulator components. */
        /* Data ports. */
        ReadPort< uint32>* rp_fetch_2_decode;
        WritePort< uint32>* wp_fetch_2_decode;
        ReadPort< FuncInstr>* rp_decode_2_execute;
        WritePort< FuncInstr>* wp_decode_2_execute;
        ReadPort< FuncInstr>* rp_execute_2_memory;
        WritePort< FuncInstr>* wp_execute_2_memory;
        ReadPort< FuncInstr>* rp_memory_2_writeback;
        WritePort< FuncInstr>* wp_memory_2_writeback;

        /* Stall ports. */
        ReadPort< bool>* rp_decode_2_fetch_stall;
        WritePort< bool>* wp_decode_2_fetch_stall;
        ReadPort< bool>* rp_execute_2_decode_stall;
        WritePort< bool>* wp_execute_2_decode_stall;
        ReadPort< bool>* rp_memory_2_execute_stall;
        WritePort< bool>* wp_memory_2_execute_stall;
        ReadPort< bool>* rp_writeback_2_memory_stall;
        WritePort< bool>* wp_writeback_2_memory_stall;

        int executed_instrs; // executed instructions counter
        bool is_silent; // mode flag

        /* Here modules stores data. */
        uint32 fetch_data;
        uint32 decode_data;
        FuncInstr execute_data;
        FuncInstr memory_data;
        FuncInstr writeback_data;

        bool PC_is_valid; // validate flag of PC

        /* Components for handling data dependency. */
        uint32 source_stall_data; // storage for data came from Fetch
        bool source_stall; // is stall flag: wait for sources
        bool source_stall_end; // stall ended: decode came from Fetch data
        bool decode_stall; // on start, Decode can generate "nop", prevent it

        /* Main methods of each modules. */
        void clockFetch( int cycle);
        void clockDecode( int cycle);
        void clockExecute( int cycle);
        void clockMemory( int cycle);
        void clockWriteback( int cycle);

        /* Checks instruction could it change PC unusually. */
        bool isJump( uint32 data);

    public:
        PerfMIPS();
        ~PerfMIPS();
        /* Starts simulator. */
        void run( const string& tr, int instr_to_run, bool is_silent);
};

#endif // #ifndef PERF_SIM_H
