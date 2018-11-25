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
class Kernel;

class Simulator : public Log {
public:
    explicit Simulator( bool log = false) : Log( log) {}

    virtual Trap run( uint64 instrs_to_run) = 0;
    virtual void set_target( const Target& target) = 0;
    virtual void set_memory( std::shared_ptr<FuncMemory> m) = 0;
    virtual void set_kernel( std::shared_ptr<Kernel> k) = 0;
    virtual void init_checker() = 0;

    Trap run_no_limit() { return run( MAX_VAL64); }
    void set_pc( Addr pc) { set_target( Target( pc, 0)); }

    static std::shared_ptr<Simulator> create_simulator( const std::string& isa, bool functional_only, bool log);
    static std::shared_ptr<Simulator> create_configured_simulator();

    virtual size_t sizeof_register() const = 0;
    virtual uint64 read_cpu_register( uint8 regno) const = 0;
    virtual void write_cpu_register( uint8 regno, uint64 value) = 0;

    int get_exit_code() const { return exit_code; }
protected:
    int exit_code = 0;
};

class CycleAccurateSimulator : public Simulator {
public:
    explicit CycleAccurateSimulator( bool log = false) : Simulator( log) {}
    virtual void clock() = 0;
    virtual void halt() = 0;
    static std::shared_ptr<CycleAccurateSimulator> create_simulator(const std::string& isa, bool log);
};

#endif // SIMULATOR_H
