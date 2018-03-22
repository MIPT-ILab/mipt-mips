/**
 * execute.cpp - Simulation of execute units
 * Copyright 2015-2018 MIPT-MIPS
 */


#include "execute.h"


template <typename ISA>
Execute<ISA>::Execute( bool log) : Log( log)
{
    wp_datapath = make_write_port<Instr>("EXECUTE_2_MEMORY", PORT_BW, PORT_FANOUT);
    rp_datapath = make_read_port<Instr>("DECODE_2_EXECUTE", PORT_LATENCY);

    rp_flush = make_read_port<bool>("MEMORY_2_ALL_FLUSH", PORT_LATENCY);

    rps_command[0] = make_read_port<typename BypassingUnit::BypassCommand>("DECODE_2_EXECUTE_SRC1_COMMAND",
                                                                           PORT_LATENCY);
    rps_command[1] = make_read_port<typename BypassingUnit::BypassCommand>("DECODE_2_EXECUTE_SRC2_COMMAND",
                                                                           PORT_LATENCY);

    wp_bypass = make_write_port<RegDstUInt>("EXECUTE_2_EXECUTE_BYPASS", PORT_BW, SRC_REGISTERS_NUM);

    rps_sources_bypass[0][0] = make_read_port<RegDstUInt>("EXECUTE_2_EXECUTE_BYPASS", PORT_LATENCY);
    rps_sources_bypass[1][0] = make_read_port<RegDstUInt>("EXECUTE_2_EXECUTE_BYPASS", PORT_LATENCY);

    rps_sources_bypass[0][1] = make_read_port<RegDstUInt>("MEMORY_2_EXECUTE_BYPASS", PORT_LATENCY);
    rps_sources_bypass[1][1] = make_read_port<RegDstUInt>("MEMORY_2_EXECUTE_BYPASS", PORT_LATENCY);

    rps_sources_bypass[0][2] = make_read_port<RegDstUInt>("WRITEBACK_2_EXECUTE_BYPASS", PORT_LATENCY);
    rps_sources_bypass[1][2] = make_read_port<RegDstUInt>("WRITEBACK_2_EXECUTE_BYPASS", PORT_LATENCY);

    wp_bypassing_unit_flush_notify = make_write_port<Instr>("EXECUTE_2_BYPASSING_UNIT_FLUSH_NOTIFY",
                                                            PORT_BW, PORT_FANOUT);
}    


template <typename ISA>
void Execute<ISA>::clock( Cycle cycle)
{
    sout << "execute cycle " << std::dec << cycle << ": ";

    /* receive flush signal */
    const bool is_flush = rp_flush->is_ready( cycle) && rp_flush->read( cycle);

    /* branch misprediction */
    if ( is_flush)
    {
        /* ignoring the upcoming instruction as it is invalid */
        if ( rp_datapath->is_ready( cycle))
        {
            const auto& instr = rp_datapath->read( cycle);
            
            /* notifying bypassing unit about invalid instruction */
            wp_bypassing_unit_flush_notify->write( instr, cycle);
        }

        /* ignoring information from command ports */
        for ( auto& port:rps_command)
            port->ignore( cycle);
        
        /* ignoring all bypassed data for source registers */
        for ( auto& rps_src_ports:rps_sources_bypass)
        {
            for ( auto& port:rps_src_ports)
                port->ignore( cycle);
        }
        
        sout << "flush\n";
        return;
    }

    /* check if there is something to process */
    if ( !rp_datapath->is_ready( cycle))
    {
        /* ignoring all bypassed data for source registers */
        for ( auto& rps_src_ports:rps_sources_bypass)
        {
            for ( auto& port:rps_src_ports)
                port->ignore( cycle);
        }

        sout << "bubble\n";
        return;
    }

    auto instr = rp_datapath->read( cycle);


    for ( uint8 src_index = 0; src_index < SRC_REGISTERS_NUM; src_index++)
    {   
        /* check whether bypassing is needed for a source register */ 
        if ( rps_command[src_index]->is_ready( cycle))
        {
            const auto bypass_command = rps_command[src_index]->read( cycle);

            /* get a port which should be used for bypassing and receive data */
            const auto bypass_direction = BypassingUnit::get_bypass_direction( bypass_command);
            const auto data = rps_sources_bypass[src_index][bypass_direction]->read( cycle);

            /* ignoring all other ports for a source register */
            for ( uint8 i = 0; i < RegisterStage::BYPASSING_STAGES_NUMBER; i++)
            {    
                if ( i != bypass_direction)
                    rps_sources_bypass[src_index][i]->ignore( cycle);
            }

            /* transform received data in accordance with bypass command */
            const auto adapted_data = BypassingUnit::adapt_bypassed_data( bypass_command, data);

            instr.set_v_src( adapted_data, src_index);
        }
        else
        {
            /* ignoring all bypassed data for a source register */
            for ( auto& port:rps_sources_bypass[src_index])
                port->ignore( cycle);
        }
    }
    

    /* perform execution */
    instr.execute();
    
    /* bypass data */
    wp_bypass->write( instr.get_bypassing_data(), cycle);

    wp_datapath->write( instr, cycle);

    /* log */
    sout << instr << std::endl;
}


#include <mips/mips.h>
#include <risc_v/risc_v.h>

template class Execute<MIPS>;
template class Execute<RISCV32>;
template class Execute<RISCV64>;
template class Execute<RISCV128>;

