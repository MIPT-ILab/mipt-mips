/**
 * ports.cpp - template for simulation of ports.
 * @author Pavel Kryukov
 * Copyright 2017-2019 MIPT-MIPS team
 */

#include "ports.h"

std::shared_ptr<PortMap> PortMap::create_port_map()
{
    struct PortMapHack : public PortMap {};
    return std::make_shared<PortMapHack>();
}

void PortMap::init() const
{
    for ( const auto& cluster : map)
    {
        if (cluster.second.writer == nullptr)
            throw PortError( cluster.first + " has no WritePort");

        cluster.second.writer->init( cluster.second.readers);
        for ( const auto& r : cluster.second.readers)
            r->init( cluster.second.writer->get_bandwidth());
    }
}

void PortMap::add_port( BasicWritePort* port)
{
    if ( map[ port->get_key()].writer != nullptr)
        throw PortError( port->get_key() + " has two WritePorts");

    map[ port->get_key()].writer = port;
}

void PortMap::add_port( BasicReadPort* port)
{
    map[ port->get_key()].readers.push_back( port);
}

Port::Port( std::shared_ptr<PortMap> port_map, std::string key)
    : pm( std::move( port_map)), k( std::move( key))
{ }

BasicReadPort::BasicReadPort( const std::shared_ptr<PortMap>& port_map, const std::string& key, Latency latency)
    : Port( port_map, key), _latency( latency)
{
    get_port_map()->add_port( this);
}

BasicWritePort::BasicWritePort( const std::shared_ptr<PortMap>& port_map, const std::string& key, uint32 bandwidth) :
    Port( port_map, key), installed_bandwidth(bandwidth)
{
    get_port_map()->add_port( this);
}

void BasicWritePort::base_init( const std::vector<BasicReadPort*>& readers)
{
    if ( readers.empty())
        throw PortError( get_key() + " has no ReadPorts");
    _fanout = readers.size();

    initialized_bandwidth = installed_bandwidth;
}
