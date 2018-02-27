/*
 * simulator.h - interface for simulator
 * Copyright 2018 MIPT-MIPS
 */

#ifndef SIMULATOR_H
#define SIMULATOR_H
 
#include <memory>
 
#include <infra/types.h>
#include <infra/log.h>
 
class Simulator : public Log {
public:
    explicit Simulator( bool log = false) : Log( log) {}

    virtual void run(const std::string& tr, uint64 instrs_to_run) = 0;
    virtual void set_PC(Addr value) = 0;

    static std::unique_ptr<Simulator> create_simulator(const std::string& isa, bool functional_only, bool log);
};

#endif // SIMULATOR_H
