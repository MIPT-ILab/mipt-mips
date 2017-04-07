/*
 * config.cpp - implementation of Config class
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2017 MIPT-MIPS
 */

/* Simulator modules */
#include "config.h"

/* Generic C++ */
#include <iostream>

/* boost - program option parsing */
#include <boost/program_options.hpp>

using namespace std;

/* constructor and destructor */
Config::Config() :
    /* by default, there is no limit of simulation and silent mode is on */
    num_steps( -1),
    disassembly_on( false)
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
        ( "binary,b",      po::value<string> ( &this->binary_filename),
                                                            "Input binary file")

        ( "numsteps,n",    po::value<int>    ( &this->num_steps),
                               "Number of instructions to run. Defaults to -1,"
                               " which means there is no limit for simulation.")

        ( "disassembly,d", po::bool_switch   ( &this->disassembly_on),
                                                            "Print disassembly")

        ( "help,h",
                                                      "Print this help message");

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
        po::notify(vm);
    }
    catch ( const std::exception& e)
    {
        (void)e;
        cout << description << endl;
        exit(0);
    }

    /* parsing help */
    if ( vm.count( "help"))
    {
         cout << "Functional and performance simulators for MIPS-based CPU.";
         cout << endl << endl;
         cout << description << endl;
         exit(0);
    }

    return 0;
}


/* get methods */
string Config::binaryFilename() const
{
    return this->binary_filename;
}


int Config::numSteps() const
{
    return this->num_steps;
}


bool Config::disassemblyOn() const
{
    return this->disassembly_on;
}

