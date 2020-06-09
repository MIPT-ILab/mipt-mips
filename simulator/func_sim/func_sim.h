/*
 * func_sim.cpp - extremely simple simulator
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS
 */

#ifndef FUNC_SIM_H
#define FUNC_SIM_H

#include "instr_memory.h"
#include "rf/rf.h"

#include <infra/config/config.h>
#include <infra/exception.h>
#include <memory/memory.h>
#include <simulator.h>

#include <memory>
#include <string>

class Driver;
class Operation;

// NOLINTNEXTLINE(fuchsia-multiple-inheritance) Cannot inherit Simulator from Log, since PerfSim inherits Module as well
class BasicFuncSim : public Simulator, public Log {
protected:
    explicit BasicFuncSim( std::string_view isa) : Simulator( isa) { }
};

template <typename ISA>
class FuncSim : public BasicFuncSim
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
        std::unique_ptr<Driver> driver;

        std::array<Addr, 8> pc = {};
        size_t delayed_slots = 0;
        void update_pc( const FuncInstr& instr);

        uint64 nops_in_a_row = 0;
        void update_and_check_nop_counter( const FuncInstr& instr);

        uint64 read_register( Register index) const { return narrow_cast<uint64>( rf.read( index)); }
        void write_register( Register index, uint64 value) { rf.write( index, narrow_cast<RegisterUInt>( value)); }

    public:
        FuncSim( std::endian endian, bool log, std::string_view isa);

        void set_memory( std::shared_ptr<FuncMemory> memory) final;
        void set_kernel( std::shared_ptr<Kernel> k) final { kernel = std::move( k); }
        void enable_driver_hooks() final;
        void disable_checker() final { };
        int get_exit_code() const noexcept final;
        FuncInstr step();
        Trap driver_step( const Operation& instr);
        Trap run( uint64 instrs_to_run) final;

        void set_target(const Target& target) final {
            pc[0] = target.address;
            delayed_slots = 0;
            sequence_id = target.sequence_id;
        }
        Addr get_pc() const final { return pc[0]; }

        size_t sizeof_register() const final { return bytewidth<RegisterUInt>; }
        size_t max_cpu_register() const final { return Register::MAX_REG; }

        uint64 read_cpu_register( size_t regno) const final { return read_register( Register::from_cpu_index( regno)); }
        uint64 read_gdb_register( size_t regno) const final;
        uint64 read_csr_register( std::string_view name) const final { return read_register( Register::from_csr_name( name)); }

        void write_cpu_register( size_t regno, uint64 value) final { write_register( Register::from_cpu_index( regno), value); }
        void write_gdb_register( size_t regno, uint64 value) final;
        void write_csr_register( std::string_view name, uint64 value) final { write_register( Register::from_csr_name( name), value); }
};

#endif
