/**
 * Yauheni Sharamed SHaramed.EI@phystech.edu
 * branch.cpp - Simulation of Branch Mispredict Detection stage
 * Copyright 2015-2018 MIPT-MIPS
 */

#include "branch.h"

static constexpr const uint32 FLUSHED_STAGES_NUM = 4;

template <typename FuncInstr>
Branch<FuncInstr>::Branch( bool log) : Log( log)
{
    wp_flush_all = make_write_port<bool>("BRANCH_2_ALL_FLUSH", PORT_BW, FLUSHED_STAGES_NUM);
    rp_flush = make_read_port<bool>("BRANCH_2_ALL_FLUSH", PORT_LATENCY);

    wp_flush_target = make_write_port<Target>("BRANCH_2_FETCH_TARGET", PORT_BW, PORT_FANOUT);
    wp_bp_update = make_write_port<BPInterface>("BRANCH_2_FETCH", PORT_BW, PORT_FANOUT);

    rp_datapath = make_read_port<Instr>("EXECUTE_2_BRANCH", PORT_LATENCY);
    wp_datapath = make_write_port<Instr>("BRANCH_2_WRITEBACK" , PORT_BW , PORT_FANOUT);    

    wp_bypassing_unit_flush_notify = make_write_port<bool>("BRANCH_2_BYPASSING_UNIT_FLUSH_NOTIFY", 
                                                                PORT_BW, PORT_FANOUT);
}

template <typename FuncInstr>
void Branch<FuncInstr>::clock( Cycle cycle)
{
    /* check if there is something to process */
    if ( !rp_datapath->is_ready( cycle))
    {
        return;
    }

    /* receieve flush signal */
    const bool is_flush = rp_flush->is_ready( cycle) && rp_flush->read( cycle);

    /* branch misprediction */
    if ( is_flush)
    {
        return;
    }

    auto instr = rp_datapath->read( cycle);

    /* acquiring real information for BPU */
    wp_bp_update->write( instr.get_bp_upd(), cycle);
     
    bool is_misprediction = false;

    if ( ( instr.get_bp_data() ).is_taken != instr.is_taken())
        is_misprediction = true;
    else if ( instr.is_taken())
        is_misprediction = ( instr.get_bp_data() ).target != instr.get_new_PC();

    /* handle misprediction */
    if ( is_misprediction )
    {
        /* flushing the pipeline */
        wp_flush_all->write( true, cycle);
          
        /* notify bypassing unit about misprediction */
        wp_bypassing_unit_flush_notify->write( true, cycle);

        /* sending valid PC to fetch stage */
        wp_flush_target->write( instr.get_actual_target(), cycle);
        sout << "misprediction on ";
    }

    /* log */
    sout << instr << std::endl;

    /* data path */
    wp_datapath->write( std::move( instr), cycle);
}


#include <mips/mips.h>
#include <risc_v/risc_v.h>

template class Branch<BaseMIPSInstr<uint32>>;
template class Branch<BaseMIPSInstr<uint64>>;
template class Branch<RISCVInstr<uint32>>;
template class Branch<RISCVInstr<uint64>>;
template class Branch<RISCVInstr<uint128>>;

