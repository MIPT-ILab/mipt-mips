/**
 * log.h - Header of log class
 * includes 2 methods to show warrnings and errors
 * @author Pavel Kryukov
 * Copyright 2017 MIPT-MIPS team
 */

#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <ostream>

class LogOstream
{
    const bool enable;
    std::ostream& stream;

public:
    LogOstream(bool value, std::ostream& _out) : enable(value), stream(_out) { }

    LogOstream& operator<<(std::ostream& (*F)(std::ostream&)) {
        if ( enable)
            F(stream);
        return *this;
    }

    template<typename T>
    LogOstream& operator<<(const T& v) {
        if ( enable)
            stream << v;

        return *this;
    }

    template<size_t N>
    LogOstream& operator<<(const char (&array)[N]) {
        return *this << static_cast<const char*>( array);
    }
};

class Log
{
public:
    mutable LogOstream sout;
    mutable LogOstream serr;

    explicit Log(bool value) : sout(value, std::cout), serr(true, std::cerr) { }

    // Rule of five
    virtual ~Log() = default;
    Log( const Log&) = delete;
    Log( Log&&) = delete;
    Log& operator=( const Log&) = delete;
    Log& operator=( Log&&) = delete;
};

#endif /* LOG_H */

