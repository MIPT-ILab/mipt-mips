/*
 * simulator.h - interface for simulator
 * Copyright 2018 MIPT-MIPS
 */

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <infra/log.h> 
#include <infra/types.h>
 
#include <memory>

class Simulator : public Log {
public:
    explicit Simulator( bool log = false) : Log( log) {}

    virtual void run( const std::string& tr, uint64 instrs_to_run) = 0;
    void run_no_limit( const std::string& tr) { run( tr, MAX_VAL64); }
    virtual void set_PC( Addr value) = 0;

    static std::unique_ptr<Simulator> create_simulator( const std::string& isa, bool functional_only, bool log);
    static std::unique_ptr<Simulator> create_configured_simulator();
};

#endif // SIMULATOR_H
