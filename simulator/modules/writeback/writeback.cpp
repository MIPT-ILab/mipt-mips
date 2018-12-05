#include "writeback.h"

#include <cassert>
#include <sstream>

template <typename ISA>
Writeback<ISA>::Writeback(bool log) : Log( log)
{
    rp_mem_datapath = make_read_port<Instr>("MEMORY_2_WRITEBACK", PORT_LATENCY);
    rp_execute_datapath = make_read_port<Instr>("EXECUTE_2_WRITEBACK", PORT_LATENCY);
    rp_branch_datapath = make_read_port<Instr>("BRANCH_2_WRITEBACK", PORT_LATENCY);

    wp_bypass = make_write_port<std::pair<RegisterUInt, RegisterUInt>>("WRITEBACK_2_EXECUTE_BYPASS", PORT_BW, SRC_REGISTERS_NUM);
    wp_halt = make_write_port<bool>("WRITEBACK_2_CORE_HALT", PORT_BW, PORT_FANOUT);
}

template <typename ISA>
void Writeback<ISA>::Checker::init( const FuncMemory& outer_mem)
{
    auto memory = FuncMemory::create_hierarchied_memory();
    sim = std::make_shared<FuncSim<ISA>>();
    outer_mem.duplicate_to( memory);
    sim->set_memory( std::move( memory));
    active = true;
}

template <typename ISA>
void Writeback<ISA>::Checker::set_target( const Target& value)
{
    if (active)
        sim->set_target( value);
}

template <typename ISA>
std::vector<Instr> Writeback<ISA>::read_instructions( Cycle cycle)
{
    std::vector<Instr> result;
    for ( auto& port : { rp_branch_datapath, rp_mem_datapath, rp_execute_datapath})
        if ( port->is_ready( cycle))
            result.emplace_back( port->read());

    return {};
}

template <typename ISA>
void Writeback<ISA>::clock( Cycle cycle)
{
    sout << "wb      cycle " << std::dec << cycle << ": ";

    auto instrs = read_instructions( cycle)

    if ( instrs.empty())
    {
        sout << "bubble\n";
        if ( cycle >= last_writeback_cycle + 100_lt)
            throw Deadlock( "");

        return;
    }

    writeback( instr.front(), cycle);
}

template <typename ISA>
void Writeback<ISA>::writeback( const Instr& instr, Cycle cycle)
{
    rf->write_dst( instr);
    instr.check_trap();
    wp_bypass->write( std::make_pair(instr.get_v_dst(), instr.get_v_dst2()), cycle);

    sout << instr << std::endl;

    checker.check( instr);
    ++executed_instrs;
    last_writeback_cycle = cycle;
    if ( executed_instrs >= instrs_to_run || instr.is_halt())
        wp_halt->write( true, cycle);
    
    sout << "Executed instructions: " << executed_instrs
         << std::endl << std::endl;
}

template <typename ISA>
void Writeback<ISA>::Checker::check( const FuncInstr& instr)
{
    if (!active)
        return;

    const auto func_dump = sim->step();

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
