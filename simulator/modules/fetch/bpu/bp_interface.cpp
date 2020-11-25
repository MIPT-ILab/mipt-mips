/**
 * bp_interface.cpp - perfoming BP updates using ports
 * @author Aleksandr Misevich
 * Copyright 2018-2019 MIPT-MIPS
 */
 
#include "bp_interface.h"

#include <iostream>

std::ostream& operator<<( std::ostream& out, const BPInterface& info)
{
    return out << "{ pc=0x" << std::hex << info.pc
               << ", target=0x" << info.target
               << ( info.is_taken ? ", T" : ", NT")
               << ( info.is_hit ? ", hit " : ", miss ") << '}' << std::dec;
}
