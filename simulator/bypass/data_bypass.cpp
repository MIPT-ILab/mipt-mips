/**
 * data_bypass.cpp - Implementation of forwarding unit
 * 
 * @author Denis Los
 * Copyright 2018 MIPT-MIPS Project
 */


#include "data_bypass.h"

#include <mips/mips.h>




// *****************************************************
// ***               MIPS SPECILIZATION              ***
// *****************************************************
template <>
uint64 DataBypass<MIPS>::adapt_bypassed_data( const BypassCommand& bypass_command, uint64 bypassed_data)
{
    const auto register_num = bypass_command.get_register_num();

    auto adapted_data = bypassed_data;

    if ( register_num.is_mips_hi())
        adapted_data >>= 32;
            
    return adapted_data;
}


template <> 
void DataBypass<MIPS>::trace_new_instr( const FuncInstr& instr)
{
    const auto dst_reg_num = instr.get_dst_num();

    if ( dst_reg_num.is_zero())
        return;

    if ( dst_reg_num.is_mips_hi_lo())
    {
        trace_new_register( instr, Register::mips_lo );
        trace_new_register( instr, Register::mips_hi );
        return;
    }

    trace_new_register( instr, dst_reg_num);
}


template <>
void DataBypass<MIPS>::untrace_instr( const FuncInstr& instr)
{
    auto dst_reg_num = instr.get_dst_num();

    if ( dst_reg_num.is_zero())
        return;

    if ( dst_reg_num.is_mips_hi_lo())
    {
        untrace_register( Register::mips_hi );
        untrace_register( Register::mips_lo );
        return;
    }

    untrace_register( dst_reg_num);
}
