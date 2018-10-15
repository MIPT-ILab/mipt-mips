/**
 * data_bypass.h - Implementation of bypassing unit
 * 
 * @author Denis Los
 * Copyright 2018 MIPT-MIPS Project
 */

#ifndef DATA_BYPASS_H
#define DATA_BYPASS_H

#include "data_bypass_interface.h"

#include <modules/core/perf_instr.h>

#include <array>

template <typename ISA>
class DataBypass
{
    using FuncInstr = typename ISA::FuncInstr;
    using Register  = typename ISA::Register;
    using Instr     = PerfInstr<FuncInstr>;

    public:
        explicit DataBypass( uint64 long_alu_latency)
            : last_execution_stage_value( static_cast<uint8>(long_alu_latency - 1))
        { }

        // checks whether a source register of an instruction is in the RF  
        auto is_in_RF( const Instr& instr, uint8 src_index) const
        {
            const auto reg_num = instr.get_src_num( src_index);
            return get_entry( reg_num).current_stage.is_in_RF();
        }

        // checks whether a source register of an instruction is bypassible
        auto is_bypassible( const Instr& instr, uint8 src_index) const
        {
            const auto reg_num = instr.get_src_num( src_index);
            return get_entry( reg_num).is_bypassible;
        }

        // checks whether the stall is needed for an instruction
        auto is_stall( const Instr& instr) const
        {
            const auto instruction_latency = get_instruction_latency( instr);
    
            return (( !is_in_RF( instr, 0) && !is_bypassible( instr, 0)) ||
                    ( !is_in_RF( instr, 1) && !is_bypassible( instr, 1)) ||
                    ( instruction_latency < writeback_stage_info.operation_latency));
        }

        // returns a bypass command for a source register of an instruction
        // in accordance with a current state of the scoreboard
        auto get_bypass_command( const Instr& instr, uint8 src_index) const
        {
            const auto reg_num = instr.get_src_num( src_index);
            return BypassCommand<Register>( get_entry( reg_num).current_stage, last_execution_stage_value);
        }

        // garners the information about a new instruction
        void trace_new_instr( const Instr& instr);

        // updates the scoreboard
        void update();

        // handles a flush of the pipeline
        void handle_flush();
    
    private:
        const uint8 last_execution_stage_value;

        struct RegisterInfo
        {
            RegisterStage current_stage;
            RegisterStage ready_stage;
            RegisterStage next_stage_after_first_execution_stage;
            bool is_bypassible = false;
            bool is_traced = false;

            void reset() { *this = RegisterInfo(); }

            void set_next_stage_after_first_execution_stage( const Instr& instr)
            {
                if ( instr.is_long_arithmetic())
                {
                    next_stage_after_first_execution_stage.set_to_first_execution_stage();
                    next_stage_after_first_execution_stage.inc();
                }
                else if ( instr.is_mem_stage_required())
                {
                    next_stage_after_first_execution_stage.set_to_mem_stage();
                }
                else
                {
                    next_stage_after_first_execution_stage.set_to_writeback();
                }
            }
        };

        struct FuncUnitInfo
        {
            Latency operation_latency = 0_lt;

            void update()
            {
                if ( operation_latency != 0_lt)
                    operation_latency = operation_latency - 1_lt;
            }
        };

        std::array<RegisterInfo, Register::MAX_REG> scoreboard = {};
        FuncUnitInfo writeback_stage_info = {};

        RegisterInfo& get_entry( Register num)
        {
            return scoreboard.at( num.to_size_t());
        }

        const RegisterInfo& get_entry( Register num) const
        {
            return scoreboard.at( num.to_size_t());
        }

        // returns a latency of an instruction
        // in accordance with a type of the instruction
        Latency get_instruction_latency( const Instr& instr) const
        {
            if ( instr.is_mem_stage_required())
                return 2_lt;
            
            if ( instr.is_long_arithmetic())
                return Latency( last_execution_stage_value + 1);
            
            return 1_lt;
        }

        // garners the information about a new register used for the 1st destination
        void trace_new_dst_register( const Instr& instr, Register num);

        // garners the information about a new register used for the 2nd destination
        void trace_new_dst2_register( const Instr& instr, Register num);
};


template <typename ISA>
void DataBypass<ISA>::trace_new_dst_register( const Instr& instr, Register num)
{
    auto& entry = get_entry( num);

    entry.current_stage.set_to_first_execution_stage();
    entry.set_next_stage_after_first_execution_stage( instr);


    if ( instr.is_long_arithmetic())
    {
        entry.ready_stage.set_to_last_execution_stage( last_execution_stage_value);
    }
    else if ( instr.is_load())
    {
        entry.ready_stage.set_to_mem_stage();
    }
    else
    {
        entry.ready_stage.set_to_first_execution_stage();
    }

    if ( !instr.is_bypassible())
        entry.ready_stage.set_to_in_RF();


    entry.is_bypassible = ( entry.current_stage == entry.ready_stage);
    entry.is_traced = true;
}


template <typename ISA>
void DataBypass<ISA>::trace_new_dst2_register( const Instr& instr, Register num)
{
    auto& entry = get_entry( num);

    entry.current_stage.set_to_first_execution_stage();
    entry.set_next_stage_after_first_execution_stage( instr);

    // values of registers used for the 2nd destination cannot be bypassed
    entry.ready_stage.set_to_in_RF();

    entry.is_traced = true;
}


template <typename ISA>
void DataBypass<ISA>::trace_new_instr( const Instr& instr)
{    
    const auto& dst  = instr.get_dst_num();
    const auto& dst2 = instr.get_dst2_num();

    writeback_stage_info.operation_latency = get_instruction_latency( instr);
    
    if ( !dst.is_zero())
        trace_new_dst_register( instr, dst);
    
    if ( !dst2.is_zero())
        trace_new_dst2_register( instr, dst2);
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
                entry.reset();
            }
            else
            {
                if ( entry.current_stage.is_first_execution_stage())
                {
                    entry.current_stage = entry.next_stage_after_first_execution_stage;
                }
                else if ( entry.current_stage.is_last_execution_stage( last_execution_stage_value))
                {
                    entry.current_stage.set_to_writeback();
                }
                else
                    entry.current_stage.inc();
                

                if ( entry.current_stage == entry.ready_stage)
                    entry.is_bypassible = true;
            }
        }
    }

    writeback_stage_info.update();
}


template <typename ISA>
void DataBypass<ISA>::handle_flush()
{
    for ( auto& entry:scoreboard)
    {
        if ( entry.is_traced)
            entry.reset();
    }

    writeback_stage_info.operation_latency = 0_lt;
}

#endif // DATA_BYPASS_H
