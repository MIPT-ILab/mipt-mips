/**
 * decode.cpp - simulation of decode stage
 * Copyright 2015-2018 MIPT-MIPS
 */

#include "decode.h"


template <typename ISA>
Decode<ISA>::Decode( bool log) : Log( log)
{
    wp_datapath = make_write_port<Instr>("DECODE_2_EXECUTE", PORT_BW, PORT_FANOUT);
    rp_datapath = make_read_port<Instr>("FETCH_2_DECODE", PORT_LATENCY);

    wp_stall_datapath = make_write_port<Instr>("DECODE_2_DECODE", PORT_BW, PORT_FANOUT);
    rp_stall_datapath = make_read_port<Instr>("DECODE_2_DECODE", PORT_LATENCY);

    wp_stall = make_write_port<bool>("DECODE_2_FETCH_STALL", PORT_BW, PORT_FANOUT);

    rp_flush = make_read_port<bool>("MEMORY_2_ALL_FLUSH", PORT_LATENCY);

    wps_command[0] = make_write_port<typename BypassingUnit::BypassCommand>("DECODE_2_EXECUTE_SRC1_COMMAND",
                                                                            PORT_BW, PORT_FANOUT);
    wps_command[1] = make_write_port<typename BypassingUnit::BypassCommand>("DECODE_2_EXECUTE_SRC2_COMMAND",
                                                                            PORT_BW, PORT_FANOUT);
    
    wp_bypassing_unit_notify = make_write_port<Instr>("DECODE_2_BYPASSING_UNIT_NOTIFY", PORT_BW, PORT_FANOUT);
    rp_bypassing_unit_notify = make_read_port<Instr>("DECODE_2_BYPASSING_UNIT_NOTIFY", PORT_LATENCY);

    rps_bypassing_unit_flush_notify[0] = make_read_port<Instr>("EXECUTE_2_BYPASSING_UNIT_FLUSH_NOTIFY",
                                                               PORT_LATENCY);
    rps_bypassing_unit_flush_notify[1] = make_read_port<Instr>("MEMORY_2_BYPASSING_UNIT_FLUSH_NOTIFY",
                                                               PORT_LATENCY);
    
    bypassing_unit = std::make_unique<BypassingUnit>();
}


template <typename ISA>
typename Decode<ISA>::Instr Decode<ISA>::read_instr( Cycle cycle)
{
    if ( rp_stall_datapath->is_ready( cycle))
    {
        rp_datapath->ignore( cycle);
        return rp_stall_datapath->read( cycle);
    }
    return rp_datapath->read( cycle);
}


template <typename ISA>
void Decode<ISA>::clock( Cycle cycle)
{
    sout << "decode  cycle " << std::dec << cycle << ": ";

    /* receive flush signal */
    const bool is_flush = rp_flush->is_ready( cycle) && rp_flush->read( cycle);

    // untrace instructions from flushed stages
    for ( auto& port:rps_bypassing_unit_flush_notify)
    {
        if ( port->is_ready( cycle))
        {
            const auto& instr = port->read( cycle);
            bypassing_unit->untrace_instr( instr);
        }
    }

    /* update bypassing unit */
    bypassing_unit->update();

    /* trace new instruction if needed */
    if ( rp_bypassing_unit_notify->is_ready( cycle))
    {
        auto instr = rp_bypassing_unit_notify->read( cycle);
        bypassing_unit->trace_new_instr( instr);
    }

    /* branch misprediction */
    if ( is_flush)
    {
        /* ignoring the upcoming instruction as it is invalid */
        rp_datapath->ignore( cycle);
        rp_stall_datapath->ignore( cycle);

        sout << "flush\n";
        return;
    }
    /* check if there is something to process */
    if ( !rp_datapath->is_ready( cycle) && !rp_stall_datapath->is_ready( cycle))
    {
        sout << "bubble\n";
        return;
    }

    auto instr = read_instr( cycle);

    if ( bypassing_unit->is_stall( instr))
    {   
        // data hazard, stalling pipeline
        wp_stall->write( true, cycle);
        wp_stall_datapath->write( instr, cycle);
        sout << instr << " (data hazard)\n";
        return;   
    }

    for ( uint8 src_index = 0; src_index < SRC_REGISTERS_NUM; src_index++)
    {
        if ( bypassing_unit->is_in_RF( instr, src_index))
        {
            rf->read_source( &instr, src_index);
        }
        else if ( bypassing_unit->is_bypassible( instr, src_index))
        {
            const auto bypass_command = bypassing_unit->get_bypass_command( instr, src_index);
            wps_command[ src_index]->write( bypass_command, cycle);
        }
    }

    /* notify bypassing unit about new instruction */
    wp_bypassing_unit_notify->write( instr, cycle);

    wp_datapath->write( instr, cycle);

    /* log */
    sout << instr << std::endl;
}


#include <mips/mips.h>
#include <risc_v/risc_v.h>

template class Decode<MIPS>;
template class Decode<RISCV32>;
template class Decode<RISCV64>;
template class Decode<RISCV128>;

