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


template <typename ISA>
class FuncSim : public Simulator
{
    using FuncInstr = typename ISA::FuncInstr;
    using Register = typename ISA::Register;
    using RegisterUInt = typename ISA::RegisterUInt;

    private:
        RF<FuncInstr> rf;
        uint64 sequence_id = 0;
        std::shared_ptr<FuncMemory> mem;
        InstrMemoryCached<ISA> imem;
        std::shared_ptr<Kernel> kernel;

        std::array<Addr, 8> pc = {};
        size_t delayed_slots = 0;
        void update_pc( const FuncInstr& instr);

        uint64 nops_in_a_row = 0;
        void update_and_check_nop_counter( const FuncInstr& instr);
        Trap handle_syscall();
        Trap step_system();

        uint64 read_register( Register index) const { return narrow_cast<uint64>( rf.read( index)); }
        void write_register( Register index, uint64 value) { return rf.write( index, narrow_cast<RegisterUInt>( value)); }

    public:
        FuncSim( Endian endian, bool log = false);

        void set_memory( std::shared_ptr<FuncMemory> memory) final;
        void set_kernel( std::shared_ptr<Kernel> k) final { kernel = std::move( k); }
        void init_checker() final { };
        FuncInstr step();
        Trap run( uint64 instrs_to_run) final;
        Trap run_single_step() final;
        Trap run_until_trap( uint64 instrs_to_run) final;

        void set_target(const Target& target) final {
            pc[0] = target.address;
            delayed_slots = 0;
            sequence_id = target.sequence_id;
        }
        Addr get_pc() const final { return pc[0]; }

        size_t sizeof_register() const final { return bytewidth<RegisterUInt>; }

        uint64 read_cpu_register( uint8 regno) const final { return read_register( Register::from_cpu_index( regno)); }
        uint64 read_gdb_register( uint8 regno) const final;

        void write_cpu_register( uint8 regno, uint64 value) final { write_register( Register::from_cpu_index( regno), value); }
        void write_gdb_register( uint8 regno, uint64 value) final;

        void write_csr_register( std::string_view name, uint64 value) final { write_register( Register::from_csr_name( name), value); }
};

#endif
