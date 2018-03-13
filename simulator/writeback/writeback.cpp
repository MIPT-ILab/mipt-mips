#include <cassert>

#include <iostream>
#include <chrono>

#include "writeback.h"

template <typename ISA>
Writeback<ISA>::Writeback(bool log) : Log( log), checker( false)
{
    rp_datapath = make_read_port<Instr>("MEMORY_2_WRITEBACK", PORT_LATENCY);
    wp_bypass = make_write_port<uint64>("WRITEBACK_2_EXECUTE_BYPASS", PORT_BW, SRC_REGISTERS_NUM);
    wp_halt = make_write_port<bool>("WRITEBACK_2_CORE_HALT", PORT_BW, PORT_FANOUT);
}

template <typename ISA>
void Writeback<ISA>::clock( Cycle cycle)
{
    sout << "wb      cycle " << std::dec << cycle << ": ";

    /* check if there is something to process */
    if ( !rp_datapath->is_ready( cycle))
    {
        sout << "bubble\n";
        if ( cycle >= last_writeback_cycle + 100_Lt)
        {
            serr << "Deadlock was detected. The process will be aborted."
                 << std::endl << std::endl << critical;
        }
        return;
    }

    auto instr = rp_datapath->read( cycle);

    /* perform writeback */
    rf->write_dst( instr);

    /* check for bubble */
    if ( instr.is_bubble())
        return;

    /* check for traps */
    instr.check_trap();

    /* bypass data */
    wp_bypass->write( instr.get_bypassing_data(), cycle);

    /* log */
    sout << instr << std::endl;

    /* perform checks */
    check( instr);

    /* update simulator cycles info */
    ++executed_instrs;
    last_writeback_cycle = cycle;
    if ( executed_instrs >= instrs_to_run || instr.is_halt())
        wp_halt->write( true, cycle);
    
    sout << "Executed instructions: " << executed_instrs
         << std::endl << std::endl;
}

template <typename ISA>
void Writeback<ISA>::check( const FuncInstr& instr)
{
    const auto func_dump = checker.step();

    if ( func_dump.Dump() != instr.Dump())
        serr << "Mismatch: " << std::endl
             << "Checker output: " << func_dump    << std::endl
             << "PerfSim output: " << instr.Dump() << std::endl
             << critical;
}

#include <mips/mips.h>

template class Writeback<MIPS>;

