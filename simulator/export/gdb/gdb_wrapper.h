/**
 * gdb_wrapper.h - Functional simulator interfaces for GDB
 * @author Vyacheslav Kompan
 * Copyright 2018-2019 MIPT-MIPS
 */

/*
 * gdb_wrapper.h and gdb_wrapper.cpp contain all the code for
 * GDB intergration without having the GDB headers included, so
 * it can be compiled by default builds and tested with Catch
 * even if no GDB code exists in the environment.
 * Please do not include any GDB headers here
 */

#ifndef GDB_WRAPPER_H
#define GDB_WRAPPER_H

#include <func_sim/traps/trap.h>
#include <infra/types.h>

#include <memory>
#include <string>
#include <vector>

class GDBSim
{
    std::shared_ptr<class Simulator> cpu = nullptr;
    std::shared_ptr<class FuncMemory> memory = nullptr;
    std::shared_ptr<class Kernel> kernel = nullptr;
    Trap trap = Trap(Trap::NO_TRAP);
public:
    explicit GDBSim( const std::string& isa);

    bool load( const std::string& filename) const;
    void shutdown();
    void resume( uint64 step);
    bool create_inferior( Addr start_addr, const char* const* argv, const char* const* envp) const;

    // Not implemented yet
    int  stop() { return 0; } // NOLINT(readability-convert-member-functions-to-static)
    void do_command( const std::string& /*command*/ ) { } // NOLINT(readability-convert-member-functions-to-static)
    void info( int /* verbose */) { } // NOLINT(readability-convert-member-functions-to-static)
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    char** sim_complete_command( const std::string& /*text*/, const std::string& /*word*/) { return nullptr; }

    int memory_read( std::byte* dst, Addr src, size_t length) const;
    int memory_write( Addr dst, const std::byte* src, size_t length) const;
    int read_register( int regno, std::byte* buf, int length) const;
    int write_register( int regno, const std::byte* buf, int length) const;

    auto get_trap() const { return trap; }
    int get_exit_code() const;
};

class GDBSimVector
{
    std::vector<GDBSim> st;
public:
    int allocate_new( const std::string& isa, const char* const* argv);
    GDBSim& at(size_t i);
};

#endif
