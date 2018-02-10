/**
 * main.cpp - entry point of the scalar MIPS CPU simulator
 * @author Ladin Oleg
 * Copyright 2017 MIPT-MIPS
 */

/* Generic C. */
#include <cstdlib>
#include <cstring>

/* Generic C++ */
#include <memory>

/* Simulator modules. */
#include <infra/config/config.h>
#include <simulator.h>

namespace config {
    static RequiredValue<std::string> binary_filename = { "binary,b", "input binary file"};
    static RequiredValue<uint64> num_steps = { "numsteps,n", "number of instructions to run"};

    static Value<std::string> isa = { "isa,I", "mips", "modeled ISA"};
    static Value<bool> disassembly_on = { "disassembly,d", false, "print disassembly"};
    static Value<bool> functional_only = { "functional-only,f", false, "run functional simulation only"};
} // namespace config

auto create_simulator()
{
    auto simulator = Simulator::create_simulator( config::isa, config::functional_only, config::disassembly_on);
    if ( simulator == nullptr) {
       std::cerr << "ERROR. Invalid simulation mode " << config::isa << ( config::functional_only ? "-functional" : "-performance") << std::endl;
       std::exit( EXIT_FAILURE);
    }
    return simulator;
}

int main( int argc, const char* argv[])
{
    try {
        /* Analysing and handling of inserted arguments */
        config::handleArgs( argc, argv);
        create_simulator()->run( config::binary_filename, config::num_steps);
    }
    catch (const std::exception& e) {
        std::cerr << *argv << ": " << e.what()
                  << std::endl << std::endl;
        return 2;
    }
    catch (...) {
        std::cerr << "Unknown exception\n";
        return 3;
    }

    return 0;
}

