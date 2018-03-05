/**
 * data_bypass.h - Implementation of forwarding unit
 * 
 * @author Denis Los
 * Copyright 2018 MIPT-MIPS Project
 */


#ifndef DATA_BYPASS_H
#define DATA_BYPASS_H

#include <unordered_set>
#include <array>

#include <mips/mips_instr.h>

class DataBypass
{
    public:
        class RegisterStage
        {
            public:
                explicit RegisterStage( uint8 value) : value( value) { }

                auto operator==( const RegisterStage& rhs) const { return value == rhs.value; }
                explicit operator uint8() const { return value; }

                void inc() { ++value; }

                static constexpr uint8 get_bypassing_stages_number() { return LAST_BYPASSING_STAGE + 1; }
                static RegisterStage get_last_bypassing_stage() { return RegisterStage( LAST_BYPASSING_STAGE); }
                static RegisterStage in_RF() { return RegisterStage( IN_RF_STAGE_VALUE); }

            private:
                uint8 value = 0;  // distance from first execute stage
                
                // EXECUTE   - 0  | Bypassing stage
                // MEMORY    - 1  | Bypassing stage
                // WRITEBACK - 2  | Bypassing stage
                // IN_RF     - MAX_VAL8

                static constexpr const uint8 IN_RF_STAGE_VALUE = MAX_VAL8;
                static constexpr const uint8 LAST_BYPASSING_STAGE = 2;
        };

        class BypassCommand
        {
            public:
                BypassCommand( RegisterStage bypassing_stage, MIPSRegister register_num)
                    : bypassing_stage( bypassing_stage)
                    , register_num( register_num)
                { }

                auto get_bypassing_stage() const { return bypassing_stage; }
                auto get_register_num() const { return register_num; }    

            private:
                const RegisterStage bypassing_stage;
                const MIPSRegister register_num;
        };

    private:
        struct RegisterInfo
        {
            RegisterStage current_stage = RegisterStage::in_RF();
            RegisterStage ready_stage = RegisterStage::in_RF();
            bool is_bypassible = false;
        };


        class Scoreboard
        {
            public:
                RegisterInfo& get_entry( MIPSRegister num)
                {
                    return array.at( num.to_size_t());
                }

                const RegisterInfo& get_entry( MIPSRegister num) const
                {
                    return array.at( num.to_size_t());
                }

            private:
                std::array<RegisterInfo, MIPSRegister::MAX_REG> array = {};
        };

        // returns current stage of passed register
        // in accordance with the current state of the scoreboard
        auto get_current_stage( MIPSRegister num) const
        {
            return scoreboard.get_entry( num).current_stage;
        }

        // introduces a source register of a passed instruction to scoreboard 
        void trace_new_register( const MIPSInstr& instr, MIPSRegister num);

        // discards the information about passed register
        void untrace_register( MIPSRegister num)
        {
            auto& entry = scoreboard.get_entry( num);

            entry.current_stage = RegisterStage::in_RF();
            entry.is_bypassible = false;
            
            traced_registers.erase( num); 
        }

        Scoreboard scoreboard = {};
        std::unordered_set<MIPSRegister> traced_registers = {};

    public:
        // checks whether the source register of passed instruction is in RF  
        auto is_in_RF( const MIPSInstr& instr, uint8 src_index) const
        {
            const auto reg_num = instr.get_src_num( src_index);
            return scoreboard.get_entry( reg_num).current_stage == RegisterStage::in_RF();
        }

        // checks whether the source register of passed instruction is bypassible
        auto is_bypassible( const MIPSInstr& instr, uint8 src_index) const
        {
            const auto reg_num = instr.get_src_num( src_index);
            return scoreboard.get_entry( reg_num).is_bypassible;
        }

        // checks if the stall needed for passed instruction
        auto is_stall( const MIPSInstr& instr) const
        {
            return ( (!is_in_RF( instr, 0) && !is_bypassible( instr, 0)) ||
                     (!is_in_RF( instr, 1) && !is_bypassible( instr, 1)) );
        }

        // returns bypass command for passed instruction and its source register
        // in accordance with current state of the scoreboard
        auto get_bypass_command( const MIPSInstr& instr, uint8 src_index) const
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

        // transforms bypassing data if needed in accordance with destination register
        static auto get_bypassing_data( const MIPSInstr& instr)
        {
            const auto register_num = instr.get_dst_num();
            
            auto bypassing_data = instr.get_v_dst();

            if ( register_num.is_mips_hi())
                bypassing_data <<= 32;
            
            return bypassing_data;
        }

        // transforms bypassed data if needed in accordance with passed bypass command
        static auto adapt_bypassed_data( const BypassCommand& bypass_command, uint64 bypassed_data)
        {
            const auto register_num = bypass_command.get_register_num();

            auto adapted_data = bypassed_data;

            if ( register_num.is_mips_hi())
                adapted_data >>= 32;
            
            return adapted_data;
        }

        // introduces new instruction to bypassing unit
        void trace_new_instr( const MIPSInstr& instr);

        // updates the scoreboard
        void update();

        // discards the information about passed instruction
        void cancel( const MIPSInstr& instr);
};


inline auto operator""_RSG( unsigned long long int number) // NOLINT https://bugs.llvm.org/show_bug.cgi?id=24840
{
    return DataBypass::RegisterStage( static_cast<uint8>( number));
}


#endif // DATA_BYPASS_H
