/*
 * simulator.h - interface for simulator
 * Copyright 2018 MIPT-MIPS
 */

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <infra/log.h>
#include <infra/target.h>
#include <infra/types.h>
#include <func_sim/trap_types.h>
 
#include <memory>

class Simulator : public Log {
public:
    explicit Simulator( bool log = false) : Log( log) {}

    virtual Trap run( const std::string& tr, uint64 instrs_to_run) = 0;
    Trap run_no_limit( const std::string& tr) { return run( tr, MAX_VAL64); }
    virtual void set_target( const Target& target) = 0;

    static std::unique_ptr<Simulator> create_simulator( const std::string& isa, bool functional_only, bool log);
    static std::unique_ptr<Simulator> create_configured_simulator();
};

class CycleAccurateSimulator : public Simulator {
public:
    explicit CycleAccurateSimulator( bool log = false) : Simulator( log) {}
    virtual void clock() = 0;
    virtual void halt() = 0;
    static std::unique_ptr<CycleAccurateSimulator> create_simulator(const std::string& isa, bool log);
};

#endif // SIMULATOR_H
