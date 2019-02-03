/**
 * ports.cpp - template for simulation of ports.
 * @author Pavel Kryukov
 * Copyright 2017 MIPT-MIPS team
 */

#include "ports.h"

/*
 * Initialize map of ports.
 *
 * Iterates all map and initalizes all port trees
*/
void PortMap::init() const
{
    for ( const auto& cluster : _map)
    {
        auto writer = cluster.second.writer;
        if (writer == nullptr)
            throw PortError( cluster.first + " has no WritePort");

        writer->init( cluster.second.readers);
    }
}

/*
 * Destroy map of ports.
 *
 * Iterates all map and de-initalizes all port trees
 * Then destroys content of map
*/
void PortMap::destroy()
{
    for ( const auto& cluster : _map)
        cluster.second.writer->destroy();

    _map.clear();
}

/*
 * Find lost elements inside port
 *
 * Argument is the number of current cycle.
 * If some token couldn't be get in future, warnings
*/
void PortMap::clean_up( Cycle cycle)
{
    for ( const auto& cluster : _map)
        cluster.second.writer->clean_up( cycle);
}

BasicWritePort::BasicWritePort( std::string key, uint32 bandwidth, uint32 fanout) :
    Port( std::move( key)), _bandwidth(bandwidth), _fanout(fanout)
{
    if ( portMap[ _key].writer != nullptr)
        serr << "Reusing of " << key
             << " key for WritePort. Last WritePort will be used." << std::endl;

    portMap[ _key].writer = this;
}

void BasicWritePort::check_init( const std::vector<Port*>& readers) const
{
    if ( readers.size() == 0)
        throw PortError( _key + " has no ReadPorts");
    if ( readers.size() > _fanout)
        throw PortError( _key + " WritePort is overloaded by fanout");
    if ( readers.size() != _fanout)
        throw PortError( _key + " WritePort is underloaded by fanout");
}

void BasicWritePort::prepare_to_write( Cycle cycle)
{
    if ( !_init)
        throw PortError(_key + " WritePort was not initializated");

    if ( _lastCycle != cycle)
        _writeCounter = 0;

    _lastCycle = cycle;

    if ( _writeCounter >= _bandwidth)
        throw PortError(_key + " port is overloaded by bandwidth");

    // If we can add something more on that cycle, forwarding it to all ReadPorts.
    _writeCounter++;
}
