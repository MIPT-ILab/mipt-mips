/**
 * exception.h - basic exception types
 * @author Pavel Kryukov
 * Copyright 2018 MIPT-MIPS team
 */

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>
#include <string>
#include <string_view>

struct Exception : std::runtime_error
{
protected:
    Exception( const std::string_view& description, const std::string& message)
        : std::runtime_error( std::string( description) + ":\t" + message + '\n')
    { }
public:
    explicit Exception( const std::string& message)
        : Exception( "Unqualified exception", message)
    { }

    Exception() = delete;
    ~Exception() override = default;
    Exception( const Exception&) = delete;
    Exception( Exception&&) = delete;
    Exception& operator=( const Exception&) = delete;
    Exception& operator=( Exception&&) = delete;
};

#endif /* EXCEPTION_H */
