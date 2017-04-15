/*
 * config.cpp - implementation of Config class
 * Copyright 2017 MIPT-MIPS
 */

/* Simulator modules */
#include "config.h"

/* Generic C++ */
#include <iostream>

namespace po = boost::program_options;

Config::BasicValue::BasicValue(Config* c)
{
    c->values.push_back(this);
}

template<>
void Config::Value<bool>::reg(bod& d)
{
    namespace po = boost::program_options;
    d.add_options()(name.c_str(),
                    po::bool_switch( &value)->default_value( default_value),
                    desc.c_str());
}

/* basic method */
int Config::handleArgs( int argc, char** argv)
{
    po::options_description description( "Allowed options");

    for ( auto value : values)
         value->reg(description);

    po::variables_map vm;

    try
    {
        po::store(po::command_line_parser(argc, argv).
                                    options(description).
                                    run(),
                                    vm);


        /* parsing help */
        if ( vm.count( "help"))
        {
            std::cout << "Functional and performance simulators for MIPS-based CPU."
                      << std::endl << std::endl
                      << description << std::endl;
            std::exit( EXIT_SUCCESS);
        }

        /* calling notify AFTER parsing help, as otherwise
         * absent required args will cause errors
         */
        po::notify(vm);
    }
    catch ( const std::exception& e)
    {
        std::cerr << argv[0] << ": " << e.what()
                  << std::endl << std::endl
                  << description << std::endl;
        std::exit( EXIT_FAILURE);
    }

    return 0;
}
