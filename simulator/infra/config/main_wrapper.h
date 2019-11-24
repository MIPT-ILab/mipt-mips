/**
 * main_wrapper.h - wrapper for entry points of MIPT-V binaries
 * @author Pavel Kryukov
 * Copyright 2019 MIPT-V
 */

#ifndef MAIN_WRAPPER_H
#define MAIN_WRAPPER_H

#include <string_view>
#include <string>

class MainWrapper
{
public:
    explicit MainWrapper( std::string_view desc) : desc( desc) { }
    int run( int argc, const char* argv[]) const ;

private:
    virtual int impl( int argc, const char* argv[]) const = 0;
    std::string desc;
};

#endif
