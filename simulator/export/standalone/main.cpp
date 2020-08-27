/**
 * main.cpp - entry point of the scalar MIPS CPU simulator
 * @author Ladin Oleg
 * Copyright 2017-2018 MIPT-MIPS
 */

/* Simulator modules. */
#include <infra/config/config.h>
#include <infra/config/main_wrapper.h>
#include <kernel/kernel.h>
#include <memory/memory.h>
#include <simulator.h>

namespace config {
    static const AliasedRequiredValue<std::string> binary_filename = { "b", "binary", "input binary file"};
    static const AliasedValue<uint64> num_steps = { "n", "numsteps", MAX_VAL64, "number of instructions to run"};
    static const Value<std::string> trap_mode = { "trap_mode",  "", "trap handler mode"};
} // namespace config

class Main : public MainWrapper
{
    using MainWrapper::MainWrapper;
private:
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
    int impl( int argc, const char* argv[]) const final; 
};

// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
int Main::impl( int argc, const char* argv[]) const {
    config::handleArgs( argc, argv, 1);
    auto memory = FuncMemory::create_default_hierarchied_memory();

    auto sim = Simulator::create_configured_simulator();
    sim->set_memory( memory);
    sim->write_csr_register( "mscratch", 0x400'0000);

    auto kernel = Kernel::create_configured_kernel();
    kernel->set_simulator( sim);
    kernel->connect_memory( memory);
    kernel->connect_exception_handler();
    kernel->load_file( config::binary_filename);
    sim->set_kernel( kernel);

    sim->set_pc( kernel->get_start_pc());
    sim->run( config::num_steps);
    return sim->get_exit_code();
}

int main( int argc, const char* argv[]) {
    return Main( "Functional and performance simulators for MIPS-based CPU.").run( argc, argv);
}
