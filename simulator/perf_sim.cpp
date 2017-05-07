#include <iostream>

#include <boost/timer/timer.hpp>

#include <mips/mips_memory.h>
#include <mips/mips_rf.h>

#include "perf_sim.h"

static const uint32 PORT_LATENCY = 1;
static const uint32 PORT_FANOUT = 1;
static const uint32 PORT_BW = 1;

PerfMIPS::PerfMIPS(bool log) : Log( log), rf( new RF), checker()
{
    executed_instrs = 0;

    wp_fetch_2_decode = make_write_port<uint32>("FETCH_2_DECODE", PORT_BW, PORT_FANOUT);
    rp_fetch_2_decode = make_read_port<uint32>("FETCH_2_DECODE", PORT_LATENCY);
    wp_decode_2_fetch_stall = make_write_port<bool>("DECODE_2_FETCH_STALL", PORT_BW, PORT_FANOUT);
    rp_decode_2_fetch_stall = make_read_port<bool>("DECODE_2_FETCH_STALL", PORT_LATENCY);

    wp_decode_2_execute = make_write_port<FuncInstr>("DECODE_2_EXECUTE", PORT_BW, PORT_FANOUT);
    rp_decode_2_execute = make_read_port<FuncInstr>("DECODE_2_EXECUTE", PORT_LATENCY);
    wp_execute_2_decode_stall = make_write_port<bool>("EXECUTE_2_DECODE_STALL", PORT_BW, PORT_FANOUT);
    rp_execute_2_decode_stall = make_read_port<bool>("EXECUTE_2_DECODE_STALL", PORT_LATENCY);

    wp_execute_2_memory = make_write_port<FuncInstr>("EXECUTE_2_MEMORY", PORT_BW, PORT_FANOUT);
    rp_execute_2_memory = make_read_port<FuncInstr>("EXECUTE_2_MEMORY", PORT_LATENCY);
    wp_memory_2_execute_stall = make_write_port<bool>("MEMORY_2_EXECUTE_STALL", PORT_BW, PORT_FANOUT);
    rp_memory_2_execute_stall = make_read_port<bool>("MEMORY_2_EXECUTE_STALL", PORT_LATENCY);

    wp_memory_2_writeback = make_write_port<FuncInstr>("MEMORY_2_WRITEBACK", PORT_BW, PORT_FANOUT);
    rp_memory_2_writeback = make_read_port<FuncInstr>("MEMORY_2_WRITEBACK", PORT_LATENCY);
    wp_writeback_2_memory_stall = make_write_port<bool>("WRITEBACK_2_MEMORY_STALL", PORT_BW, PORT_FANOUT);
    rp_writeback_2_memory_stall = make_read_port<bool>("WRITEBACK_2_MEMORY_STALL", PORT_LATENCY);

    init_ports();
}

void PerfMIPS::run( const std::string& tr, uint64 instrs_to_run)
{
    assert( instrs_to_run < MAX_VAL32);
    uint64 cycle = 0;

    decode_next_time = false;

    mem = new MIPSMemory( tr);
    checker.init( tr);

    PC = mem->startPC();
    PC_is_valid = true;

    boost::timer::cpu_timer timer;

    while (executed_instrs < instrs_to_run)
    {
        clock_writeback( cycle);
        clock_decode( cycle);
        clock_fetch( cycle);
        clock_execute( cycle);
        clock_memory( cycle);
        ++cycle;

        sout << "Executed instructions: " << executed_instrs << std::endl << std::endl;

        check_ports( cycle);
    }

    auto time = timer.elapsed().wall;
    auto frequency = 1e6 * cycle / time;
    auto ipc = 1.0 * executed_instrs / cycle;
    auto simips = 1e6 * executed_instrs / time;

    std::cout << std::endl << "****************************"
              << std::endl << "cycles:   " << cycle
              << std::endl << "IPC:      " << ipc
              << std::endl << "sim freq: " << frequency << " kHz"
              << std::endl << "sim IPS:  " << simips    << " kips"
              << std::endl << "****************************"
              << std::endl;

    delete mem;
}

void PerfMIPS::clock_fetch( int cycle) {
    sout << "fetch   cycle " << std::dec << cycle << ":";

    bool is_stall = false;
    rp_decode_2_fetch_stall->read( &is_stall, cycle);
    if ( is_stall)
    {
        sout << "bubble\n";
        return;
    }

    if (PC_is_valid)
    {
        uint32 module_data = mem->fetch( PC);
        wp_fetch_2_decode->write( module_data, cycle);

        sout << std::hex << "0x" << module_data << std::endl;
    }
    else
    {
        sout << "bubble\n";
    }
}

void PerfMIPS::clock_decode( int cycle) {
    sout << "decode  cycle " << std::dec << cycle << ":";

    bool is_stall = false;
    rp_execute_2_decode_stall->read( &is_stall, cycle);
    if ( is_stall) {
        wp_decode_2_fetch_stall->write( true, cycle);

        sout << "bubble\n";
        return;
    }

    bool is_anything_from_fetch = rp_fetch_2_decode->read( &decode_data, cycle);

    FuncInstr instr( decode_data, PC);

    if ( instr.isJump() && is_anything_from_fetch)
        PC_is_valid = false;

    if ( !is_anything_from_fetch && !decode_next_time)
    {
        sout << "bubble\n";
        return;
    }

    if ( rf->check_sources( instr))
    {
        rf->read_sources( instr);
        wp_decode_2_execute->write( instr, cycle);

        decode_next_time = false;

        if (!instr.isJump())
            PC += 4;

        sout << instr << std::endl;
    }
    else
    {
        wp_decode_2_fetch_stall->write( true, cycle);
        decode_next_time = true;
        sout << "bubble\n";
    }
}

void PerfMIPS::clock_execute( int cycle)
{
    std::ostringstream oss;
    sout << "execute cycle " << std::dec << cycle << ":";

    bool is_stall = false;
    rp_memory_2_execute_stall->read( &is_stall, cycle);
    if ( is_stall)
    {
        wp_execute_2_decode_stall->write( true, cycle);

        sout << "bubble\n";
        return;
    }

    FuncInstr instr;
    if ( !rp_decode_2_execute->read( &instr, cycle))
    {
        sout << "bubble\n";
        return;
    }

    instr.execute();
    wp_execute_2_memory->write( instr, cycle);

    sout << instr << std::endl;
}

void PerfMIPS::clock_memory( int cycle)
{
    sout << "memory  cycle " << std::dec << cycle << ":";

    bool is_stall = false;
    rp_writeback_2_memory_stall->read( &is_stall, cycle);
    if ( is_stall)
    {
        wp_memory_2_execute_stall->write( true, cycle);
        sout << "bubble\n";
        return;
    }

    FuncInstr instr;
    if ( !rp_execute_2_memory->read( &instr, cycle))
    {
        sout << "bubble\n";
        return;
    }

    mem->load_store( instr);
    wp_memory_2_writeback->write( instr, cycle);

    sout << instr << std::endl;
}

void PerfMIPS::clock_writeback( int cycle)
{
    sout << "wb      cycle " << std::dec << cycle << ":";

    FuncInstr instr;
    if ( !rp_memory_2_writeback->read( &instr, cycle))
    {
        sout << "bubble\n";
        if ( cycle - last_writeback_cycle >= 1000)
        {
            serr << "Deadlock was detected. The process will be aborted."
                 << std::endl << std::endl << critical;
        }
        return;
    }

    if ( instr.isJump())
    {
        PC_is_valid = true;
        PC = instr.get_new_PC();
    }

    rf->write_dst( instr);

    sout << instr << std::endl;

    check(instr);

    ++executed_instrs;
    last_writeback_cycle = cycle;
}

void PerfMIPS::check( const FuncInstr& instr)
{
    const std::string func_dump = checker.step();

    if ( func_dump != instr.Dump())
        serr << "****************************" << std::endl
             << "Mismatch: " << std::endl
             << "Checker output: " << func_dump    << std::endl
             << "PerfSim output: " << instr.Dump() << std::endl
             << critical;
}

