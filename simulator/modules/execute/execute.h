/**
 * execute.h - Simulation of execute units
 * Copyright 2015-2018 MIPT-MIPS
 */

#ifndef EXECUTE_H
#define EXECUTE_H

#include <func_sim/operation.h>
#include <infra/config/config.h>
#include <modules/core/perf_instr.h>
#include <modules/decode/bypass/data_bypass_interface.h>
#include <modules/ports_instance.h>

namespace config {
    extern const PredicatedValue<uint64> long_alu_latency;
} // namespace config

template <typename FuncInstr>
class Execute : public Module
{
    using Register = typename FuncInstr::Register;
    using Instr = PerfInstr<FuncInstr>;
    using RegisterUInt = typename FuncInstr::RegisterUInt;
    using InstructionOutput = std::array< RegisterUInt, MAX_DST_NUM>;

    private:
        static constexpr const uint8 SRC_REGISTERS_NUM = 2;
        const Latency last_execution_stage_latency;

        /* Inputs */
        ReadPort<Instr>* rp_datapath = nullptr;
        ReadPort<Instr>* rp_long_latency_execution_unit = nullptr;
        ReadPort<bool>* rp_flush = nullptr;
        ReadPort<bool>* rp_trap = nullptr;

        struct BypassPorts {
            ReadPort<BypassCommand<Register>>* command_port;
            std::array<ReadPort<InstructionOutput>*, RegisterStage::BYPASSING_STAGES_NUMBER> data_ports;
        };
        std::array<BypassPorts, SRC_REGISTERS_NUM> rps_bypass;

        /* Outputs */
        WritePort<Instr>* wp_mem_datapath = nullptr;
        WritePort<Instr>* wp_branch_datapath = nullptr;
        WritePort<Instr>* wp_writeback_datapath = nullptr;
        WritePort<Instr>* wp_long_latency_execution_unit = nullptr;
        WritePort<InstructionOutput>* wp_bypass = nullptr;
        WritePort<InstructionOutput>* wp_long_arithmetic_bypass = nullptr;

        Latency flush_expiration_latency = 0_lt;

        void save_flush() { flush_expiration_latency = last_execution_stage_latency; }
        void clock_saved_flush()
        {
            if ( flush_expiration_latency != 0_lt)
                flush_expiration_latency = flush_expiration_latency - 1_lt;
        }
        auto has_flush_expired() const { return flush_expiration_latency == 0_lt; }

    public:
        explicit Execute( Module* parent);
        void clock( Cycle cycle);
};

#endif // EXECUTE_H
