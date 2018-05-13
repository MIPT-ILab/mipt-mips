/*
 * config.cpp - implementation of Config class
 * Copyright 2017-2018 MIPT-MIPS
 */

#include <infra/ports/timing.h>

/* Simulator modules */
#include "config.h"

namespace config {

static AliasedSwitch help_option = { "h", "help", "print help"};

/* basic method */
void handleArgs( int argc, const char* argv[])
{
    BaseValue::values().parse( argc, argv);

    /* parsing help */
    if ( help_option)
    {
        std::cout << "Functional and performance simulators for MIPS-based CPU."
                  << std::endl << std::endl
                  << BaseValue::values() << std::endl;
        std::exit( EXIT_SUCCESS);
    }
    else if ( !BaseValue::values().unknown_options().empty() && !BaseValue::values().non_option_args().empty()) {
        std::cerr << "Unknown options:" << std::endl;
        for ( const auto& opt: BaseValue::values().unknown_options())
            std::cerr << opt << std::endl;
        for ( const auto& opt: BaseValue::values().non_option_args())
            std::cerr << opt << std::endl;
        std::cerr << "Correct options are:"
            << std::endl << std::endl
            << BaseValue::values() << std::endl;
        throw std::exception();
    }
}

} // namespace config

