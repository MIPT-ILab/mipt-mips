/*
 * mips_instr.cpp - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015-2018 MIPT-MIPS
 */

#include "mips_instr.h"

#include <iomanip>
#include <iostream>
#include <sstream>

template<typename R>
void BaseMIPSInstr<R>::execute()
{
    execute_function(this);
    complete = true;
}

template<typename R>
void BaseMIPSInstr<R>::set_v_dst( R value)
{
    memory_complete = true;
    if ( operation == OUT_I_LOAD || is_partial_load())
    {
        switch ( get_mem_size())
        {
            case 1: v_dst = narrow_cast<int8>( value); break;
            case 2: v_dst = narrow_cast<int16>( value); break;
            case 4: v_dst = narrow_cast<int32>( value); break;
            case 8: v_dst = value; break;
            default: assert( false);
        }
    }
    else if ( operation == OUT_I_LOADU)
    {
        v_dst = value;
    }
    else
    {
        assert( false);
    }
}
template<typename R>
std::string BaseMIPSInstr<R>::string_dump() const
{
    std::ostringstream oss;
    dump( oss);
    return oss.str();
}

template<typename R>
std::ostream& BaseMIPSInstr<R>::dump( std::ostream& out) const
{
    out << "{" << sequence_id << "}\t";
    out << disasm;
    if ( !dst.is_zero() && (is_load() ? memory_complete : complete) && get_mask() != 0)
    {
        out << "\t [ $" << dst << " = 0x" << std::hex << (v_dst & mask);
        if ( !dst2.is_zero())
            out << ", $" << dst2 << " = 0x" << v_dst2;
        out << " ]";
    }
    if ( trap != Trap::NO_TRAP)
        out << "\t trap";

    out << std::dec;
    return out;
}

template class BaseMIPSInstr<uint32>;
template class BaseMIPSInstr<uint64>;
