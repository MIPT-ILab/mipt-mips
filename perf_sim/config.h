/*
 * config.h - class for analysing and handling of inputed arguments
 * Copyright 2017 MIPT-MIPS
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class Config
{
    template<typename T>
    class Value {
        T value;
        T* get_ptr() { return &value; }
        friend class Config;
    public:
        operator const T&() const { return value; }
    };
public:
    /* variables */
    Value<std::string> binary_filename;
    Value<int>         num_steps;
    Value<bool>        disassembly_on;
    Value<bool>        functional_only;

    /* constructors */
    Config();
    ~Config();

    /* methods */
    int handleArgs( int argc, char** argv);
};

#endif  // CONFIG_H
