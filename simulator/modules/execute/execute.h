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
    using RegDstUInt = typename ISA::RegDstUInt;

    private:   
        static constexpr const uint8 SRC_REGISTERS_NUM = 2;
        static constexpr const Latency COMPLEX_ALU_LATENCY = 2_Lt;

        /* Inputs */
        std::unique_ptr<ReadPort<Instr>> rp_datapath = nullptr;
        std::unique_ptr<ReadPort<Instr>> rp_long_latency_execution_unit = nullptr;
        std::unique_ptr<ReadPort<bool>> rp_flush = nullptr;
        std::array<std::unique_ptr<ReadPort<BypassCommand<Register>>>, SRC_REGISTERS_NUM> rps_command;
        std::array<std::array<std::unique_ptr<ReadPort<RegDstUInt>>, RegisterStage::BYPASSING_STAGES_NUMBER>, SRC_REGISTERS_NUM>
            rps_sources_bypass;

        /* Outputs */
        std::unique_ptr<WritePort<Instr>> wp_mem_datapath = nullptr;
        std::unique_ptr<WritePort<Instr>> wp_writeback_datapath = nullptr;
        std::unique_ptr<WritePort<Instr>> wp_long_latency_execution_unit = nullptr;
        std::unique_ptr<WritePort<RegDstUInt>> wp_bypass = nullptr;
        std::unique_ptr<WritePort<RegDstUInt>> wp_complex_arithmetic_bypass = nullptr;

        class SavedFlushHandler
        {
            public:
                void set() 
                { 
                    cycles_till_expiration = cycles_till_expiration + COMPLEX_ALU_LATENCY; 
                }

                auto has_expired() const { return cycles_till_expiration == 0_Cl; }
                void update() { cycles_till_expiration.dec(); }

            private:
                Cycle cycles_till_expiration = 0_Cl;
        };

        SavedFlushHandler saved_flush;

    public:
        explicit Execute( bool log);
        void clock( Cycle cycle);
};

#endif // EXECUTE_H
