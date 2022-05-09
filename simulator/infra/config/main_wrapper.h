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

    virtual ~MainWrapper() = default;
    MainWrapper( const MainWrapper&) = delete;
    MainWrapper( MainWrapper&&) = delete;
    MainWrapper& operator=( const MainWrapper&) = delete;
    MainWrapper& operator=( MainWrapper&&) = delete;  

    // NOLINTNEXTLINE(*-avoid-c-arrays)
    int run( int argc, const char* argv[]) const;

private:
    // NOLINTNEXTLINE(*-avoid-c-arrays)
    virtual int impl( int argc, const char* argv[]) const = 0;
    std::string desc;
    std::ostream& out;
    std::ostream& err;
};

#endif
