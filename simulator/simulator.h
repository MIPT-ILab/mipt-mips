/*
 * simulator.h - interface for simulator
 * Copyright 2018 MIPT-MIPS
 */

#ifndef SIMULATOR_H
#define SIMULATOR_H

/* Simulator modules */
#include <infra/log.h>
#include <infra/target.h>
#include <infra/types.h>
#include <infra/trap_types.h>
#include <infra/exception.h>
/* Generic C++ */
#include <memory>
#include <utility>

struct BearingLost final : Exception {
    BearingLost() : Exception("Bearing lost", "10 nops in a row") { }
};

struct NoBinaryFile final : Exception {
    NoBinaryFile() : Exception("No binary file loaded", "can't run simulator without binary file") { }
};

class Simulator : public Log {
public:
    explicit Simulator( bool log = false) : Log( log) {}

    enum class StopReason : int { Halted, TrapHit, Error };

    virtual void load_binary_file( const std::string &tr) = 0;
    virtual void prepare_to_run() = 0;
    virtual void init( const std::string& tr) = 0;
    virtual std::pair<StopReason, TrapType> run( uint64 instrs_to_run) = 0;
    virtual std::pair<StopReason, TrapType> run_single_step() = 0;
    void run_no_limit( const std::string& tr) { init( tr); run( MAX_VAL64); }
    virtual void set_target( const Target& target) = 0;

    static std::unique_ptr<Simulator> create_simulator( const std::string& isa, bool functional_only, bool log);
    static std::unique_ptr<Simulator> create_configured_simulator();

    virtual size_t mem_read( Addr addr, unsigned char *buf, size_t length) const = 0;
    virtual size_t mem_read_noexcept( Addr addr, unsigned char *buf, size_t length) const noexcept = 0;
    virtual size_t mem_write( Addr addr, const unsigned char *buf, size_t length) = 0;
    virtual size_t mem_write_noexcept( Addr addr, const unsigned char *buf, size_t length) noexcept = 0;

protected:
    bool binary_file_loaded = false;
};

class CycleAccurateSimulator : public Simulator {
public:
    explicit CycleAccurateSimulator( bool log = false) : Simulator( log) {}
    virtual void clock() = 0;
    virtual void halt() = 0;
    static std::unique_ptr<CycleAccurateSimulator> create_simulator(const std::string& isa, bool log);
};

#endif // SIMULATOR_H
