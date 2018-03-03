/**
 * data_bypass.cpp - Implementation of forwarding unit
 * 
 * @author Denis Los
 * Copyright 2018 MIPT-MIPS Project
 */


#include "data_bypass.h"

#include <mips/mips_instr.h>
#include <infra/ports/timing.h>

void DataBypass::trace_new_register( const MIPSInstr& instr, MIPSRegister num)
{
    auto& entry = scoreboard.get_entry( num);

    entry.current_stage = 0_RSG; // first execute stage

    if ( instr.is_conditional_move())
        entry.ready_stage = RegisterStage::in_RF();
    else
        entry.ready_stage = instr.is_load() ? 1_RSG  // MEMORY 
                                            : 0_RSG; // EXECUTE

    if ( instr.get_dst_num().is_mips_hi_lo())
    {
        is_HI_master_DIVMULT = true;
    }
    else if ( num.is_mips_lo())
    {
        is_HI_master_DIVMULT = false;
    }

    entry.is_bypassible = ( entry.current_stage == entry.ready_stage);
    traced_registers.emplace( num);
}

void DataBypass::trace_new_instr( const MIPSInstr& instr)
{    
    const auto dst_reg_num = instr.get_dst_num();

    if ( dst_reg_num.is_zero())
        return;

    if ( dst_reg_num.is_mips_hi_lo())
    {
        trace_new_register( instr, MIPSRegister::mips_lo );
        trace_new_register( instr, MIPSRegister::mips_hi );
        return;
    }

    trace_new_register( instr, dst_reg_num);
}


void DataBypass::update()
{
    for ( auto it = traced_registers.begin(); it != traced_registers.end();)
    {
        auto& entry = scoreboard.get_entry( *it);

        if ( entry.current_stage == RegisterStage::get_last_bypassing_stage())
        {
            entry.current_stage = RegisterStage::in_RF();
            entry.is_bypassible = false;
            it = traced_registers.erase( it);
        }
        else
        {
            entry.current_stage.inc();

            if ( entry.current_stage == entry.ready_stage)
                entry.is_bypassible = true;

            ++it;
        }
    }
}


void DataBypass::cancel( const MIPSInstr& instr)
{
    auto dst_reg_num = instr.get_dst_num();

    if ( dst_reg_num.is_zero())
        return;

    if ( dst_reg_num.is_mips_hi_lo())
    {
        untrace_register( MIPSRegister::mips_hi );
        untrace_register( MIPSRegister::mips_lo );
        return;
    }

    untrace_register( dst_reg_num);
}
