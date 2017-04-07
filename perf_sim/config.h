/*
 * config.h - class for analysing and handling of inputed arguments
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2017 MIPT-MIPS
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
using namespace std;

class Config
{
private:
    /* variables */
    string binary_filename;
    int    num_steps;
    bool   disassembly_on;

public:
    /* constructors */
    Config();
    ~Config();

    /* methods */
    int handleArgs( int argc, char** argv);

    /* get methods */
    string binaryFilename() const;
    int    numSteps()       const;
    bool   disassemblyOn()  const;

};

#endif  // CONFIG_H
