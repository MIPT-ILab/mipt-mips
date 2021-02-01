/**
 * Yauheni Sharamed SHaramed.EI@phystech.edu
 * branch.cpp - Simulation of Branch Mispredict Detection stage
 * Copyright 2015-2018 MIPT-MIPS
 */

#include "branch.h"

template <typename FuncInstr>
Branch<FuncInstr>::Branch( Module* parent) : Module( parent, "branch")
{
    wp_flush_all = make_write_port<bool>("BRANCH_2_ALL_FLUSH", Port::BW);
    rp_flush = make_read_port<bool>("BRANCH_2_ALL_FLUSH", Port::LATENCY);
    rp_trap = make_read_port<bool>("WRITEBACK_2_ALL_FLUSH", Port::LATENCY);

    wp_flush_target = make_write_port<Target>("BRANCH_2_FETCH_TARGET", Port::BW);
    wp_bp_update = make_write_port<BPInterface>("BRANCH_2_FETCH", Port::BW);

    rp_datapath = make_read_port<Instr>("EXECUTE_2_BRANCH", Port::LATENCY);
    wp_datapath = make_write_port<Instr>("BRANCH_2_WRITEBACK" , Port::BW );    

    wp_bypass = make_write_port<InstructionOutput>("BRANCH_2_EXECUTE_BYPASS", Port::BW);

    wp_bypassing_unit_flush_notify = make_write_port<bool>("BRANCH_2_BYPASSING_UNIT_FLUSH_NOTIFY", Port::BW);
}

template <typename FuncInstr>
void Branch<FuncInstr>::clock( Cycle cycle)
{
    if ( !rp_datapath->is_ready( cycle))
        return;

    const bool is_flush = ( rp_flush->is_ready( cycle) && rp_flush->read( cycle))
                       || ( rp_trap->is_ready( cycle) && rp_trap->read( cycle));

    if ( is_flush)
        return;

    sout << "branch  cycle " << std::dec << cycle << ": ";
    auto instr = rp_datapath->read( cycle);

    /* acquiring real information for BPU */
    wp_bp_update->write( instr.get_bp_upd(), cycle);

    if ( instr.is_jump())
        num_jumps++;

    /* handle misprediction */
    if ( is_misprediction( instr, instr.get_bp_data()))
    {
        num_mispredictions++;

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

    /* bypass data */
    wp_bypass->write( instr.get_v_dst(), cycle);

    /* data path */
    wp_datapath->write( std::move( instr), cycle);
}


#include <mips/mips.h>
#include <modules/t/test_instr.h>
#include <risc_v/risc_v.h>

template class Branch<BaseMIPSInstr<uint32>>;
template class Branch<BaseMIPSInstr<uint64>>;
template class Branch<RISCVInstr<uint32>>;
template class Branch<RISCVInstr<uint64>>;
template class Branch<RISCVInstr<uint128>>;
template class Branch<BranchTestInstr>;
