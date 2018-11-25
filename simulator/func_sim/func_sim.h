/*
 * func_sim.cpp - extremely simple simulator
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS
 */

#ifndef FUNC_SIM_H
#define FUNC_SIM_H

#include "instr_memory.h"
#include "rf/rf.h"

#include <infra/exception.h>
#include <memory/memory.h>
#include <simulator.h>

#include <memory>
#include <string>

struct BearingLost final : Exception
{
    BearingLost() : Exception("Bearing lost", "10 nops in a row") { }
};

template <typename ISA>
class FuncSim : public Simulator
{
    using FuncInstr = typename ISA::FuncInstr;
    using Register = typename ISA::Register;
    using RegisterUInt = typename ISA::RegisterUInt;

    private:
        RF<ISA> rf;
        Addr PC = NO_VAL32;
        uint64 sequence_id = 0;
        std::shared_ptr<FuncMemory> mem;
        InstrMemoryCached<FuncInstr> imem;
        std::shared_ptr<Kernel> kernel;

        uint64 nops_in_a_row = 0;
        void update_and_check_nop_counter( const FuncInstr& instr);
        Trap handle_syscall();

    public:
        explicit FuncSim( bool log = false);

        void set_memory( std::shared_ptr<FuncMemory> memory) final;
        void set_kernel( std::shared_ptr<Kernel> k) final { kernel = std::move( k); }
        void init_checker() final { };
        FuncInstr step();
        Trap run(uint64 instrs_to_run) final;
        void set_target(const Target& target) final {
            PC = target.address;
            sequence_id = target.sequence_id;
        }

        size_t sizeof_register() const final { return bytewidth<RegisterUInt>; }

        uint64 read_cpu_register( uint8 regno) const final {
            return narrow_cast<uint64>( rf.read( Register::from_cpu_index( regno)));
        }

        void write_cpu_register( uint8 regno, uint64 value) final {
            rf.write( Register::from_cpu_index( regno), narrow_cast<RegisterUInt>( value));
        }
};

#endif
