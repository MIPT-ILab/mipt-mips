/**
 * main.cpp - entry point of the scalar MIPS CPU simulator
 * @author Ladin Oleg
 * Copyright 2017-2018 MIPT-MIPS
 */

/* Simulator modules. */
#include <infra/config/config.h>
#include <memory/elf/elf_loader.h>
#include <memory/memory.h>
#include <simulator.h>

namespace config {
    static AliasedRequiredValue<std::string> binary_filename = { "b", "binary", "input binary file"};
    static AliasedValue<uint64> num_steps = { "n", "numsteps", MAX_VAL64, "number of instructions to run"};
} // namespace config

int main( int argc, const char* argv[]) try {
    config::handleArgs( argc, argv, 1);
    auto memory = FuncMemory::create_hierarchied_memory();
    ::load_elf_file( memory.get(), config::binary_filename);

    auto sim = Simulator::create_configured_simulator();
    sim->set_memory( memory.get());
    sim->init_checker();
    sim->run( config::num_steps);
    return 0;
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
