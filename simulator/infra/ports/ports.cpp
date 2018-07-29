/**
 * ports.cpp - template for simulation of ports.
 * @author Pavel Kryukov
 * Copyright 2017 MIPT-MIPS team
 */

#include "ports.h"

std::list<BasePort::BaseMap*> BasePort::BaseMap::all_maps;

void init_ports()
{
    for ( auto map : BasePort::BaseMap::all_maps)
        map->init();
}

void clean_up_ports( Cycle cycle)
{
    for ( auto map : BasePort::BaseMap::all_maps)
        map->clean_up( cycle);
}

void destroy_ports()
{
    for ( auto map : BasePort::BaseMap::all_maps)
        map->destroy();
}

