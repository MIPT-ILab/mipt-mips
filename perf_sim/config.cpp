/*
 * config.cpp - implementation of Config class
 * Copyright 2017 MIPT-MIPS
 */

/* Simulator modules */
#include "config.h"

/* Generic C++ */
#include <iostream>

/* boost - program option parsing */
#include <boost/program_options.hpp>

/* constructor and destructor */
Config::Config()
{

}

Config::~Config()
{

}


/* basic method */
int Config::handleArgs( int argc, char** argv)
{
    namespace po = boost::program_options;

    po::options_description description( "Allowed options");

    description.add_options()
        ( "binary,b",      po::value<std::string>( &this->binary_filename)->required(),
          "input binary file")

        ( "numsteps,n",    po::value<int>( &this->num_steps)->required(),
          "number of instructions to run")

        /* by default, the silent mode is on */
        ( "disassembly,d", po::bool_switch( &this->disassembly_on)->default_value( false),
          "print disassembly")

        ( "help,h",
          "print this help message");

    po::positional_options_description posDescription;
    posDescription.add( "binary", 1).add( "numsteps", 2);
    po::variables_map vm;

    try
    {
        po::store(po::command_line_parser(argc, argv).
                                    options(description).
                                    positional(posDescription).
                                    run(),
                                    vm);


        /* parsing help */
        if ( vm.count( "help"))
        {
            std::cout << "Functional and performance simulators for MIPS-based CPU.";
            std::cout << std::endl << std::endl;
            std::cout << description << std::endl;
            std::exit( EXIT_SUCCESS);
        }

        /* calling notify AFTER parsing help, as otherwise
         * absent required args will cause errors
         */
        po::notify(vm);
    }
    catch ( const std::exception& e)
    {
        std::cout << argv[0] << ": " << e.what();
        std::cout << std::endl << std::endl;
        std::cout << description << std::endl;
        std::exit( EXIT_SUCCESS);
    }

    return 0;
}
