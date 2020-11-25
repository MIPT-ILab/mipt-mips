/*
 * target.cpp - input interface for Fetch module
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2020 MIPT-MIPS
 */

#include <infra/target.h>

#include <iostream>

std::ostream& operator<<( std::ostream& out, const Target& target)
{
    if ( target.valid)
        return out << target.address;

    return out << "invalid";
}
