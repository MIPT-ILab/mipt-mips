/*
 * main.cpp - MIPS single-cycle implementation
 * @author Aleksandr Shashev <aleksandr.shashev.phystech.edu>
 * Copyright 2016 MIPT-MIPS
 */

//Generic C++
 #include <iostream>
 #include <string>
 #include <cstdlib>


//MIPT-MIPS modules
 #include <func_sim.h>

int main ( int argc, char** argv)
{
    if ( argc != 3)
    {
        std::cout << "2 arguments required: mips_exe filename\
                      and num of instructions to run" 
                  << std::endl;
        std::exit( EXIT_FAILURE);
    }

    MIPS* mips = new MIPS;
 
    int arg = 0;
    
    sscanf (argv[2], "%d", &arg);
    
    std::cout << "arg = " << arg << std::endl;
    
    mips->run( argv[1], arg);
    
    delete mips;
    return 0;
}
