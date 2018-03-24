/**
 * execute.h - Simulation of execute units
 * Copyright 2015-2018 MIPT-MIPS
 */


#ifndef EXECUTE_H
#define EXECUTE_H


#include <infra/ports/ports.h>
#include <core/perf_instr.h>
#include <bypass/data_bypass.h>


template <typename ISA>
class Execute : public Log
{
    using FuncInstr = typename ISA::FuncInstr;
    using Instr = PerfInstr<FuncInstr>;
    using BypassingUnit = DataBypass<ISA>;
    using RegDstUInt = typename ISA::RegDstUInt;

    private:
        std::unique_ptr<WritePort<Instr>> wp_datapath = nullptr;
        std::unique_ptr<ReadPort<Instr>> rp_datapath = nullptr;

        std::unique_ptr<ReadPort<bool>> rp_flush = nullptr;

        static constexpr const uint8 SRC_REGISTERS_NUM = 2;

        std::array<std::unique_ptr<ReadPort<typename BypassingUnit::BypassCommand>>, SRC_REGISTERS_NUM>
            rps_command;
        
        std::array<std::array<std::unique_ptr<ReadPort<RegDstUInt>>, RegisterStage::BYPASSING_STAGES_NUMBER>, SRC_REGISTERS_NUM>
            rps_sources_bypass;
        
        std::unique_ptr<WritePort<RegDstUInt>> wp_bypass = nullptr;

        std::unique_ptr<WritePort<Instr>> wp_bypassing_unit_flush_notify = nullptr;
    
    public:
        explicit Execute( bool log);
        void clock( Cycle cycle);
};



#endif // EXECUTE_H
