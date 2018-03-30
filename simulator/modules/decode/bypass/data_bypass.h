/**
 * data_bypass.h - Implementation of bypassing unit
 * 
 * @author Denis Los
 * Copyright 2018 MIPT-MIPS Project
 */

#ifndef DATA_BYPASS_H
#define DATA_BYPASS_H

#include <array>

#include <modules/core/perf_instr.h>

#include "data_bypass_interface.h"

template <typename ISA>
class DataBypass
{
    using FuncInstr = typename ISA::FuncInstr;
    using Register  = typename ISA::Register;
    using Instr     = PerfInstr<FuncInstr>;
    using RegDstUInt = typename ISA::RegDstUInt;

    public:
        // checks whether the source register of passed instruction is in RF  
        auto is_in_RF( const Instr& instr, uint8 src_index) const
        {
            const auto reg_num = instr.get_src_num( src_index);
            return get_entry( reg_num).current_stage == RegisterStage::in_RF();
        }

        // checks whether the source register of passed instruction is bypassible
        auto is_bypassible( const Instr& instr, uint8 src_index) const
        {
            const auto reg_num = instr.get_src_num( src_index);
            return get_entry( reg_num).is_bypassible;
        }

        // checks if the stall needed for passed instruction
        auto is_stall( const Instr& instr) const
        {
            return ( (!is_in_RF( instr, 0) && !is_bypassible( instr, 0)) ||
                     (!is_in_RF( instr, 1) && !is_bypassible( instr, 1)) );
        }

        // returns bypass command for passed instruction and its source register
        // in accordance with current state of the scoreboard
        auto get_bypass_command( const Instr& instr, uint8 src_index) const
        {
            const auto reg_num = instr.get_src_num( src_index);
            return BypassCommand<Register>( get_current_stage( reg_num), reg_num);
        }

        // introduces new instruction to bypassing unit
        void trace_new_instr( const Instr& instr);

        // updates the scoreboard
        void update();

        // removes the information about passed instruction from the scoreboard
        void untrace_instr( const Instr& instr);
    
    private:
        struct RegisterInfo
        {
            RegisterStage current_stage = RegisterStage::in_RF();
            RegisterStage ready_stage = RegisterStage::in_RF();
            bool is_bypassible = false;
            bool is_traced = false;
        };

        std::array<RegisterInfo, Register::MAX_REG> scoreboard = {};

        RegisterInfo& get_entry( Register num)
        {
            return scoreboard.at( num.to_size_t());
        }

        const RegisterInfo& get_entry( Register num) const
        {
            return scoreboard.at( num.to_size_t());
        }

        // returns current stage of passed register
        // in accordance with the current state of the scoreboard
        RegisterStage get_current_stage( Register num) const
        {
            return get_entry( num).current_stage;
        }

        // introduces a source register of a passed instruction to scoreboard 
        void trace_new_register( const Instr& instr, Register num);

        // discards the information about passed register
        void untrace_register( Register num)
        {
            auto& entry = get_entry( num);

            entry.current_stage = RegisterStage::in_RF();
            entry.is_bypassible = false;
            entry.is_traced = false; 
        }
};

template <typename ISA>
void DataBypass<ISA>::trace_new_register( const Instr& instr, Register num)
{
    auto& entry = get_entry( num);

    entry.current_stage = 0_RSG; // first execute stage

    if ( !instr.is_bypassible())
        entry.ready_stage = RegisterStage::in_RF();
    else
        entry.ready_stage = instr.is_load() ? 1_RSG  // MEMORY
                                            : 0_RSG; // EXECUTE

    entry.is_bypassible = ( entry.current_stage == entry.ready_stage);
    entry.is_traced = true;
}


template <typename ISA>
void DataBypass<ISA>::trace_new_instr( const Instr& instr)
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


template <typename ISA>
void DataBypass<ISA>::update()
{
    for ( auto& entry:scoreboard)
    {
        if ( entry.is_traced)
        {
            if ( entry.current_stage.is_writeback())
            {
                entry.current_stage = RegisterStage::in_RF();
                entry.is_bypassible = false;
                entry.is_traced = false;
            }
            else
            {
                entry.current_stage.inc();

                if ( entry.current_stage == entry.ready_stage)
                    entry.is_bypassible = true;   
            }
        }
    }
}


template <typename ISA>
void DataBypass<ISA>::untrace_instr( const Instr& instr)
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

#endif // DATA_BYPASS_H
