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

BasicWritePort::BasicWritePort( const std::string& key, uint32 bandwidth, uint32 fanout) :
    Port( key), _fanout(fanout), installed_bandwidth(bandwidth)
{
    if ( portMap[ _key].writer != nullptr)
        serr << "Reusing of " << key
             << " key for WritePort. Last WritePort will be used." << std::endl;

    portMap[ _key].writer = this;
}

void BasicWritePort::check_init( const std::vector<Port*>& readers) const
{
    if ( readers.empty())
        throw PortError( _key + " has no ReadPorts");
    if ( readers.size() > _fanout)
        throw PortError( _key + " WritePort is overloaded by fanout");
    if ( readers.size() != _fanout)
        throw PortError( _key + " WritePort is underloaded by fanout");
}
