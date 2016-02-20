/*
 * main.cpp - mips functional simulator
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS 
 */

#include <iostream>
#include <cstdlib>

#include <func_sim.h>

int main( int argc, char* argv[])
{
    if ( argc != 3)
    {
        std::cout << "2 arguments required: mips_exe filename and amount of instrs to run" << endl;
        std::exit(EXIT_FAILURE);
    }

    MIPS* mips = new MIPS();
    mips->run(std::string(argv[1]), atoi(argv[2]));
    delete mips;

    return 0;
}

