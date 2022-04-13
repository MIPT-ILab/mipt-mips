/**
 * mem.cpp - Simulation of memory stage
 * Copyright 2015-2018 MIPT-MIPS
 */

#include "mem.h"
#include <memory/memory.h>

template <typename FuncInstr>
Mem<FuncInstr>::Mem( Module* parent) : Module( parent, "mem")
{

}

template <typename FuncInstr>
void Mem<FuncInstr>::clock( Cycle cycle)
{
    sout << "memory  cycle " << std::dec << cycle << ": ";

    const bool is_flush = ( rp_flush->is_ready( cycle) && rp_flush->read( cycle))
                       || ( rp_trap->is_ready( cycle) && rp_trap->read( cycle));

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
    wp_bypass->write( instr.get_v_dst(), cycle);
    
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
