/**
 * ports.cpp - template for simulation of ports.
 * @author Pavel Kryukov
 * Copyright 2017-2019 MIPT-MIPS team
 */

#include "ports.h"

PortMap& PortMap::get_instance()
{
    static PortMap instance;
    return instance;
}

PortMap::PortMap() noexcept : Log( false) { }

void PortMap::init() const
{
    for ( const auto& cluster : map)
    {
        if (cluster.second.writer == nullptr)
            throw PortError( cluster.first + " has no WritePort");

        cluster.second.writer->init( cluster.second.readers);
    }
}

void PortMap::clean_up( Cycle cycle)
{
    for ( const auto& cluster : map)
        cluster.second.writer->clean_up( cycle);
}

void PortMap::add_port( BasicWritePort* port)
{
    if ( map[ port->get_key()].writer != nullptr)
        serr << "Reusing of " << port->get_key()
             << " key for WritePort. Last WritePort will be used." << std::endl;

    map[ port->get_key()].writer = port;
}

void PortMap::add_port( BasicReadPort* port)
{
    map[ port->get_key()].readers.push_back( port);
}

void PortMap::destroy()
{
    map.clear();
}

Port::Port( std::string key) : Log( false), _key( std::move( key)) { }

BasicReadPort::BasicReadPort( const std::string& key, Latency latency)
    : Port( key), _latency( latency)
{
    portMap.add_port( this);
}


BasicWritePort::BasicWritePort( const std::string& key, uint32 bandwidth, uint32 fanout) :
    Port( key), _fanout(fanout), installed_bandwidth(bandwidth)
{
    portMap.add_port( this);
}

void BasicWritePort::base_init( const std::vector<BasicReadPort*>& readers)
{
    if ( readers.empty())
        throw PortError( get_key() + " has no ReadPorts");
    if ( readers.size() > _fanout)
        throw PortError( get_key() + " WritePort is overloaded by fanout");
    if ( readers.size() != _fanout)
        throw PortError( get_key() + " WritePort is underloaded by fanout");

    initialized_bandwidth = installed_bandwidth;
}
