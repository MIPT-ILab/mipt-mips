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
    struct Critical { };

    LogOstream(bool value, std::ostream& _out) : enable(value), stream(_out) { }

    friend LogOstream& operator<<(LogOstream& /*stream*/, const Critical& /* dummy */) {
         exit( EXIT_FAILURE);
    }

    LogOstream& operator<<(std::ostream& (*F)(std::ostream&)) {
        if ( enable)
            F(stream);
        return *this;
    }

    template<typename T>
    LogOstream& operator<<(const T& v) {
        if ( enable) {
            stream << v;
        }

        return *this;
    }
};

class Log
{
public:
    mutable LogOstream sout;
    mutable LogOstream serr;
    const LogOstream::Critical critical;

    explicit Log(bool value) : sout(value, std::cout), serr(true, std::cerr), critical() { }
    virtual ~Log() = default;
};

#endif /* LOG_H */

