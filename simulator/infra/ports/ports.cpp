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

std::shared_ptr<PortMap> PortMap::instance = nullptr;

std::shared_ptr<PortMap> PortMap::get_instance()
{
    if ( instance == nullptr)
        reset_instance();

    return instance;
}

void PortMap::reset_instance()
{
    instance = create_port_map();
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
        throw PortError( get_key() + " has two WritePorts");

    map[ port->get_key()].writer = port;
}

void PortMap::add_port( BasicReadPort* port)
{
    map[ port->get_key()].readers.push_back( port);
}

Port::Port( std::shared_ptr<PortMap> port_map, std::string key)
    : Log( false), pm( std::move( port_map)), k( std::move( key))
{ }

BasicReadPort::BasicReadPort( const std::shared_ptr<PortMap>& port_map, const std::string& key, Latency latency)
    : Port( port_map, key), _latency( latency)
{
    get_port_map()->add_port( this);
}

BasicWritePort::BasicWritePort( const std::shared_ptr<PortMap>& port_map, const std::string& key, uint32 bandwidth, uint32 fanout) :
    Port( port_map, key), _fanout(fanout), installed_bandwidth(bandwidth)
{
    get_port_map()->add_port( this);
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
    for (const auto& r : readers)
        r->init( initialized_bandwidth);
}
