/* This is an open source non-commercial project. Dear PVS-Studio, please check it.
 * PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
*/
#include <iostream>

#include <boost/chrono.hpp>
#include <boost/timer/timer.hpp>

#include <infra/config/config.h>

#include <mips/mips_memory.h>
#include <mips/mips_rf.h>

#include "perf_sim.h"

static const uint32 PORT_LATENCY = 1;
static const uint32 PORT_FANOUT = 1;
static const uint32 PORT_BW = 1;
static const uint32 FLUSHED_STAGES_NUM = 4;

namespace config {
    static Value<std::string> bp_mode = { "bp-mode", "dynamic_two_bit", "branch prediction mode"};
    static Value<uint32> bp_size = { "bp-size", 128, "BTB size in entries"};
    static Value<uint32> bp_ways = { "bp-ways", 16, "number of ways in BTB"};
} // namespace config

PerfMIPS::PerfMIPS(bool log) : Log( log), rf( new RF), checker( false)
{
    executed_instrs = 0;

    wp_fetch_2_decode = make_write_port<IfIdData>("FETCH_2_DECODE", PORT_BW, PORT_FANOUT);
    rp_fetch_2_decode = make_read_port<IfIdData>("FETCH_2_DECODE", PORT_LATENCY);
    wp_decode_2_fetch_stall = make_write_port<bool>("DECODE_2_FETCH_STALL", PORT_BW, PORT_FANOUT);
    rp_decode_2_fetch_stall = make_read_port<bool>("DECODE_2_FETCH_STALL", PORT_LATENCY);

    wp_decode_2_decode = make_write_port<FuncInstr>("DECODE_2_DECODE", PORT_BW, PORT_FANOUT);
    rp_decode_2_decode = make_read_port<FuncInstr>("DECODE_2_DECODE", PORT_LATENCY);

    wp_decode_2_execute = make_write_port<FuncInstr>("DECODE_2_EXECUTE", PORT_BW, PORT_FANOUT);
    rp_decode_2_execute = make_read_port<FuncInstr>("DECODE_2_EXECUTE", PORT_LATENCY);

    wp_execute_2_memory = make_write_port<FuncInstr>("EXECUTE_2_MEMORY", PORT_BW, PORT_FANOUT);
    rp_execute_2_memory = make_read_port<FuncInstr>("EXECUTE_2_MEMORY", PORT_LATENCY);

    wp_memory_2_writeback = make_write_port<FuncInstr>("MEMORY_2_WRITEBACK", PORT_BW, PORT_FANOUT);
    rp_memory_2_writeback = make_read_port<FuncInstr>("MEMORY_2_WRITEBACK", PORT_LATENCY);

    /* branch misprediction unit ports */
    wp_memory_2_all_flush = make_write_port<bool>("MEMORY_2_ALL_FLUSH", PORT_BW, FLUSHED_STAGES_NUM);
    rp_fetch_flush = make_read_port<bool>("MEMORY_2_ALL_FLUSH", PORT_LATENCY);
    rp_decode_flush = make_read_port<bool>("MEMORY_2_ALL_FLUSH", PORT_LATENCY);
    rp_execute_flush = make_read_port<bool>("MEMORY_2_ALL_FLUSH", PORT_LATENCY);
    rp_memory_flush = make_read_port<bool>("MEMORY_2_ALL_FLUSH", PORT_LATENCY);

    wp_memory_2_fetch_target = make_write_port<Addr>("MEMORY_2_FETCH_TARGET", PORT_BW, PORT_FANOUT);
    rp_memory_2_fetch_target = make_read_port<Addr>("MEMORY_2_FETCH_TARGET", PORT_LATENCY);

    BPFactory bp_factory;
    bp = bp_factory.create( config::bp_mode, config::bp_size, config::bp_ways);

    init_ports();
}

FuncInstr PerfMIPS::read_instr(uint64 cycle)
{
    if (rp_decode_2_decode->is_ready( cycle))
    {
        rp_fetch_2_decode->ignore( cycle);
        return rp_decode_2_decode->read( cycle);
    }
    const auto& _data = rp_fetch_2_decode->read( cycle);
    FuncInstr instr( _data.raw,
        _data.PC,
        _data.predicted_taken,
        _data.predicted_target);
    return instr;

}



void PerfMIPS::run( const std::string& tr,
                    uint64 instrs_to_run)
{
    assert( instrs_to_run < MAX_VAL32);
    Cycles cycle = 0;

    memory = new MIPSMemory( tr);

    checker.init( tr);

    new_PC = memory->startPC();

    boost::timer::cpu_timer timer;

    while (executed_instrs < instrs_to_run)
    {
        clock_writeback( cycle);
        clock_fetch( cycle);
        clock_decode( cycle);
        clock_execute( cycle);
        clock_memory( cycle);
        ++cycle;

        sout << "Executed instructions: " << executed_instrs
             << std::endl << std::endl;

        check_ports( cycle);
    }

    auto time = timer.elapsed().wall;
    auto frequency = 1e6 * cycle / time;
    auto ipc = 1.0 * executed_instrs / cycle;
    auto simips = 1e6 * executed_instrs / time;

    std::cout << std::endl << "****************************"
              << std::endl << "instrs:   " << executed_instrs
              << std::endl << "cycles:   " << cycle
              << std::endl << "IPC:      " << ipc
              << std::endl << "sim freq: " << frequency << " kHz"
              << std::endl << "sim IPS:  " << simips    << " kips"
              << std::endl << "****************************"
              << std::endl;
}

void PerfMIPS::clock_fetch( int cycle)
{
    /* receive flush and stall signals */
    const bool is_flush = rp_fetch_flush->is_ready( cycle) && rp_fetch_flush->read( cycle);
    const bool is_stall = rp_decode_2_fetch_stall->is_ready( cycle) && rp_decode_2_fetch_stall->read( cycle);

    /* updating PC */
    if ( is_flush)
        PC = rp_memory_2_fetch_target->read( cycle); // fixing PC
    else if ( !is_stall)
        PC = new_PC;

    /* creating structure to be sent to decode stage */
    IfIdData data;

    /* fetching instruction */
    data.raw = memory->fetch( PC);

    /* saving predictions and updating PC according to them */
    data.PC = PC;
    data.predicted_taken = bp->is_taken( PC);
    data.predicted_target = bp->get_target( PC);

    /* updating PC according to prediction */
    new_PC = data.predicted_target;

    /* sending to decode */
    wp_fetch_2_decode->write( data, cycle);

    /* log */
    sout << "fetch   cycle " << std::dec << cycle << ": 0x"
         << std::hex << PC << ": 0x" << data.raw << std::endl;
}

void PerfMIPS::clock_decode( int cycle)
{
    sout << "decode  cycle " << std::dec << cycle << ": ";

    /* receive flush signal */
    const bool is_flush = rp_decode_flush->is_ready( cycle) && rp_decode_flush->read( cycle);

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

    /* TODO: replace all this code by introducing Forwarding unit */
    if( rf->check_sources( instr))
    {
        rf->read_sources( &instr);

        wp_decode_2_execute->write( instr, cycle);

        /* log */
        sout << instr << std::endl;
    }
    else // data hazard, stalling pipeline
    {
        wp_decode_2_fetch_stall->write( true, cycle);
        wp_decode_2_decode->write( instr, cycle);
        sout << instr << " (data hazard)\n";
    }
}

void PerfMIPS::clock_execute( int cycle)
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
            rf->cancel( instr);
        }
        sout << "flush\n";
        return;
    }

    /* check if there is something to process */
    if ( !rp_decode_2_execute->is_ready( cycle))
    {
        sout << "bubble\n";
        return;
    }

    auto instr = rp_decode_2_execute->read( cycle);

    /* preform execution */
    instr.execute();

    wp_execute_2_memory->write( instr, cycle);

    /* log */
    sout << instr << std::endl;
}

void PerfMIPS::clock_memory( int cycle)
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
            rf->cancel( instr);
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

    if (instr.is_jump()) {
        /* acquiring real information for BPU */
        bool actually_taken = instr.is_jump_taken();
        Addr real_target = instr.get_new_PC();
        bp->update( actually_taken, instr.get_PC(), real_target);

        /* handle misprediction */
        if ( instr.is_misprediction())
        {
            /* flushing the pipeline */
            wp_memory_2_all_flush->write( true, cycle);

            /* sending valid PC to fetch stage */
            wp_memory_2_fetch_target->write( real_target, cycle);

            sout << "misprediction on ";
        }
    }

    /* perform required loads and stores */
    memory->load_store( &instr);

    wp_memory_2_writeback->write( instr, cycle);

    /* log */
    sout << instr << std::endl;
}

void PerfMIPS::clock_writeback( int cycle)
{
    sout << "wb      cycle " << std::dec << cycle << ": ";

    /* check if there is something to process */
    if ( !rp_memory_2_writeback->is_ready( cycle))
    {
        sout << "bubble\n";
        if ( cycle - last_writeback_cycle >= 10)
        {
            serr << "Deadlock was detected. The process will be aborted."
                 << std::endl << std::endl << critical;
        }
        return;
    }

    FuncInstr instr = rp_memory_2_writeback->read( cycle);

    /* perform writeback */
    rf->write_dst( instr);

    /* check for traps */
    instr.check_trap();

    /* log */
    sout << instr << std::endl;

    /* perform checks */
    check( instr);

    /* update simulator cycles info */
    ++executed_instrs;
    last_writeback_cycle = cycle;
}

void PerfMIPS::check( const FuncInstr& instr)
{
    const auto func_dump = checker.step();

    if ( func_dump.Dump() != instr.Dump())
        serr << "****************************" << std::endl
             << "Mismatch: " << std::endl
             << "Checker output: " << func_dump    << std::endl
             << "PerfSim output: " << instr.Dump() << std::endl
             << critical;
}

