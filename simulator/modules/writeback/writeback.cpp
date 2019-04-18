#include "writeback.h"

#include <cassert>
#include <sstream>

template <typename ISA>
Writeback<ISA>::Writeback( Endian endian, bool log, uint32 writeback_bandwidth) : Log( log), endian( endian)
{
    set_bandwidth( writeback_bandwidth);
    rp_mem_datapath = make_read_port<Instr>("MEMORY_2_WRITEBACK", PORT_LATENCY);
    rp_execute_datapath = make_read_port<Instr>("EXECUTE_2_WRITEBACK", PORT_LATENCY);
    rp_branch_datapath = make_read_port<Instr>("BRANCH_2_WRITEBACK", PORT_LATENCY);

    wp_bypass = make_write_port<std::pair<RegisterUInt, RegisterUInt>>("WRITEBACK_2_EXECUTE_BYPASS", PORT_BW, SRC_REGISTERS_NUM);
    wp_halt = make_write_port<bool>("WRITEBACK_2_CORE_HALT", PORT_BW, PORT_FANOUT);
}

template <typename ISA>
void Writeback<ISA>::Checker::init( Endian endian, const FuncMemory& outer_mem)
{
    auto memory = FuncMemory::create_hierarchied_memory();
    sim = std::make_shared<FuncSim<ISA>>( endian);
    outer_mem.duplicate_to( memory);
    sim->set_memory( std::move( memory));
    active = true;
}

template <typename ISA>
void Writeback<ISA>::Checker::set_target( const Target& value)
{
    if ( active)
        sim->set_target( value);
}

template<typename ISA>
void Writeback<ISA>::set_target( const Target& value)
{
    checker.set_target( value);
    next_PC = value.address;
}
    
template <typename ISA>
auto Writeback<ISA>::read_instructions( Cycle cycle)
{
    auto ports = { rp_branch_datapath.get(), rp_mem_datapath.get(), rp_execute_datapath.get() };
    std::vector<Instr> result;
    result.reserve( bandwidth); // depends on WB througput

    for ( auto& port : ports)
        if ( port->is_ready( cycle))
            result.emplace_back( port->read( cycle));

    return result;
}

template <typename ISA>
void Writeback<ISA>::clock( Cycle cycle)
{
    auto instrs = read_instructions( cycle);

    if ( instrs.empty())
    {
        sout << "wb      cycle " << std::dec << cycle << ": ";
        writeback_bubble( cycle);
    }
    else
        for( uint32 i = 0; i < bandwidth && i < instrs.size(); i++)
        {
            sout << "wb      cycle " << std::dec << cycle << ": ";
            writeback_instruction( instrs.at(i), cycle);
        }
}

template <typename ISA>
void Writeback<ISA>::writeback_bubble( Cycle cycle)
{
    sout << "bubble\n";
    if ( cycle >= last_writeback_cycle + 100_lt)
        throw Deadlock( "");
}

template <typename ISA>
void Writeback<ISA>::writeback_instruction( const Writeback<ISA>::Instr& instr, Cycle cycle)
{
    if ( instr.trap_type() == Trap::UNKNOWN_INSTRUCTION)
        throw UnknownInstruction( instr.string_dump() + ' ' + instr.bytes_dump());

    rf->write_dst( instr);
    wp_bypass->write( std::make_pair(instr.get_v_dst(), instr.get_v_dst2()), cycle);

    sout << instr << std::endl;

    checker.check( instr);
    ++executed_instrs;
    last_writeback_cycle = cycle;
    next_PC = instr.get_actual_target().address;
    if ( executed_instrs >= instrs_to_run || instr.is_halt())
        wp_halt->write( true, cycle);
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

template <typename ISA>
void Writeback<ISA>::set_bandwidth( uint32 writeback_bandwidth)
{
    bandwidth = writeback_bandwidth;
}

#include <mips/mips.h>
#include <risc_v/risc_v.h>

template class Writeback<MIPSI>;
template class Writeback<MIPSII>;
template class Writeback<MIPSIII>;
template class Writeback<MIPSIV>;
template class Writeback<MIPS32>;
template class Writeback<MIPS64>;
template class Writeback<MARS>;
template class Writeback<MARS64>;
template class Writeback<RISCV32>;
template class Writeback<RISCV64>;
template class Writeback<RISCV128>;
