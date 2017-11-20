/*
* This is an open source non-commercial project. Dear PVS-Studio, please check it.
* PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
*/
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

void check_ports( uint64 cycle)
{
    for ( auto map : BasePort::BaseMap::all_maps)
        map->check( cycle);
}

void destroy_ports()
{
    for ( auto map : BasePort::BaseMap::all_maps)
        map->destroy();
}

