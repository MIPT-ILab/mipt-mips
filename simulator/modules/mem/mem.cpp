/**
 * mem.cpp - Simulation of memory stage
 * Copyright 2015-2018 MIPT-MIPS
 */

#include "mem.h"
#include <memory/memory.h>

template <typename FuncInstr>
Mem<FuncInstr>::Mem( bool log) : Log( log)
{
    wp_datapath = make_write_port<Instr>("MEMORY_2_WRITEBACK", PORT_BW, PORT_FANOUT);
    rp_datapath = make_read_port<Instr>("EXECUTE_2_MEMORY", PORT_LATENCY);

    rp_flush = make_read_port<bool>("BRANCH_2_ALL_FLUSH", PORT_LATENCY);

    wp_bypass = make_write_port<InstructionOutput>("MEMORY_2_EXECUTE_BYPASS", PORT_BW, SRC_REGISTERS_NUM);
}

template <typename FuncInstr>
void Mem<FuncInstr>::clock( Cycle cycle)
{
    sout << "memory  cycle " << std::dec << cycle << ": ";

    /* receieve flush signal */
    const bool is_flush = rp_flush->is_ready( cycle) && rp_flush->read( cycle);

    /* branch misprediction */
    if ( is_flush)
    {
        sout << "flush\n";
        return;
    }

    /* check if there is something to process */
    if ( !rp_datapath->is_ready( cycle))
    {
        sout << "bubble\n";
        return;
    }

    auto instr = rp_datapath->read( cycle);

    /* perform required loads and stores */
    memory->load_store( &instr);
    
    /* bypass data */
    wp_bypass->write( std::make_pair(instr.get_v_dst(), instr.get_v_dst2()), cycle);
    
    /* data path */
    wp_datapath->write( std::move( instr), cycle);
}


#include <mips/mips.h>
#include <risc_v/risc_v.h>

template class Mem<BaseMIPSInstr<uint32>>;
template class Mem<BaseMIPSInstr<uint64>>;
template class Mem<RISCVInstr<uint32>>;
template class Mem<RISCVInstr<uint64>>;
template class Mem<RISCVInstr<uint128>>;
