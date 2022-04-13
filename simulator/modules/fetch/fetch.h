/*
 * fetch.h - simulator of fetch unit
 * Copyright 2015-2018 MIPT-MIPS
 */

#ifndef FETCH_H
#define FETCH_H

#include "bpu/bpu.h"

#include <func_sim/instr_memory.h>
#include <infra/cache/cache_tag_array.h>
#include <modules/core/perf_instr.h>
#include <modules/ports_instance.h>
 
template <typename FuncInstr>
class Fetch : public Module
{
    using Instr = PerfInstr<FuncInstr>;

public:
    explicit Fetch( Module* parent);
    void clock( Cycle cycle);
    void set_memory( std::unique_ptr<InstrMemoryIface<FuncInstr>> mem)
    {
        memory = std::move( mem);
    }

private:
    std::unique_ptr<InstrMemoryIface<FuncInstr>> memory = nullptr;
    std::unique_ptr<BaseBP> bp = nullptr;
    std::unique_ptr<CacheTagArray> tags = nullptr;
    
    /* Input signals */
    ReadPort<bool>* rp_stall = make_read_port<bool>("DECODE_2_FETCH_STALL", Port::LATENCY);
    ReadPort<bool>* rp_hit_or_miss = make_read_port<bool>("HIT_OR_MISS", Port::LATENCY);

    /* Input signals - BP */
    ReadPort<BPInterface>* rp_bp_update = make_read_port<BPInterface>("BRANCH_2_FETCH", Port::LATENCY);
    ReadPort<BPInterface>* rp_bp_update_from_decode = make_read_port<BPInterface>("DECODE_2_FETCH", Port::LATENCY);
    
    /* Input signals - PC values */
    ReadPort<Target>* rp_flush_target = make_read_port<Target>("BRANCH_2_FETCH_TARGET", Port::LATENCY);
    ReadPort<Target>* rp_external_target = make_read_port<Target>("WRITEBACK_2_FETCH_TARGET", Port::LATENCY);
    ReadPort<Target>* rp_hold_pc = make_read_port<Target>("HOLD_PC", Port::LATENCY);
    ReadPort<Target>* rp_target = make_read_port<Target>("TARGET", Port::LATENCY);
    ReadPort<Target>* rp_long_latency_pc_holder = make_read_port<Target>("LONG_LATENCY_PC_HOLDER", Port::LONG_LATENCY);

    /* Outputs */
    WritePort<Instr>* wp_datapath = make_write_port<Instr>("FETCH_2_DECODE", Port::BW);
    WritePort<Target>* wp_hold_pc = make_write_port<Target>("HOLD_PC", Port::BW);
    WritePort<Target>* wp_target = make_write_port<Target>("TARGET", Port::BW);
    WritePort<Target>* wp_long_latency_pc_holder = make_write_port<Target>("LONG_LATENCY_PC_HOLDER", Port::BW);
    WritePort<bool>* wp_hit_or_miss = make_write_port<bool>("HIT_OR_MISS", Port::BW);

    /* port needed for handling misprediction at decode stage */
    ReadPort<Target>* rp_flush_target_from_decode = make_read_port<Target>("DECODE_2_FETCH_TARGET", Port::LATENCY);

    Target get_target( Cycle cycle);
    Target get_cached_target( Cycle cycle);
    void clock_bp( Cycle cycle);
    void clock_instr_cache( Cycle cycle);
    void save_flush( Cycle cycle);

    uint32 _fetchahead_size; // value of fetchahead distance size
    std::string _prefetch_method; // value of prefetch method

    bool is_wrong_path = false;
    void prefetch_next_line(Addr requested_addr);
};

struct PrefetchMethodException final : Exception
{
    explicit PrefetchMethodException( const std::string& msg)
            : Exception( "Invalid prefetch method name", msg)
    { }
};

#endif
