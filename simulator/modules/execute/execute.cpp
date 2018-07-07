/**
 * execute.cpp - Simulation of execute units
 * Copyright 2015-2018 MIPT-MIPS
 */

#include <infra/config/config.h>

#include "execute.h"

namespace config {
    Value<uint64> complex_alu_latency = { "complex-alu-latency", 3, "Latency of complex arithmetic logic unit"};
} // namespace config


template <typename ISA>
Execute<ISA>::Execute( bool log) 
    : Log( log)
    , last_execution_stage_latency( Latency( config::complex_alu_latency - 1))
{
    wp_mem_datapath = make_write_port<Instr>("EXECUTE_2_MEMORY", PORT_BW, PORT_FANOUT);
    wp_writeback_datapath = make_write_port<Instr>("EXECUTE_2_WRITEBACK", PORT_BW, PORT_FANOUT);
    rp_datapath = make_read_port<Instr>("DECODE_2_EXECUTE", PORT_LATENCY);

    if ( config::complex_alu_latency < 2)
        serr << "ERROR: Wrong argument! Latency of complex arithmetic logic unit should be greater than 1"
             << std::endl << critical;
    
    if ( config::complex_alu_latency > 64)
        serr << "ERROR: Wrong argument! Latency of complex arithmetic logic unit should be less than 64"
             << std::endl << critical;

    wp_long_latency_execution_unit = make_write_port<Instr>("EXECUTE_2_EXECUTE_LONG_LATENCY", PORT_BW, PORT_FANOUT);
    rp_long_latency_execution_unit = make_read_port<Instr>("EXECUTE_2_EXECUTE_LONG_LATENCY",
                                                           last_execution_stage_latency);

    rp_flush = make_read_port<bool>("MEMORY_2_ALL_FLUSH", PORT_LATENCY);

    rps_command[0] = make_read_port<BypassCommand<Register>>("DECODE_2_EXECUTE_SRC1_COMMAND",
                                                                           PORT_LATENCY);
    rps_command[1] = make_read_port<BypassCommand<Register>>("DECODE_2_EXECUTE_SRC2_COMMAND",
                                                                           PORT_LATENCY);

    wp_bypass = make_write_port<std::pair<RegisterUInt, RegisterUInt>>("EXECUTE_2_EXECUTE_BYPASS", PORT_BW, SRC_REGISTERS_NUM);
    wp_complex_arithmetic_bypass = make_write_port<std::pair<RegisterUInt, RegisterUInt>>("EXECUTE_COMPLEX_ALU_2_EXECUTE_BYPASS",
                                                               PORT_BW, SRC_REGISTERS_NUM);

    rps_sources_bypass[0][0] = make_read_port<std::pair<RegisterUInt, RegisterUInt>>("EXECUTE_2_EXECUTE_BYPASS", PORT_LATENCY);
    rps_sources_bypass[1][0] = make_read_port<std::pair<RegisterUInt, RegisterUInt>>("EXECUTE_2_EXECUTE_BYPASS", PORT_LATENCY);

    rps_sources_bypass[0][1] = make_read_port<std::pair<RegisterUInt, RegisterUInt>>("EXECUTE_COMPLEX_ALU_2_EXECUTE_BYPASS", PORT_LATENCY);
    rps_sources_bypass[1][1] = make_read_port<std::pair<RegisterUInt, RegisterUInt>>("EXECUTE_COMPLEX_ALU_2_EXECUTE_BYPASS", PORT_LATENCY);

    rps_sources_bypass[0][2] = make_read_port<std::pair<RegisterUInt, RegisterUInt>>("MEMORY_2_EXECUTE_BYPASS", PORT_LATENCY);
    rps_sources_bypass[1][2] = make_read_port<std::pair<RegisterUInt, RegisterUInt>>("MEMORY_2_EXECUTE_BYPASS", PORT_LATENCY);

    rps_sources_bypass[0][3] = make_read_port<std::pair<RegisterUInt, RegisterUInt>>("WRITEBACK_2_EXECUTE_BYPASS", PORT_LATENCY);
    rps_sources_bypass[1][3] = make_read_port<std::pair<RegisterUInt, RegisterUInt>>("WRITEBACK_2_EXECUTE_BYPASS", PORT_LATENCY);
}    

template <typename ISA>
void Execute<ISA>::clock( Cycle cycle)
{
    sout << "execute cycle " << std::dec << cycle << ": ";

    /* receive flush signal */
    const bool is_flush = rp_flush->is_ready( cycle) && rp_flush->read( cycle);
    
    /* update information about mispredictions */
    clock_saved_flush();

    /* branch misprediction */
    if ( is_flush)
    {
        /* ignoring the upcoming instruction as it is invalid */
        rp_datapath->ignore( cycle);

        /* ignoring the instruction from complex ALU */
        rp_long_latency_execution_unit->ignore( cycle);

        /* ignoring information from command ports */
        for ( auto& port:rps_command)
            port->ignore( cycle);

        /* ignoring all bypassed data for source registers */
        for ( auto& rps_src_ports:rps_sources_bypass)
        {
            for ( auto& port:rps_src_ports)
                port->ignore( cycle);
        }

        save_flush();

        sout << "flush\n";
        return;
    }


    /* get the instruction from complex ALU if it is ready */
    if ( rp_long_latency_execution_unit->is_ready( cycle))
    {
        auto instr = rp_long_latency_execution_unit->read( cycle);

        if ( has_flush_expired())
        {
            wp_complex_arithmetic_bypass->write( std::make_pair(instr.get_v_dst(), instr.get_v_dst2()), cycle);
            wp_writeback_datapath->write( instr, cycle);
        }
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
            const auto bypass_direction = bypass_command.get_bypass_direction();
            const auto data = rps_sources_bypass[src_index][bypass_direction]->read( cycle);

            /* ignoring all other ports for a source register */
            for ( uint8 i = 0; i < RegisterStage::BYPASSING_STAGES_NUMBER; i++)
            {
                if ( i != bypass_direction)
                    rps_sources_bypass[src_index][i]->ignore( cycle);
            }

            instr.set_v_src( data.first, src_index);
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

    if ( instr.is_complex_arithmetic()) 
    {
        wp_long_latency_execution_unit->write( instr, cycle);
    }
    else
    {
        /* bypass data */
        wp_bypass->write( std::make_pair(instr.get_v_dst(), instr.get_v_dst2()), cycle);
        
        if ( instr.is_mem_stage_required())
            wp_mem_datapath->write( instr, cycle);
        else
            wp_writeback_datapath->write( instr, cycle);
    }


    /* log */
    sout << instr << std::endl;
}


#include <mips/mips.h>
#include <risc_v/risc_v.h>

template class Execute<MIPSI>;
template class Execute<MIPSII>;
template class Execute<MIPSIII>;
template class Execute<MIPSIV>;
template class Execute<MIPS32>;
template class Execute<MIPS64>;
template class Execute<RISCV32>;
template class Execute<RISCV64>;
template class Execute<RISCV128>;

