#include <cassert>

#include <iostream>
#include <chrono>

#include "perf_sim.h"

static constexpr const uint32 FLUSHED_STAGES_NUM = 3;

template <typename ISA>
PerfSim<ISA>::PerfSim(bool log) : Simulator( log), rf( new RF<ISA>), fetch( log), writeback( log)
{
    rp_fetch_2_decode = make_read_port<Instr>("FETCH_2_DECODE", PORT_LATENCY);

    wp_decode_2_fetch_stall = make_write_port<bool>("DECODE_2_FETCH_STALL", PORT_BW, PORT_FANOUT);

    wp_decode_2_decode = make_write_port<Instr>("DECODE_2_DECODE", PORT_BW, PORT_FANOUT);
    rp_decode_2_decode = make_read_port<Instr>("DECODE_2_DECODE", PORT_LATENCY);

    wp_decode_2_execute = make_write_port<Instr>("DECODE_2_EXECUTE", PORT_BW, PORT_FANOUT);
    rp_decode_2_execute = make_read_port<Instr>("DECODE_2_EXECUTE", PORT_LATENCY);

    wp_execute_2_memory = make_write_port<Instr>("EXECUTE_2_MEMORY", PORT_BW, PORT_FANOUT);
    rp_execute_2_memory = make_read_port<Instr>("EXECUTE_2_MEMORY", PORT_LATENCY);

    /* branch misprediction unit ports */
    wp_memory_2_all_flush = make_write_port<bool>("MEMORY_2_ALL_FLUSH", PORT_BW, FLUSHED_STAGES_NUM);
    rp_decode_flush = make_read_port<bool>("MEMORY_2_ALL_FLUSH", PORT_LATENCY);
    rp_execute_flush = make_read_port<bool>("MEMORY_2_ALL_FLUSH", PORT_LATENCY);
    rp_memory_flush = make_read_port<bool>("MEMORY_2_ALL_FLUSH", PORT_LATENCY);

    wp_memory_2_fetch_target = make_write_port<Addr>("MEMORY_2_FETCH_TARGET", PORT_BW, PORT_FANOUT);

    wp_core_2_fetch_target = make_write_port<Addr>("CORE_2_FETCH_TARGET", PORT_BW, PORT_FANOUT);

    wp_memory_2_bp = make_write_port<BPInterface>("MEMORY_2_FETCH", PORT_BW, PORT_FANOUT);

    wp_execute_2_execute_bypass = make_write_port<uint64>("EXECUTE_2_EXECUTE_BYPASS", PORT_BW, SRC_REGISTERS_NUM);
    rps_stages_2_execute_sources_bypass[0][0] = make_read_port<uint64>("EXECUTE_2_EXECUTE_BYPASS", PORT_LATENCY);
    rps_stages_2_execute_sources_bypass[1][0] = make_read_port<uint64>("EXECUTE_2_EXECUTE_BYPASS", PORT_LATENCY);

    wp_memory_2_execute_bypass = make_write_port<uint64>("MEMORY_2_EXECUTE_BYPASS", PORT_BW, SRC_REGISTERS_NUM);
    rps_stages_2_execute_sources_bypass[0][1] = make_read_port<uint64>("MEMORY_2_EXECUTE_BYPASS", PORT_LATENCY);
    rps_stages_2_execute_sources_bypass[1][1] = make_read_port<uint64>("MEMORY_2_EXECUTE_BYPASS", PORT_LATENCY);

    rps_stages_2_execute_sources_bypass[0][2] = make_read_port<uint64>("WRITEBACK_2_EXECUTE_BYPASS", PORT_LATENCY);
    rps_stages_2_execute_sources_bypass[1][2] = make_read_port<uint64>("WRITEBACK_2_EXECUTE_BYPASS", PORT_LATENCY);

    wps_decode_2_execute_command[0] = make_write_port<DataBypass::BypassCommand>("DECODE_2_EXECUTE_SRC1_COMMAND",
                                                                                 PORT_BW, PORT_FANOUT);
    rps_decode_2_execute_command[0] = make_read_port<DataBypass::BypassCommand>("DECODE_2_EXECUTE_SRC1_COMMAND",
                                                                                PORT_LATENCY);
    
    wps_decode_2_execute_command[1] = make_write_port<DataBypass::BypassCommand>("DECODE_2_EXECUTE_SRC2_COMMAND",
                                                                                 PORT_BW, PORT_FANOUT);
    rps_decode_2_execute_command[1] = make_read_port<DataBypass::BypassCommand>("DECODE_2_EXECUTE_SRC2_COMMAND",
                                                                                PORT_LATENCY);

    wp_decode_2_bypassing_unit = make_write_port<Instr>("DECODE_2_BYPASSING_UNIT", PORT_BW, PORT_FANOUT);
    rp_decode_2_bypassing_unit = make_read_port<Instr>("DECODE_2_BYPASSING_UNIT", PORT_LATENCY);

    rp_halt = make_read_port<bool>("WRITEBACK_2_CORE_HALT", PORT_LATENCY);
    wp_memory_2_writeback = make_write_port<Instr>("MEMORY_2_WRITEBACK", PORT_BW, PORT_FANOUT);

    init_ports();
}

template <typename ISA>
typename PerfSim<ISA>::Instr PerfSim<ISA>::read_instr( Cycle cycle)
{
    if (rp_decode_2_decode->is_ready( cycle))
    {
        rp_fetch_2_decode->ignore( cycle);
        return rp_decode_2_decode->read( cycle);
    }
    return rp_fetch_2_decode->read( cycle);
}

template <typename ISA>
void PerfSim<ISA>::set_PC( Addr value)
{
    wp_core_2_fetch_target->write( value, curr_cycle);
    writeback.set_PC( value);
}

template<typename ISA>
void PerfSim<ISA>::run( const std::string& tr,
                    uint64 instrs_to_run)
{
    memory = new Memory( tr);
    fetch.set_memory( memory);
    writeback.set_instrs_to_run( instrs_to_run);
    writeback.set_RF( rf.get());
    writeback.init_checker( tr);

    set_PC( memory->startPC());

    bypassing_unit = std::make_unique<DataBypass>();

    auto t_start = std::chrono::high_resolution_clock::now();

    while (true)
    {
        if (rp_halt->is_ready( curr_cycle) && rp_halt->read( curr_cycle))
            break;

        writeback.clock( curr_cycle);
        fetch.clock( curr_cycle);
        clock_decode( curr_cycle);
        clock_execute( curr_cycle);
        clock_memory( curr_cycle);
        curr_cycle.inc();

        check_ports( curr_cycle);
    }

    auto t_end = std::chrono::high_resolution_clock::now();

    auto executed_instrs = writeback.get_executed_instrs();
    auto time = std::chrono::duration<double, std::milli>(t_end - t_start).count();
    auto frequency = static_cast<double>( curr_cycle) / time; // cycles per millisecond = kHz
    auto ipc = 1.0 * executed_instrs / static_cast<double>( curr_cycle);
    auto simips = executed_instrs / time;

    std::cout << std::endl << "****************************"
              << std::endl << "instrs:     " << executed_instrs
              << std::endl << "cycles:     " << curr_cycle
              << std::endl << "IPC:        " << ipc
              << std::endl << "sim freq:   " << frequency << " kHz"
              << std::endl << "sim IPS:    " << simips    << " kips"
              << std::endl << "instr size: " << sizeof(Instr) << " bytes"
              << std::endl << "****************************"
              << std::endl;
}

template <typename ISA> 
void PerfSim<ISA>::clock_decode( Cycle cycle)
{
    sout << "decode  cycle " << std::dec << cycle << ": ";

    /* receive flush signal */
    const bool is_flush = rp_decode_flush->is_ready( cycle) && rp_decode_flush->read( cycle);

    /* update bypassing unit */
    bypassing_unit->update();

    /* trace new instruction if needed */
    if ( rp_decode_2_bypassing_unit->is_ready( cycle))
    {
        auto instr = rp_decode_2_bypassing_unit->read( cycle);
        bypassing_unit->trace_new_instr( instr);
    }

    /* branch misprediction */
    if ( is_flush)
    {
        /* ignoring the upcoming instruction as it is invalid */
        rp_fetch_2_decode->ignore( cycle);
        rp_decode_2_decode->ignore( cycle);

        sout << "flush\n";
        return;
    }
    /* check if there is something to process */
    if ( !rp_fetch_2_decode->is_ready( cycle) && !rp_decode_2_decode->is_ready( cycle))
    {
        sout << "bubble\n";
        return;
    }

    auto instr = read_instr( cycle);

    if ( bypassing_unit->is_stall( instr))
    {   
        // data hazard, stalling pipeline
        wp_decode_2_fetch_stall->write( true, cycle);
        wp_decode_2_decode->write( instr, cycle);
        sout << instr << " (data hazard)\n";
        return;   
    }

    for ( std::size_t src_index = 0; src_index < SRC_REGISTERS_NUM; src_index++)
    {
        if ( bypassing_unit->is_in_RF( instr, src_index))
        {
            rf->read_source( &instr, src_index);
        }
        else if ( bypassing_unit->is_bypassible( instr, src_index))
        {
            const auto bypass_command = bypassing_unit->get_bypass_command( instr, src_index);
            wps_decode_2_execute_command[ src_index]->write( bypass_command, cycle);
        }
    }

    /* notify bypassing unit about new instruction */
    wp_decode_2_bypassing_unit->write( instr, cycle);

    wp_decode_2_execute->write( instr, cycle);

    /* log */
    sout << instr << std::endl;
}

template <typename ISA>
void PerfSim<ISA>::clock_execute( Cycle cycle)
{
    sout << "execute cycle " << std::dec << cycle << ": ";

    /* receive flush signal */
    const bool is_flush = rp_execute_flush->is_ready( cycle) && rp_execute_flush->read( cycle);

    /* branch misprediction */
    if ( is_flush)
    {
        /* ignoring the upcoming instruction as it is invalid */
        if ( rp_decode_2_execute->is_ready( cycle))
        {
            const auto& instr = rp_decode_2_execute->read( cycle);
            bypassing_unit->cancel( instr);
        }

        /* ignoring information from command ports */
        for ( auto& port:rps_decode_2_execute_command)
            port->ignore( cycle);
        
        /* ignoring all bypassed data for source registers */
        for ( auto& rps_src_bypass_ports:rps_stages_2_execute_sources_bypass)
        {
            for ( auto& port:rps_src_bypass_ports)
                port->ignore( cycle);
        }
        
        sout << "flush\n";
        return;
    }

    /* check if there is something to process */
    if ( !rp_decode_2_execute->is_ready( cycle))
    {
        /* ignoring all bypassed data for source registers */
        for ( auto& rps_src_bypass_ports:rps_stages_2_execute_sources_bypass)
        {
            for ( auto& port:rps_src_bypass_ports)
                port->ignore( cycle);
        }

        sout << "bubble\n";
        return;
    }

    auto instr = rp_decode_2_execute->read( cycle);


    void (MIPSInstr::*setters_v_sources[SRC_REGISTERS_NUM])( uint32) = 
    {
        &MIPSInstr::set_v_src1,
        &MIPSInstr::set_v_src2
    };

    for ( std::size_t src_index = 0; src_index < SRC_REGISTERS_NUM; src_index++)
    {   
        /* check whether bypassing is needed for a source register */ 
        if ( rps_decode_2_execute_command[src_index]->is_ready( cycle))
        {
            const auto bypass_command = rps_decode_2_execute_command[src_index]->read( cycle);

            /* get a port which should be used for bypassing and receive data */
            const auto bypass_direction = bypassing_unit->get_bypass_direction( bypass_command);
            const auto data = rps_stages_2_execute_sources_bypass[src_index][bypass_direction]->read( cycle);

            /* ignoring all other ports for a source register */
            for ( std::size_t i = 0; i < DataBypass::RegisterStage::get_bypassing_stages_number(); i++)
            {    
                if ( i != bypass_direction)
                    rps_stages_2_execute_sources_bypass[src_index][i]->ignore( cycle);
            }

            /* transform received data in accordance with bypass command */
            const auto adapted_data = bypassing_unit->adapt_bypassed_data( bypass_command, data);

            (instr.*setters_v_sources[src_index])( adapted_data);
        }
        else
        {
            /* ignoring all bypassed data for a source register */
            for ( auto& port:rps_stages_2_execute_sources_bypass[src_index])
                port->ignore( cycle);    
        }
    }
    

    /* perform execution */
    instr.execute();
    
    /* bypass data */
    wp_execute_2_execute_bypass->write( instr.get_v_dst(), cycle);

    wp_execute_2_memory->write( instr, cycle);

    /* log */
    sout << instr << std::endl;
}

template <typename ISA>
void PerfSim<ISA>::clock_memory( Cycle cycle)
{
    sout << "memory  cycle " << std::dec << cycle << ": ";

    /* receieve flush signal */
    const bool is_flush = rp_memory_flush->is_ready( cycle) && rp_memory_flush->read( cycle);

    /* branch misprediction */
    if ( is_flush)
    {
        /* drop instruction as it is invalid */
        if ( rp_execute_2_memory->is_ready( cycle))
        {
            const auto& instr = rp_execute_2_memory->read( cycle);
            bypassing_unit->cancel( instr);
        }
        sout << "flush\n";
        return;
    }

    /* check if there is something to process */
    if ( !rp_execute_2_memory->is_ready( cycle))
    {
        sout << "bubble\n";
        return;
    }

    auto instr = rp_execute_2_memory->read( cycle);

    if ( instr.is_jump()) {
        /* acquiring real information for BPU */
        wp_memory_2_bp->write( instr.get_bp_upd(), cycle);
        
        /* handle misprediction */
        if ( instr.is_misprediction())
        {
            /* flushing the pipeline */
            wp_memory_2_all_flush->write( true, cycle);

            /* sending valid PC to fetch stage */
            wp_memory_2_fetch_target->write( instr.get_new_PC(), cycle);
            sout << "misprediction on ";
        }
    }

    /* perform required loads and stores */
    memory->load_store( &instr);
    
    /* bypass data */
    wp_memory_2_execute_bypass->write( instr.get_v_dst(), cycle);

    wp_memory_2_writeback->write( instr, cycle);

    /* log */
    sout << instr << std::endl;
}

#include <mips/mips.h>

template class PerfSim<MIPS>;

