/*
 * config.cpp - implementation of Config class
 * Copyright 2017 MIPT-MIPS
 */

/* Simulator modules */
#include "config.h"

/* Generic C++ */
#include <iostream>

/* Boost */
#include <boost/program_options.hpp>

namespace config {

namespace po = boost::program_options;

template<>
void Value<bool>::reg(bod& d)
{
    namespace po = boost::program_options;
    d.add_options()(name.c_str(),
                    po::bool_switch( &value)->default_value( default_value),
                    desc.c_str());
}

template<typename T>
void Value<T>::reg(bod& d)
{
    namespace po = boost::program_options;
    if (is_required)
    {
        d.add_options()(name.c_str(),
                    po::value<T>( &value)->default_value( default_value)->required(),
                    desc.c_str());
    }
    else {
        d.add_options()(name.c_str(),
                    po::value<T>( &value)->default_value( default_value),
                    desc.c_str());
    }
}

template class Value<std::string>;
template class Value<uint64>;
template class Value<uint32>;
template class Value<int32>;

/* basic method */
void handleArgs( int argc, char** argv)
{
    po::options_description description( "Allowed options");

    for ( auto value : BaseValue::values())
         value->reg(description);

    po::variables_map vm;

    try
    {
        po::store(po::command_line_parser(argc, argv).
                                    options(description).
                                    run(),
                                    vm);


        /* parsing help */
        if ( vm.count( "help") != 0u)
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
}

} // namespace config

