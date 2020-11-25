/**
 * gdb_wrapper.cpp - Functional simulator interfaces for GDB
 * @author Vyacheslav Kompan
 * Copyright 2018-2019 MIPT-MIPS
 */
 
#include "gdb_wrapper.h"

#include <infra/argv.h>
#include <infra/config/config.h>
#include <kernel/kernel.h>
#include <memory/argv_loader/argv_loader.h>
#include <memory/memory.h>
#include <simulator.h>

#include <iostream>

GDBSim::GDBSim( const std::string& isa)
{
    cpu = Simulator::create_configured_isa_simulator( isa);
    cpu->enable_driver_hooks();
    memory = FuncMemory::create_default_hierarchied_memory();
    kernel = Kernel::create_configured_kernel();
    cpu->set_memory( memory);
    kernel->set_simulator( cpu);
    kernel->connect_memory( memory);
    kernel->connect_exception_handler();
    cpu->set_kernel( kernel);
    cpu->disable_checker(); // FIXME(pkryukov): checker should work correctly
}

bool GDBSim::load( const std::string& filename) const try
{
    kernel->load_file( filename);
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

bool GDBSim::create_inferior( Addr start_addr, const char* const* argv, const char* const* envp) const try
{
    Addr sp = cpu->read_gdb_register( 29);

    if ( cpu->sizeof_register() == bytewidth<uint32>)
        sp += ArgvLoader<uint32, std::endian::native>( argv, envp).load_to( memory, sp);

    if ( cpu->sizeof_register() == bytewidth<uint64>)
        sp += ArgvLoader<uint64, std::endian::native>( argv, envp).load_to( memory, sp);

    while ( sp % 4 != 0) ++sp;

    cpu->write_gdb_register( 29, sp);
    std::cout << "MIPT-MIPS: arguments loaded" << std::endl;

    cpu->set_pc( start_addr);
    std::cout << "MIPT-MIPS: prepared to run" << std::endl;
    return true;
}
catch (const std::exception& e) {
    std::cerr << "MIPT-MIPS: failed to load arguments:\n\t" << e.what() << std::endl;
    return false;
}
catch (...) {
    std::cerr << "MIPT-MIPS: failed to load arguments due to unknown exception" << std::endl;
    return false;
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
    trap = cpu->run( instrs_to_run);
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

int GDBSim::memory_read( std::byte* dst, Addr src, size_t length) const
{
    return narrow_cast<int>( memory->memcpy_guest_to_host( dst, src, length));
}

int GDBSim::memory_write( Addr dst, const std::byte* src, size_t length) const
{
    return narrow_cast<int>( memory->memcpy_host_to_guest_noexcept( dst, src, length));
}

int GDBSim::read_register(int regno, std::byte* buf, int length) const
{
    if ( length == 8)
        put_value_to_pointer<uint64, std::endian::native>( buf, cpu->read_gdb_register( regno), 8);
    else if ( length == 4)
        put_value_to_pointer<uint32, std::endian::native>( buf, cpu->read_gdb_register( regno), 4);
    else
        return 0;

    return length;
}

int GDBSim::write_register(int regno, const std::byte* buf, int length) const
{
    if ( length == 8)
        cpu->write_gdb_register( regno, get_value_from_pointer<uint64, std::endian::native>( buf, 8));
    else if ( length == 4)
        cpu->write_gdb_register( regno, get_value_from_pointer<uint32, std::endian::native>( buf, 4));
    else
        return 0;

    return length;
}

int GDBSim::get_exit_code() const
{
    return cpu->get_exit_code();
}

int GDBSimVector::allocate_new( const std::string& isa, const char* const* argv) try
{
    /* argv[0] has to be ignored */
    config::handleArgs( count_argc( argv), argv, 2);
    st.emplace_back( GDBSim( isa));
    auto id = st.size() - 1;
    std::cout << "MIPT-MIPS: simulator instance created, id " << id << std::endl;
    return narrow_cast<int>( id);
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
