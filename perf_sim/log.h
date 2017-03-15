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

    friend LogOstream& operator<<(LogOstream& out, const Critical&) {
         exit(-1);
    }

    LogOstream& operator<<(std::ostream& (*F)(std::ostream&)) {
        if ( enable)
            F(stream);
        return *this;
    }

    template<typename T>
    friend LogOstream& operator<<(LogOstream& out, const T& v) {
        if ( out.enable) {
            out.stream << v;
        }
        
        return out;
    }
};

class Log
{
public:
    LogOstream sout;
    LogOstream serr;
    LogOstream::Critical critical;

    Log(bool value) : sout(value, std::cout), serr(true, std::cerr) { }
};


#endif /* LOG_H */
