/**
 * main_wrapper.h - wrapper for entry points of MIPT-V binaries
 * @author Pavel Kryukov
 * Copyright 2019 MIPT-V
 */

#ifndef MAIN_WRAPPER_H
#define MAIN_WRAPPER_H

#include <ostream>
#include <string>
#include <string_view>

class MainWrapper
{
public:
    MainWrapper( std::string_view desc, std::ostream& out, std::ostream& err)
        : desc( desc) 
        , out( out)
        , err( err)
    { }
    explicit MainWrapper( std::string_view desc);

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
    int run( int argc, const char* argv[]) const;

private:
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
    virtual int impl( int argc, const char* argv[]) const = 0;
    std::string desc;
    std::ostream& out;
    std::ostream& err;
};

#endif
