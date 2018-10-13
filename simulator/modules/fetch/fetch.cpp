/*
 * fetch.cpp - simulator of fetch unit
 * Copyright 2015-2018 MIPT-MIPS
 */

#include <infra/config/config.h>

#include "fetch.h"

namespace config {
    /* Cache parameters */
    static Value<uint32> instruction_cache_size = { "icache-size", 2048, "Size of instruction level 1 cache (in bytes)"};
    static Value<uint32> instruction_cache_ways = { "icache-ways", 4, "Amount of ways in instruction level 1 cache"};
    static Value<uint32> instruction_cache_line_size = { "icache-line-size", 64, "Line size of instruction level 1 cache (in bytes)"};
} // namespace config

template <typename ISA>
Fetch<ISA>::Fetch(bool log) : Log( log)
{
    wp_datapath = make_write_port<Instr>("FETCH_2_DECODE", PORT_BW, PORT_FANOUT);
    rp_stall = make_read_port<bool>("DECODE_2_FETCH_STALL", PORT_LATENCY);

    rp_flush_target = make_read_port<Target>("MEMORY_2_FETCH_TARGET", PORT_LATENCY);

    wp_target = make_write_port<Target>("TARGET", PORT_BW, PORT_FANOUT);
    rp_target = make_read_port<Target>("TARGET", PORT_LATENCY);

    wp_hold_pc = make_write_port<Target>("HOLD_PC", PORT_BW, PORT_FANOUT);
    rp_hold_pc = make_read_port<Target>("HOLD_PC", PORT_LATENCY);

    rp_external_target = make_read_port<Target>("CORE_2_FETCH_TARGET", PORT_LATENCY);

    rp_bp_update = make_read_port<BPInterface>("MEMORY_2_FETCH", PORT_LATENCY);

    wp_long_latency_pc_holder = make_write_port<Target>("LONG_LATENCY_PC_HOLDER", PORT_BW, PORT_FANOUT);
    rp_long_latency_pc_holder = make_read_port<Target>("LONG_LATENCY_PC_HOLDER", PORT_LONG_LATENCY);

    wp_hit_or_miss = make_write_port<bool>("HIT_OR_MISS", PORT_BW, PORT_FANOUT);
    rp_hit_or_miss = make_read_port<bool>("HIT_OR_MISS", PORT_LATENCY);

    bp = BaseBP::create_configured_bp();
    tags = std::make_unique<CacheTagArray>( config::instruction_cache_size,
                                            config::instruction_cache_ways,
                                            config::instruction_cache_line_size);
}

template <typename ISA>
Target Fetch<ISA>::get_target( Cycle cycle)
{
    /* receive flush and stall signals */
    const bool is_stall = rp_stall->is_ready( cycle) && rp_stall->read( cycle);

    /* Receive all possible PC */
    const Target external_target = rp_external_target->is_ready( cycle) ? rp_external_target->read( cycle) : Target();
    const Target hold_target     = rp_hold_pc->is_ready( cycle) ? rp_hold_pc->read( cycle) : Target();
    const Target flushed_target  = rp_flush_target->is_ready( cycle) ? rp_flush_target->read( cycle) : Target();
    const Target branch_target   = rp_target->is_ready( cycle) ? rp_target->read( cycle) : Target();

    /* Multiplexing */
    if ( external_target.valid)
        return external_target;

    if( flushed_target.valid)
        return flushed_target;

    if ( !is_stall && branch_target.valid)
        return branch_target;

    if ( hold_target.valid)
        return hold_target;

    return Target();
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
    if( rp_long_latency_pc_holder->is_ready( cycle))
    {
        /* get PC from long-latency port if it's possible */
        auto target = rp_long_latency_pc_holder->read( cycle);

        /* write new PC to tags array */
        tags->write( target.address);

        /* save PC to the next stage */
        wp_hold_pc->write( target, cycle);
        return;
    }
    wp_hit_or_miss->write( false, cycle);
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
Target Fetch<ISA>::get_cached_target( Cycle cycle)
{
    /* simulate request to the memory in the case of cache miss */
    if ( rp_hit_or_miss->is_ready( cycle))
    {
        save_flush( cycle);
        clock_instr_cache( cycle);
        return Target();
    }

    /* getting PC */
    auto target = get_target( cycle);

    /* push bubble */
    if ( !target.valid)
        return Target();

    /* hit or miss */
    auto is_hit = tags->lookup( target.address);

    if ( is_hit)
        return target;

    /* send miss to the next cycle */
    wp_hit_or_miss->write( is_hit, cycle);

    /* send PC to cache*/
    wp_long_latency_pc_holder->write( target, cycle);
    return Target();
}


template <typename ISA>
void Fetch<ISA>::clock( Cycle cycle)
{
    clock_bp( cycle);

    /* getting PC */
    auto target = get_cached_target( cycle);

    /* push bubble */
    if ( !target.valid)
        return;

    /* hold PC for the stall case */
    wp_hold_pc->write( target, cycle);

    Instr instr( memory.fetch_instr( target.address), bp->get_bp_info( target.address));
    instr.set_sequence_id( target.sequence_id);

    /* set next target according to prediction */
    wp_target->write( instr.get_predicted_target(), cycle);

    /* log */
    sout << "fetch   cycle " << std::dec << cycle << ": " << instr << std::endl;

    /* sending to decode */
    wp_datapath->write( std::move( instr), cycle);
}

#include <mips/mips.h>
#include <risc_v/risc_v.h>

template class Fetch<MIPSI>;
template class Fetch<MIPSII>;
template class Fetch<MIPSIII>;
template class Fetch<MIPSIV>;
template class Fetch<MIPS32>;
template class Fetch<MIPS64>;
template class Fetch<RISCV32>;
template class Fetch<RISCV64>;
template class Fetch<RISCV128>;
