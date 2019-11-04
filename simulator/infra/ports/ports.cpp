/**
 * ports.cpp - template for simulation of ports.
 * @author Pavel Kryukov
 * Copyright 2017-2019 MIPT-MIPS team
 */

#include "ports.h"

#include <boost/property_tree/ptree.hpp>

namespace pt = boost::property_tree;

std::shared_ptr<PortMap> PortMap::create_port_map()
{
    struct PortMapHack : public PortMap {};
    return std::make_shared<PortMapHack>();
}

void PortMap::init() const
{
    for ( const auto& cluster : map)
    {
        if ( cluster.second.writer == nullptr)
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

pt::ptree PortMap::dump() const
{
    pt::ptree portmap;
    for ( const auto& elem : map) {
        pt::ptree cluster;
        pt::ptree write_port;
        pt::ptree read_ports;
        write_port.put( "fanout", elem.second.writer->get_fanout());
        write_port.put( "bandwidth", elem.second.writer->get_bandwidth());
        for ( const auto& read_port : elem.second.readers) {
            read_ports.put( "latency", read_port->get_latency());
        }
        cluster.add_child( "write_port", write_port);
        cluster.add_child( "read_ports", read_ports);
        portmap.add_child( elem.first, cluster);
    }
    return portmap;
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
