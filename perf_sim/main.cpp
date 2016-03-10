/*
 * main.cpp - mips functional simulator
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS 
 */

#include <iostream>
#include <cstdlib>
#include <cstring>

#include <perf_sim.h>

int main( int argc, char* argv[])
{
    bool silent = true;

    if ( argc == 3)
    {
        MIPS* mips = new MIPS();
        mips->run(std::string(argv[1]), atoi(argv[2]), true);
        delete mips; 
    }
    else if ( argc == 4)
    {
        silent = (strcmp("-d", argv[1]) != 0);
        MIPS* mips = new MIPS();
        mips->run(std::string(argv[2]), atoi(argv[3]), silent);
        delete mips;
    }
    else
    {
        std::cout << "2 or 3 arguments required: mips_exe [-d] filename and amount of instrs to run" << endl;
        std::exit(EXIT_FAILURE);
    }

    return 0;
}

