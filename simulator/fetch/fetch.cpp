/*
 * fetch.cpp - simulator of fetch unit
 * Copyright 2015-2018 MIPT-MIPS
 */

#include <infra/config/config.h>
 
#include "fetch.h"

namespace config {
    static Value<std::string> bp_mode = { "bp-mode", "dynamic_two_bit", "branch prediction mode"};
    static Value<uint32> bp_size = { "bp-size", 128, "BTB size in entries"};
    static Value<uint32> bp_ways = { "bp-ways", 16, "number of ways in BTB"};
} // namespace config

template <typename ISA>
Fetch<ISA>::Fetch(bool log) : Log( log)
{
    wp_datapath = make_write_port<Instr>("FETCH_2_DECODE", PORT_BW, PORT_FANOUT);
    rp_stall = make_read_port<bool>("DECODE_2_FETCH_STALL", PORT_LATENCY);

    rp_flush_target = make_read_port<Addr>("MEMORY_2_FETCH_TARGET", PORT_LATENCY);

    wp_target = make_write_port<Addr>("TARGET", PORT_BW, PORT_FANOUT);
    rp_target = make_read_port<Addr>("TARGET", PORT_LATENCY);

    wp_hold_pc = make_write_port<Addr>("HOLD_PC", PORT_BW, PORT_FANOUT);
    rp_hold_pc = make_read_port<Addr>("HOLD_PC", PORT_LATENCY);

    rp_external_target = make_read_port<Addr>("CORE_2_FETCH_TARGET", PORT_LATENCY);

    rp_bp_update = make_read_port<BPInterface>("MEMORY_2_FETCH", PORT_LATENCY);

    wp_long_latency_pc_holder = make_write_port<Addr>("LONG_LATENCY_PC_HOLDER", PORT_BW, PORT_FANOUT);
    rp_long_latency_pc_holder = make_read_port<Addr>("LONG_LATENCY_PC_HOLDER", PORT_LONG_LATENCY);

    wp_hit_or_miss = make_write_port<bool>("HIT_OR_MISS", PORT_BW, PORT_FANOUT);
    rp_hit_or_miss = make_read_port<bool>("HIT_OR_MISS", PORT_LATENCY);

    BPFactory bp_factory;
    bp = bp_factory.create( config::bp_mode, config::bp_size, config::bp_ways);
    tags = std::make_unique<CacheTagArray>( size_in_bytes, ways, line_size);

}

template <typename ISA>
Addr Fetch<ISA>::get_PC( Cycle cycle) 
{
    /* receive flush and stall signals */
    const bool is_stall = rp_stall->is_ready( cycle) && rp_stall->read( cycle);

    /* Receive all possible PC */
    const Addr external_PC = rp_external_target->is_ready( cycle) ? rp_external_target->read( cycle) : 0;
    const Addr hold_PC     = rp_hold_pc->is_ready( cycle) ? rp_hold_pc->read( cycle) : 0;
    const Addr flushed_PC  = rp_flush_target->is_ready( cycle) ? rp_flush_target->read( cycle) : 0;
    const Addr target_PC   = rp_target->is_ready( cycle) ? rp_target->read( cycle) : 0;

    /* Multiplexing */
    if ( external_PC != 0)
        return external_PC;

    if( flushed_PC != 0)
        return flushed_PC;

    if ( !is_stall && target_PC != 0)
        return target_PC;

    if ( hold_PC != 0)
        return hold_PC;
    
    return 0;
}

template <typename ISA>
void Fetch<ISA>::clock_bp( Cycle cycle)
{
    /* Process BP updates */
    if ( rp_bp_update->is_ready( cycle))
        bp->update( rp_bp_update->read( cycle));
}

template <typename ISA>
void Fetch<ISA>::clock_instr_cache( Cycle cycle) 
{
    ignore( cycle);
    
    if( rp_long_latency_pc_holder->is_ready( cycle))
    {
        /* get PC from long-latency port if it's possible */
        auto PC = rp_long_latency_pc_holder->read( cycle);
        
        /* write new PC to tags array */
        tags->write( PC);
        
        /* save PC to the next stage */
        wp_hold_pc->write( PC, cycle);
        return;
    }
    wp_hit_or_miss->write( false, cycle);
}

template <typename ISA>
void Fetch<ISA>::ignore( Cycle cycle)
{
    /* ignore PC from other ports in the case of cache miss */ 
    rp_external_target->ignore( cycle);
    rp_hold_pc->ignore( cycle);
    rp_target->ignore( cycle);

    /* ignore miss signal */
    rp_hit_or_miss->ignore( cycle);
}

template <typename ISA>
void Fetch<ISA>::save_flush( Cycle cycle)
{
    /* save PC in the case of flush signal */
    if( rp_flush_target->is_ready( cycle))
        wp_target->write( rp_flush_target->read( cycle), cycle);
    else if( rp_target->is_ready( cycle))
        wp_target->write( rp_target->read( cycle), cycle);
}



template <typename ISA>
Addr Fetch<ISA>::get_cached_PC( Cycle cycle)
{
    /* simulate request to the memory in the case of cache miss */
    if( rp_hit_or_miss->is_ready( cycle))
    {
        save_flush( cycle);
        clock_instr_cache( cycle);
        return 0;
    }

    /* getting PC */
    auto PC = get_PC( cycle);

    /* push bubble */
    if( PC == 0)
        return 0;

    /* hit or miss */
    auto is_hit = tags->lookup( PC);
    
    if( !is_hit)
    {
        /* send miss to the next cycle */
        wp_hit_or_miss->write( is_hit, cycle);

        /* send PC to cache*/
        wp_long_latency_pc_holder->write( PC, cycle);
        return 0;
    }
    return PC;
}


template <typename ISA>
void Fetch<ISA>::clock( Cycle cycle)
{
    clock_bp( cycle);

    /* getting PC */
    auto PC = get_cached_PC( cycle);

    /* push bubble */
    if( PC == 0)
        return;  

    /* hold PC for the stall case */
    wp_hold_pc->write( PC, cycle);

    Instr instr( memory->fetch_instr( PC), bp->get_bp_info( PC));

    /* updating PC according to prediction */
    wp_target->write( instr.get_predicted_target(), cycle);

    /* sending to decode */
    wp_datapath->write( instr, cycle);

    /* log */
    sout << "fetch   cycle " << std::dec << cycle << ": 0x"
         << std::hex << PC << ": 0x" << instr << std::endl;

}

#include <mips/mips.h>

template class Fetch<MIPS>;

