/**
 * ports.cpp - template for simulation of ports.
 * @author Pavel Kryukov
 * Copyright 2017 MIPT-MIPS team
 */

#include "ports.h"

std::list<GlobalPortMap*> GlobalPortMap::all_maps;

void init_ports()
{
    for ( auto map : GlobalPortMap::all_maps)
        map->init();
}

void check_ports( uint64 cycle)
{
    for ( auto map : GlobalPortMap::all_maps)
        map->check( cycle);
}

