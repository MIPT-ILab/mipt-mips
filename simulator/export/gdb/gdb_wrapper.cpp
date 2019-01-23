/**
 * gdb_wrapper.cpp - Functional simulator interfaces for GDB
 * @author Vyacheslav Kompan
 * Copyright 2018-2019 MIPT-MIPS
 */
 
#include "gdb_wrapper.h"

#include <infra/config/config.h>
#include <memory/elf/elf_loader.h>
#include <memory/memory.h>
#include <simulator.h>
 
GDBSim::GDBSim()
{
    cpu = Simulator::create_simulator("mips32", true, true);
    memory = FuncMemory::create_hierarchied_memory();
    cpu->set_memory( memory);
}

bool GDBSim::load(const std::string& filename) const try
{
    ElfLoader( filename).load_to( memory.get());
    std::cout << "MIPT-MIPS: Binary file " << filename << " loaded" << std::endl;
    return true;
}
catch (const std::exception& e) {
    std::cerr << "MIPT-MIPS: failed to load program:\n\t" << e.what() << std::endl;
    return false;
}
catch (...) {
    std::cerr << "MIPT-MIPS: failed to load program due to unknown exception" << std::endl;
    return false;
}

bool GDBSim::create_inferior( Addr start_addr) const
{
    cpu->set_pc( start_addr);
    std::cout << "MIPT-MIPS: prepared to run" << std::endl;
    return true;
}

void GDBSim::shutdown()
{
    cpu = nullptr;
    memory = nullptr;
}
    
void GDBSim::resume( uint64 step) try
{
    std::cout << "MIPT-MIPS: resuming, steps: " << step << std::endl;
    uint64 instrs_to_run = (step == 0) ? MAX_VAL64 : step;
    if (instrs_to_run == 1)
        trap = cpu->run_single_step ();
    else
        trap = cpu->run_until_trap( instrs_to_run);
}
catch (const BearingLost &e) {
    trap = Trap::HALT;
    std::cout << "MIPT-MIPS: " << e.what() << std::endl;
}
catch (const std::exception &e) {
    std::cerr << "MIPT-MIPS: " << e.what () << std::endl;
}
catch (...) {
    std::cerr << "MIPT-MIPS: Unknown exception\n";
}

int GDBSim::memory_read( Byte* dst, Addr src, size_t length) const
{
    return memory->memcpy_guest_to_host( dst, src, length);
}

int GDBSim::memory_write( Addr dst, const Byte* src, size_t length) const
{
    return memory->memcpy_host_to_guest_noexcept( dst, src, length);
}

int GDBSim::read_register(int regno, Byte* buf, int length) const
{
    if ( length == 8)
        put_value_to_pointer<uint64, Endian::native>( buf, cpu->read_gdb_register( regno));
    else if ( length == 4)
        put_value_to_pointer<uint32, Endian::native>( buf, cpu->read_gdb_register( regno));
    else
        return 0;

    return length;
}

int GDBSim::write_register(int regno, const Byte* buf, int length) const
{
    if ( length == 8)
        cpu->write_gdb_register( regno, get_value_from_pointer<uint64, Endian::native>( buf));
    else if ( length == 4)
        cpu->write_gdb_register( regno, get_value_from_pointer<uint32, Endian::native>( buf));
    else
        return 0;

    return length;
}

int GDBSim::get_exit_code() const
{
    return cpu->get_exit_code();
}

static int count_argc (const char *const *argv)
{
    /* Passed arguments start at argv[2], end with NULL */
    int argc = 0;
    while (argv[2 + argc] != nullptr)
        argc++;
    return argc;
}

int GDBSimVector::allocate_new( const char* const* argv) try
{
    config::handleArgs( count_argc( argv), argv, 2);
    st.emplace_back( GDBSim());
    auto id = st.size() - 1;
    std::cout << "MIPT-MIPS: simulator instance created, id " << id << std::endl;
    return id;
}
catch (const std::exception &e) {
    std::cerr << "MIPT-MIPS: simulator is not initialized:\n\t" << e.what() << std::endl;
    return -1;
}
catch (...) {
    std::cerr << "MIPT-MIPS: simulator is not initialized due to unknown exception." << std::endl;
    return -1;
}

GDBSim& GDBSimVector::at(size_t i)
{
    return st.at(i);
}
