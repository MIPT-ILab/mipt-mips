/**
 * execute.cpp - Simulation of execute units
 * Copyright 2015-2018 MIPT-MIPS
 */

#include <infra/config/config.h>

#include "execute.h"

namespace config {
    Value<uint64> long_alu_latency = { "long-alu-latency", 3, "Latency of long arithmetic logic unit"};
} // namespace config


template <typename ISA>
Execute<ISA>::Execute( bool log) 
    : Log( log)
    , last_execution_stage_latency( Latency( config::long_alu_latency - 1))
{
    wp_mem_datapath = make_write_port<Instr>("EXECUTE_2_MEMORY", PORT_BW, PORT_FANOUT);
    wp_writeback_datapath = make_write_port<Instr>("EXECUTE_2_WRITEBACK", PORT_BW, PORT_FANOUT);
    rp_datapath = make_read_port<Instr>("DECODE_2_EXECUTE", PORT_LATENCY);

    if (config::long_alu_latency < 2)
        throw Exception("Wrong argument! Latency of long arithmetic logic unit should be greater than 1");
    
    if (config::long_alu_latency > 64)
        throw Exception("Wrong argument! Latency of long arithmetic logic unit should be less than 64");

    wp_long_latency_execution_unit = make_write_port<Instr>("EXECUTE_2_EXECUTE_LONG_LATENCY", PORT_BW, PORT_FANOUT);
    rp_long_latency_execution_unit = make_read_port<Instr>("EXECUTE_2_EXECUTE_LONG_LATENCY",
                                                           last_execution_stage_latency);

    rp_flush = make_read_port<bool>("MEMORY_2_ALL_FLUSH", PORT_LATENCY);

    rps_bypass[0].command_port = make_read_port<BypassCommand<Register>>("DECODE_2_EXECUTE_SRC1_COMMAND", PORT_LATENCY);
    rps_bypass[1].command_port = make_read_port<BypassCommand<Register>>("DECODE_2_EXECUTE_SRC2_COMMAND", PORT_LATENCY);

    wp_bypass = make_write_port<InstructionOutput>("EXECUTE_2_EXECUTE_BYPASS", PORT_BW, SRC_REGISTERS_NUM);
    wp_long_arithmetic_bypass = make_write_port<InstructionOutput>("EXECUTE_COMPLEX_ALU_2_EXECUTE_BYPASS",
                                                               PORT_BW, SRC_REGISTERS_NUM);

    rps_bypass[0].data_ports[0] = make_read_port<InstructionOutput>("EXECUTE_2_EXECUTE_BYPASS", PORT_LATENCY);
    rps_bypass[1].data_ports[0] = make_read_port<InstructionOutput>("EXECUTE_2_EXECUTE_BYPASS", PORT_LATENCY);

    rps_bypass[0].data_ports[1] = make_read_port<InstructionOutput>("EXECUTE_COMPLEX_ALU_2_EXECUTE_BYPASS", PORT_LATENCY);
    rps_bypass[1].data_ports[1] = make_read_port<InstructionOutput>("EXECUTE_COMPLEX_ALU_2_EXECUTE_BYPASS", PORT_LATENCY);

    rps_bypass[0].data_ports[2] = make_read_port<InstructionOutput>("MEMORY_2_EXECUTE_BYPASS", PORT_LATENCY);
    rps_bypass[1].data_ports[2] = make_read_port<InstructionOutput>("MEMORY_2_EXECUTE_BYPASS", PORT_LATENCY);

    rps_bypass[0].data_ports[3] = make_read_port<InstructionOutput>("WRITEBACK_2_EXECUTE_BYPASS", PORT_LATENCY);
    rps_bypass[1].data_ports[3] = make_read_port<InstructionOutput>("WRITEBACK_2_EXECUTE_BYPASS", PORT_LATENCY);
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
    if (is_flush)
    {
        save_flush();
        sout << "flush\n";
        return;
    }

    /* get the instruction from long ALU if it is ready */
    if ( rp_long_latency_execution_unit->is_ready( cycle))
    {
        auto instr = rp_long_latency_execution_unit->read( cycle);

        if ( has_flush_expired())
        {
            wp_long_arithmetic_bypass->write( instr.get_dst_v(), cycle);
            wp_writeback_datapath->write( instr, cycle);
        }
    }


    /* check if there is something to process */
    if ( !rp_datapath->is_ready( cycle))
    {
        sout << "bubble\n";
        return;
    }

    auto instr = rp_datapath->read( cycle);

    auto src_index = 0;
    for ( auto& bypass_source : rps_bypass)
    {
        /* check whether bypassing is needed for a source register */
        if ( bypass_source.command_port->is_ready( cycle))
        {
            const auto bypass_direction = bypass_source.command_port->read( cycle).get_bypass_direction();
            const auto data = bypass_source.data_ports.at( bypass_direction)->read( cycle).first;
            instr.set_v_src( data, src_index);
        }
        ++src_index;
    }

    /* perform execution */
    instr.execute();

    /* log */
    sout << instr << std::endl;

    if ( instr.is_long_arithmetic()) 
    {
        wp_long_latency_execution_unit->write( std::move( instr), cycle);
    }
    else
    {
        /* bypass data */
        wp_bypass->write( instr.get_dst_v(), cycle);

        if ( instr.is_mem_stage_required())
            wp_mem_datapath->write( std::move( instr), cycle);
        else
            wp_writeback_datapath->write( std::move( instr), cycle);
    }
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

