/**
 * execute.h - Simulation of execute units
 * Copyright 2015-2018 MIPT-MIPS
 */

#ifndef EXECUTE_H
#define EXECUTE_H

#include <infra/ports/ports.h>
#include <modules/core/perf_instr.h>
#include <modules/decode/bypass/data_bypass_interface.h>

template <typename ISA>
class Execute : public Log
{
    using FuncInstr = typename ISA::FuncInstr;
    using Register = typename ISA::Register;
    using Instr = PerfInstr<FuncInstr>;
    using RegisterUInt = typename ISA::RegisterUInt;
    using InstructionOutput = std::pair< RegisterUInt, RegisterUInt>;

    private:   
        static constexpr const uint8 SRC_REGISTERS_NUM = 2;
        const Latency last_execution_stage_latency;

        /* Inputs */
        std::unique_ptr<ReadPort<Instr>> rp_datapath = nullptr;
        std::unique_ptr<ReadPort<Instr>> rp_long_latency_execution_unit = nullptr;
        std::unique_ptr<ReadPort<bool>> rp_flush = nullptr;

        struct BypassPorts {
            std::unique_ptr<ReadPort<BypassCommand<Register>>> command_port;
            std::array<std::unique_ptr<ReadPort<InstructionOutput>>, RegisterStage::BYPASSING_STAGES_NUMBER> data_ports;
        };
        std::array<BypassPorts, SRC_REGISTERS_NUM> rps_bypass;

        /* Outputs */
        std::unique_ptr<WritePort<Instr>> wp_mem_datapath = nullptr;
        std::unique_ptr<WritePort<Instr>> wp_writeback_datapath = nullptr;
        std::unique_ptr<WritePort<Instr>> wp_long_latency_execution_unit = nullptr;
        std::unique_ptr<WritePort<InstructionOutput>> wp_bypass = nullptr;
        std::unique_ptr<WritePort<InstructionOutput>> wp_long_arithmetic_bypass = nullptr;

        Latency flush_expiration_latency = 0_lt;

        void save_flush() { flush_expiration_latency = last_execution_stage_latency; }
        void clock_saved_flush()
        {
            if ( flush_expiration_latency != 0_lt)
                flush_expiration_latency = flush_expiration_latency - 1_lt;
        }
        auto has_flush_expired() const { return flush_expiration_latency == 0_lt; }


    public:
        explicit Execute( bool log);
        void clock( Cycle cycle);
};

#endif // EXECUTE_H
