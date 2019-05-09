/**
 * driver.h - exception handler
 * @author Vsevolod Pukhov, Pavel Kryukov
 * Copyright 2019 MIPT-MIPS
 */

#ifndef DRIVER_H
#define DRIVER_H

#include "traps/trap.h"

#include <string>

class Simulator;

class Driver
{
public:
    Driver( const std::string& mode, Simulator* sim);
    Trap handle_trap( Trap trap) const;
private:
    enum class HandleTrapMode : uint8
    {
        STOP,
        STOP_ON_HALT,
        IGNORE,
    } handle_trap_mode = HandleTrapMode::STOP_ON_HALT;

    bool handle_trap_critical = false;
    bool handle_trap_verbose = false;
};

#endif
