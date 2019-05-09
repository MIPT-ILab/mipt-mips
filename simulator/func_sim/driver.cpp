/**
 * driver.cpp - exception handler
 * @author Vsevolod Pukhov, Pavel Kryukov
 * Copyright 2019 MIPT-MIPS
 */

#include "driver.h"

#include <boost/tokenizer.hpp>

Driver::Driver( const std::string& mode, Simulator* /* sim */)
{
    for ( const auto& e : boost::tokenizer( mode, boost::char_separator(",")))
        if ( e == "stop")
            handle_trap_mode = HandleTrapMode::STOP;
        else if ( e == "stop_on_halt")
            handle_trap_mode = HandleTrapMode::STOP_ON_HALT;
        else if ( e == "ignore")
            handle_trap_mode = HandleTrapMode::IGNORE;
        else if ( e == "critical")
            handle_trap_critical = true;
        else if ( e == "verbose")
            handle_trap_verbose = true;
}

Trap Driver::handle_trap( Trap trap) const
{
    if ( trap == Trap::NO_TRAP)
        return trap;

    if ( handle_trap_verbose)
        std::cout << "\tFuncSim trap: " << trap << std::endl;

    if ( handle_trap_critical)
        throw std::runtime_error( "critical trap");

    switch ( handle_trap_mode)
    {
    case HandleTrapMode::STOP_ON_HALT: return trap == Trap::HALT ? trap : Trap(Trap::NO_TRAP);
    case HandleTrapMode::IGNORE: return Trap(Trap::NO_TRAP);
    default: return trap;
    }
}
