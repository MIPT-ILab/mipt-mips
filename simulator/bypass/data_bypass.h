/**
 * data_bypass.h - Implementation of bypassing unit
 * 
 * @author Denis Los
 * Copyright 2018 MIPT-MIPS Project
 */


#ifndef DATA_BYPASS_H
#define DATA_BYPASS_H


#include <array>

#include <mips/mips.h>



class RegisterStage
{
    public:
        explicit RegisterStage( uint8 value) : value( value) { }

        auto operator==( const RegisterStage& rhs) const { return value == rhs.value; }
        explicit operator uint8() const { return value; }

        void inc() { ++value; }

        static constexpr const uint8 BYPASSING_STAGES_NUMBER = 3;
        static const RegisterStage WRITEBACK;
        static const RegisterStage IN_RF;

    private:
        uint8 value = 0;  // distance from first execute stage
                
        // EXECUTE   - 0  | Bypassing stage
        // MEMORY    - 1  | Bypassing stage
        // WRITEBACK - 2  | Bypassing stage
        // IN_RF     - MAX_VAL8
};


// NOLINTNEXTLINE(google-runtime-int) https://bugs.llvm.org/show_bug.cgi?id=24840
inline auto operator""_RSG( unsigned long long int number)
{
    return RegisterStage( static_cast<uint8>( number));
}



template <typename ISA>
class DataBypass
{
    using FuncInstr = typename ISA::FuncInstr;
    using Register  = typename ISA::Register;

    public:
        class BypassCommand
        {
            public:
                BypassCommand( RegisterStage bypassing_stage, Register register_num)
                    : bypassing_stage( bypassing_stage)
                    , register_num( register_num)
                { }

                auto get_bypassing_stage() const { return bypassing_stage; }
                auto get_register_num() const { return register_num; }

            private:
                const RegisterStage bypassing_stage;
                const Register register_num;
        };

        // checks whether the source register of passed instruction is in RF  
        auto is_in_RF( const FuncInstr& instr, uint8 src_index) const
        {
            const auto reg_num = instr.get_src_num( src_index);
            return scoreboard.get_entry( reg_num).current_stage == RegisterStage::IN_RF;
        }

        // checks whether the source register of passed instruction is bypassible
        auto is_bypassible( const FuncInstr& instr, uint8 src_index) const
        {
            const auto reg_num = instr.get_src_num( src_index);
            return scoreboard.get_entry( reg_num).is_bypassible;
        }

        // checks if the stall needed for passed instruction
        auto is_stall( const FuncInstr& instr) const
        {
            return ( (!is_in_RF( instr, 0) && !is_bypassible( instr, 0)) ||
                     (!is_in_RF( instr, 1) && !is_bypassible( instr, 1)) );
        }

        // returns bypass command for passed instruction and its source register
        // in accordance with current state of the scoreboard
        auto get_bypass_command( const FuncInstr& instr, uint8 src_index) const
        {
            const auto reg_num = instr.get_src_num( src_index);
            return BypassCommand( get_current_stage( reg_num), reg_num);
        }

        // returns an index of the port where bypassed data should be get from
        // in accordance with passed bypass command
        auto get_bypass_direction( const BypassCommand& bypass_command) const
        {
            const auto bypassing_stage = bypass_command.get_bypassing_stage();
            return static_cast<uint8>( bypassing_stage);
        }

        // transforms bypassed data if needed in accordance with passed bypass command
        static uint64 adapt_bypassed_data( const BypassCommand& bypass_command, uint64 bypassed_data);

        // introduces new instruction to bypassing unit
        void trace_new_instr( const FuncInstr& instr);

        // updates the scoreboard
        void update();

        // removes the information about passed instruction from the scoreboard
        void untrace_instr( const FuncInstr& instr);
    
    private:
        struct RegisterInfo
        {
            RegisterStage current_stage = RegisterStage::IN_RF;
            RegisterStage ready_stage = RegisterStage::IN_RF;
            bool is_bypassible = false;
            bool is_traced = false;
        };


        class Scoreboard
        {
            public:
                RegisterInfo& get_entry( Register num)
                {
                    return array.at( num.to_size_t());
                }

                const RegisterInfo& get_entry( Register num) const
                {
                    return array.at( num.to_size_t());
                }

                auto begin() { return array.begin(); }
                auto end() { return array.end(); }

            private:
                std::array<RegisterInfo, Register::MAX_REG> array = {};
        };

        // returns current stage of passed register
        // in accordance with the current state of the scoreboard
        RegisterStage get_current_stage( Register num) const
        {
            return scoreboard.get_entry( num).current_stage;
        }

        // introduces a source register of a passed instruction to scoreboard 
        void trace_new_register( const FuncInstr& instr, Register num);

        // discards the information about passed register
        void untrace_register( Register num)
        {
            auto& entry = scoreboard.get_entry( num);

            entry.current_stage = RegisterStage::IN_RF;
            entry.is_bypassible = false;
            entry.is_traced = false; 
        }

        Scoreboard scoreboard = {};
};



template <typename ISA>
void DataBypass<ISA>::trace_new_register( const FuncInstr& instr, Register num)
{
    auto& entry = scoreboard.get_entry( num);

    entry.current_stage = 0_RSG; // first execute stage

    if ( !instr.is_bypassible())
        entry.ready_stage = RegisterStage::IN_RF;
    else
        entry.ready_stage = instr.is_load() ? 1_RSG  // MEMORY
                                            : 0_RSG; // EXECUTE

    entry.is_bypassible = ( entry.current_stage == entry.ready_stage);
    entry.is_traced = true;
}


template <typename ISA>
void DataBypass<ISA>::trace_new_instr( const FuncInstr& instr)
{    
    const auto dst_reg_num = instr.get_dst_num();

    if ( dst_reg_num.is_zero())
        return;

    trace_new_register( instr, dst_reg_num);
}


template <typename ISA>
void DataBypass<ISA>::update()
{
    for ( auto& entry:scoreboard)
    {
        if ( entry.is_traced)
        {
            if ( entry.current_stage == RegisterStage::WRITEBACK)
            {
                entry.current_stage = RegisterStage::IN_RF;
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
void DataBypass<ISA>::untrace_instr( const FuncInstr& instr)
{
    auto dst_reg_num = instr.get_dst_num();

    if ( dst_reg_num.is_zero())
        return;

    untrace_register( dst_reg_num);
}



// *****************************************************
// ***              MIPS SPECILIZATION               ***
// *****************************************************
template <>
uint64 DataBypass<MIPS>::adapt_bypassed_data( const BypassCommand& bypass_command, uint64 bypassed_data);

template <>
void DataBypass<MIPS>::trace_new_instr( const FuncInstr& instr);

template <>
void DataBypass<MIPS>::untrace_instr( const FuncInstr& instr);


#endif // DATA_BYPASS_H
