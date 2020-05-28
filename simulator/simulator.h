/*
 * simulator.h - interface for simulator
 * Copyright 2018 MIPT-MIPS
 */

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <func_sim/traps/trap.h>
#include <infra/exception.h>
#include <infra/log.h>
#include <infra/ports/module.h>
#include <infra/target.h>
#include <infra/types.h>

#include <memory>
#include <string>
#include <vector>

struct BearingLost final : Exception {
    BearingLost() : Exception("Bearing lost", "10 nops in a row") { }
};

struct InvalidISA final : Exception
{
    explicit InvalidISA( const std::string& isa)
        : Exception("Invalid ISA", isa)
    { }
};

class CPUModel
{
public:
    CPUModel() = default;
    CPUModel( const CPUModel&) = delete;
    CPUModel( CPUModel&&) = delete;
    CPUModel& operator=( const CPUModel&) = delete;
    CPUModel& operator=( CPUModel&&) = delete;
    virtual ~CPUModel() = default;

    void set_pc( Addr pc) { set_target( Target( pc, 0)); }
    virtual void set_target( const Target& target) = 0;
    virtual Addr get_pc() const = 0;
    virtual std::string_view get_isa() const = 0;

    virtual size_t sizeof_register() const = 0;
    virtual size_t max_cpu_register() const = 0;

    virtual uint64 read_cpu_register( size_t regno) const = 0;
    virtual uint64 read_gdb_register( size_t regno) const = 0;
    virtual uint64 read_csr_register( std::string_view name) const = 0;

    virtual void write_cpu_register( size_t regno, uint64 value) = 0;
    virtual void write_gdb_register( size_t regno, uint64 value) = 0;
    virtual void write_csr_register( std::string_view name, uint64 value) = 0;

    void duplicate_all_registers_to( CPUModel* model) const;
};

class FuncMemory;
class Kernel;

class Simulator : public CPUModel
{
public:
    explicit Simulator( std::string_view i) : isa( i) { }
    virtual Trap run( uint64 instrs_to_run) = 0;
    virtual void set_memory( std::shared_ptr<FuncMemory> m) = 0;
    virtual void set_kernel( std::shared_ptr<Kernel> k) = 0;
    virtual void disable_checker() = 0;
    virtual void enable_driver_hooks() = 0;
    virtual int get_exit_code() const noexcept = 0;
    std::string_view get_isa() const final { return isa; }

    Trap run_no_limit() { return run( MAX_VAL64); }

    static std::vector<std::string> get_supported_isa();
    static std::shared_ptr<Simulator> create_simulator( const std::string& isa, bool functional_only, bool log);
    static std::shared_ptr<Simulator> create_simulator( const std::string& isa, bool functional_only);
    static std::shared_ptr<Simulator> create_configured_simulator();
    static std::shared_ptr<Simulator> create_configured_isa_simulator( const std::string& isa);
    static std::shared_ptr<Simulator> create_functional_simulator( const std::string& isa, bool log)
    {
        return create_simulator( isa, true, log);
    }
    static std::shared_ptr<Simulator> create_functional_simulator( const std::string& isa)
    {
        return create_functional_simulator( isa, false);
    }
private:
    std::string isa;
};

// NOLINTNEXTLINE(fuchsia-multiple-inheritance) Need to mix timing and functional model somewhere...
class CycleAccurateSimulator : public Simulator, public Root
{
public:
    explicit CycleAccurateSimulator( std::string_view isa) : Simulator( isa), Root( "cpu") { }
    virtual void clock() = 0;
    static std::shared_ptr<CycleAccurateSimulator> create_simulator(const std::string& isa);
};

#endif // SIMULATOR_H
