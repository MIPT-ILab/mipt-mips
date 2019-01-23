/**
 * gdb_wrapper.h - Functional simulator interfaces for GDB
 * @author Vyacheslav Kompan
 * Copyright 2018-2019 MIPT-MIPS
 */

#ifndef GDB_WRAPPER_H
#define GDB_WRAPPER_H

#include <func_sim/trap_types.h>
#include <infra/byte.h>

#include <memory>
#include <string>
#include <vector>

class GDBSim
{
    std::shared_ptr<class Simulator> cpu = nullptr;
    std::shared_ptr<class FuncMemory> memory = nullptr;
    Trap trap = Trap::NO_TRAP;
public:
    GDBSim();

    bool load( const std::string& filename) const;
    void shutdown();
    void resume( uint64 step);
    bool create_inferior( Addr start_addr) const;

    int memory_read( Byte* dst, Addr src, size_t length) const;
    int memory_write( Addr dst, const Byte* src, size_t length) const;
    int read_register( int regno, Byte* buf, int length) const;
    int write_register( int regno, const Byte* buf, int length) const;

    auto get_trap() const { return trap; }
    int get_exit_code() const;
};

class GDBSimVector
{
    std::vector<GDBSim> st;
public:
    int allocate_new( const char* const* argv);
    GDBSim& at(size_t i);
};

#endif
