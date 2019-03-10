/**
 * main.cpp - entry point of the scalar MIPS CPU simulator
 * @author Ladin Oleg
 * Copyright 2017-2018 MIPT-MIPS
 */

/* Simulator modules. */
#include <infra/config/config.h>
#include <kernel/kernel.h>
#include <memory/elf/elf_loader.h>
#include <memory/memory.h>
#include <simulator.h>

namespace config {
    static AliasedRequiredValue<std::string> binary_filename = { "b", "binary", "input binary file"};
    static AliasedValue<uint64> num_steps = { "n", "numsteps", MAX_VAL64, "number of instructions to run"};
} // namespace config

int main( int argc, const char* argv[]) try {
    config::handleArgs( argc, argv, 1);
    ElfLoader elf( config::binary_filename);
    auto memory = FuncMemory::create_hierarchied_memory();
    elf.load_to( memory.get());

    auto sim = Simulator::create_configured_simulator();
    sim->set_memory( memory);
    sim->init_checker();
    sim->set_pc( elf.get_startPC());
    sim->write_csr_register( "mscratch", 0x400'0000);

    auto kernel = Kernel::create_configured_kernel();
    kernel->set_memory( memory);
    kernel->set_simulator( sim);
    sim->set_kernel( kernel);

    sim->run( config::num_steps);
    return sim->get_exit_code();
}
catch (const config::HelpOption& e) {
    std::cout << "Functional and performance simulators for MIPS-based CPU."
              << std::endl << std::endl << e.what() << std::endl;
    return 0;
}
catch (const Exception& e) {
    std::cerr << e.what() << std::endl;
    return 2;
}
catch (const std::exception& e) {
    std::cerr << "System exception:\t\n" << e.what() << std::endl;
    return 2;
}
catch (...) {
    std::cerr << "Unknown exception\n";
    return 3;
}
