/*
 * simulator.h - interface for simulator
 * Copyright 2018 MIPT-MIPS
 */

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <func_sim/trap_types.h>
#include <infra/log.h>
#include <infra/target.h>
#include <infra/types.h>
 
#include <memory>

class FuncMemory;

class Simulator : public Log {
public:
    explicit Simulator( bool log = false) : Log( log) {}

    virtual Trap run( uint64 instrs_to_run) = 0;
    virtual void set_target( const Target& target) = 0;
    virtual void set_memory( FuncMemory* m) = 0;
    virtual void init_checker() = 0;

    Trap run_no_limit() { return run( MAX_VAL64); }
    void set_pc( Addr pc) { set_target( Target( pc, 0)); }

    static std::unique_ptr<Simulator> create_simulator( const std::string& isa, bool functional_only, bool log);
    static std::unique_ptr<Simulator> create_configured_simulator();

    virtual size_t sizeof_register() const = 0;
    template <typename T>
    T read_cpu_register( uint8 regno) {
        return static_cast<T>( this->read_cpu_register_internal( regno));
    }
    template <typename T>
    void write_cpu_register( uint8 regno, T value) {
        this->write_cpu_register_internal( regno, static_cast<int_largest>( value));
    }

private:
    virtual int_largest read_cpu_register_internal( uint8 regno) const = 0;
    virtual void write_cpu_register_internal( uint8 regno, int_largest value) = 0;
};

class CycleAccurateSimulator : public Simulator {
public:
    explicit CycleAccurateSimulator( bool log = false) : Simulator( log) {}
    virtual void clock() = 0;
    virtual void halt() = 0;
    static std::unique_ptr<CycleAccurateSimulator> create_simulator(const std::string& isa, bool log);
};

#endif // SIMULATOR_H
