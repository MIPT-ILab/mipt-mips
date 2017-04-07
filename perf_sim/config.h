/*
 * config.h - class for analysing and handling of inputed arguments
 * Copyright 2017 MIPT-MIPS
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class Config
{
public:
    /* variables */
    std::string binary_filename;
    int         num_steps;
    bool        disassembly_on;

    /* constructors */
    Config();
    ~Config();

    /* methods */
    int handleArgs( int argc, char** argv);
};

#endif  // CONFIG_H
