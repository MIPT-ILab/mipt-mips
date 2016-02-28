/*
 * main.cpp - mips performance simulator
 * @author Grigoriy Chirkov grigoriy.chirkov@phystech.edu
 * Copyright 2016 MIPT-MIPS
 */


#include "perf_sim.h"
#include <iostream>
#include <cstdlib>
#include <cstring>

const char* USAGE = "USAGE: filename    #instrs to run      [-d] full output";

int main( int argc, char* argv[])
{
    bool silent = false;
    if ( argc != 3 && argc != 4)
    {
        std::cout << USAGE << endl;
        std::exit( EXIT_FAILURE);
    }
    if ( argc == 4 && strcmp( argv[3], "-d"))
    {
        std::cout << USAGE << endl;
        std::exit( EXIT_FAILURE);
    }
    if ( argc == 3)
        silent = true;

    PerfMIPS* mips = new PerfMIPS();
    mips->run( std::string( argv[1]), atoi( argv[2]), silent);
    delete mips;
    return 0;
}
