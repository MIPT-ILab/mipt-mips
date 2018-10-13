#include "writeback.h"

#include <cassert>
#include <sstream>

template <typename ISA>
Writeback<ISA>::Writeback(bool log) : Log( log), checker( false)
{
    rp_mem_datapath = make_read_port<Instr>("MEMORY_2_WRITEBACK", PORT_LATENCY);
    rp_execute_datapath = make_read_port<Instr>("EXECUTE_2_WRITEBACK", PORT_LATENCY);
    wp_bypass = make_write_port<std::pair<RegisterUInt, RegisterUInt>>("WRITEBACK_2_EXECUTE_BYPASS", PORT_BW, SRC_REGISTERS_NUM);
    wp_halt = make_write_port<bool>("WRITEBACK_2_CORE_HALT", PORT_BW, PORT_FANOUT);
}

template <typename ISA>
void Writeback<ISA>::init_checker( const FuncMemory& mem)
{
    checker.init( mem);
}

template <typename ISA>
void Writeback<ISA>::clock( Cycle cycle)
{
    sout << "wb      cycle " << std::dec << cycle << ": ";

    /* check if there is something to process */
    if ( !rp_mem_datapath->is_ready( cycle) && !rp_execute_datapath->is_ready( cycle))
    {
        sout << "bubble\n";
        if ( cycle >= last_writeback_cycle + 100_lt)
            throw Deadlock( "");

        return;
    }

    auto instr = ( rp_mem_datapath->is_ready( cycle))
        ? rp_mem_datapath->read( cycle)
        : rp_execute_datapath->read( cycle);

    /* no bubble instructions */
    assert( !instr.is_bubble());

    /* perform writeback */
    rf->write_dst( instr);

    /* check for traps */
    instr.check_trap();

    /* bypass data */
    wp_bypass->write( std::make_pair(instr.get_v_dst(), instr.get_v_dst2()), cycle);

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

    if ( func_dump.is_same_checker(instr))
        return;
    
    std::ostringstream oss;
    oss << "Checker output: " << func_dump << std::endl
        << "PerfSim output: " << instr     << std::endl;

    throw CheckerMismatch(oss.str());
}

#include <mips/mips.h>
#include <risc_v/risc_v.h>

template class Writeback<MIPSI>;
template class Writeback<MIPSII>;
template class Writeback<MIPSIII>;
template class Writeback<MIPSIV>;
template class Writeback<MIPS32>;
template class Writeback<MIPS64>;
template class Writeback<RISCV32>;
template class Writeback<RISCV64>;
template class Writeback<RISCV128>;
