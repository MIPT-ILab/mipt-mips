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

#include <func_sim/func_sim.h>
#include <core/perf_sim.h>

namespace config {
    static RequiredValue<std::string> binary_filename = { "binary,b", "input binary file"};
    static RequiredValue<uint64> num_steps = { "numsteps,n", "number of instructions to run"};

    static Value<bool> disassembly_on = { "disassembly,d", false, "print disassembly"};
    static Value<bool> functional_only = { "functional-only,f", false, "run functional simulation only"};
} // namespace config

int main( int argc, const char* argv[])
{
    try {
        /* Analysing and handling of inserted arguments */
        config::handleArgs( argc, argv);

        /* running simulation */
        if ( !config::functional_only)
        {
            PerfMIPS p_mips( config::disassembly_on);
            p_mips.run( config::binary_filename,
                        config::num_steps);
        }
        else
        {
            MIPS mips( config::disassembly_on);
            mips.run( config::binary_filename, config::num_steps);
        }
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

