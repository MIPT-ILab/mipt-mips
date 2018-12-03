/**
 * rediction.cpp - Simulation of Branch Mispredict Detection stage
 * Copyright 2015-2018 MIPT-MIPS
 */

#include "prediction.h"

static constexpr const uint32 FLUSHED_STAGES_NUM = 4;

template <typename ISA>
Prediction<ISA>::Prediction( bool log) : Log( log)
{
    wp_flush_all = make_write_port<bool>("MEMORY_2_ALL_FLUSH", PORT_BW, FLUSHED_STAGES_NUM);
    rp_flush = make_read_port<bool>("MEMORY_2_ALL_FLUSH", PORT_LATENCY);

    wp_flush_target = make_write_port<Target>("MEMORY_2_FETCH_TARGET", PORT_BW, PORT_FANOUT);
    wp_bp_update = make_write_port<BPInterface>("MEMORY_2_FETCH", PORT_BW, PORT_FANOUT);

    rp_datapath = make_read_port<Instr>("EXECUTE_2_PREDICTION", PORT_LATENCY);

    wp_bypassing_unit_flush_notify = make_write_port<bool>("MEMORY_2_BYPASSING_UNIT_FLUSH_NOTIFY", PORT_BW, PORT_FANOUT);

}

template <typename ISA>
void Prediction<ISA>::clock( Cycle cycle)
{
    /* receieve flush signal */
    const bool is_flush = rp_flush->is_ready( cycle) && rp_flush->read( cycle);

    /* branch misprediction */
    if ( is_flush)
    {
        //sout << "flush\n"; was writing by mem
        return;
    }

    /* check if there is something to process */
    if ( !rp_datapath->is_ready( cycle))
    {
        //sout << "bubble\n"; was writing by mem
        return;
    }


    auto instr = rp_datapath->read( cycle);

    if ( instr.is_jump()) {
        /* acquiring real information for BPU */
        wp_bp_update->write( instr.get_bp_upd(), cycle);

        /* handle misprediction */
        if ( instr.is_misprediction())
        {
            /*flushing the pipeline */
            wp_flush_all->write( true, cycle);

            /* notify bypassing unit about misprediction */
            wp_bypassing_unit_flush_notify->write( true, cycle);

            /* sending valid PC to fetch stage */
            wp_flush_target->write( instr.get_actual_target(), cycle);
            sout << "misprediction on ";
        }
    }

    /* log */
    sout << instr << std::endl;
}


#include <mips/mips.h>
#include <risc_v/risc_v.h>

template class Prediction<MIPSI>;
template class Prediction<MIPSII>;
template class Prediction<MIPSIII>;
template class Prediction<MIPSIV>;
template class Prediction<MIPS32>;
template class Prediction<MIPS64>;
template class Prediction<RISCV32>;
template class Prediction<RISCV64>;
template class Prediction<RISCV128>;

