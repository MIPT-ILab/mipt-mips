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
    static AliasedRequiredValue<std::string> binary_filename = { "b", "binary", "input binary file"};

    static AliasedValue<uint64> num_steps = { "n", "numsteps", MAX_VAL64, "number of instructions to run"};
    static AliasedValue<std::string> isa = { "I", "isa", "mips32", "modeled ISA"};
    static AliasedSwitch disassembly_on = { "d", "disassembly", "print disassembly"};
    static AliasedSwitch functional_only = { "f", "functional-only", "run functional simulation only"};
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

