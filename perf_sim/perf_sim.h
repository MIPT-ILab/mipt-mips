/*
 * perf_sim.h - mips pipeline simulator
 * @author Ilya Belikov ilya.i.belikov@gmail.com
 * Copyright 2015 MIPT-MIPS
 */

#ifndef PERF_SIM_H
#define PERF_SIM_H

#include <func_instr.h>
#include <func_memory.h>
#include <ports.h>
#include <rf.h>

#define PORT_BW 1
#define PORT_FANOUT 1
#define PORT_LATENCY 1


class PerfMIPS {
    RF* rf;
    uint32 PC;
    bool PC_is_valid;
    FuncMemory* mem;

    bool silent;

    int cycle;
    int executed_instrs;
    int num_of_instructions;

    uint32 fetch( bool* is_valid) const {
        *is_valid = PC_is_valid;
        return mem->read( PC);
    }

    void invalidate_PC() { PC_is_valid = false;}
    void update_PC( uint32 new_PC) {
        PC = new_PC;
        PC_is_valid = true;
    }


    bool read_src( FuncInstr& instr) const {
        return rf->read_src1( instr) && rf->read_src2( instr);
    }

    void load( FuncInstr& instr) const {
        instr.set_v_dst( mem->read( instr.get_mem_addr(), instr.get_mem_size()));
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

    void wb( const FuncInstr& instr) {
        rf->write_dst( instr);
    }

    void clock_fetch( int cycle);
    void clock_decode( int cycle);
    void clock_execute( int cycle);
    void clock_memory( int cycle);
    void clock_writeback( int cycle);

    // Ports
    ReadPort<uint32>*       rp_fetch_2_decode;
    ReadPort<FuncInstr>*    rp_decode_2_execute;
    ReadPort<FuncInstr>*    rp_execute_2_memory;
    ReadPort<FuncInstr>*    rp_memory_2_writeback;

    WritePort<uint32>*      wp_fetch_2_decode;
    WritePort<FuncInstr>*   wp_decode_2_execute;
    WritePort<FuncInstr>*   wp_execute_2_memory;
    WritePort<FuncInstr>*   wp_memory_2_writeback;

    // Stall ports
    ReadPort<bool>*     rp_decode_2_fetch_stall;

    WritePort<bool>*    wp_decode_2_fetch_stall;

public:

    PerfMIPS();
    void run( const std::string& tr, int instrs_to_run, bool silent_mode = true);
    ~PerfMIPS();
};

#endif
