/**
 * module.cpp - module template
 * @author Pavel Kryukov
 * Copyright 2019 MIPT-V team
 */

#include "module.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

Module::Module( Module* parent, std::string name)
    : parent( parent), name( std::move( name))
{
    if ( parent != nullptr)
        parent->add_child( this);
}

void Module::force_enable_logging()
{  
    sout.enable();
    for (const auto& c : children)
        c->force_enable_logging();
}

void Module::force_disable_logging()
{  
    sout.disable();
    for (const auto& c : children)
        c->force_disable_logging();
}

void Module::enable_logging_impl( const std::unordered_set<std::string>& names)
{
    if ( names.count( name) != 0)
        force_enable_logging();
    else if ( names.count( '!' + name) != 0)
        force_disable_logging();;

    for ( const auto& c : children)
        c->enable_logging_impl( names);
}

void Module::module_dumping( pt::ptree& pt_modules) const
{
    pt::ptree pt_module;
    pt_module.add_child( "write_ports", topology_write_ports);
    pt_module.add_child( "read_ports", topology_read_ports);
    pt_modules.add_child( name, pt_module);
    for ( const auto& c : children)
        c->module_dumping( pt_modules);
}

void Module::modulemap_dumping_impl( pt::ptree& pt_modulemap) const
{
    pt::ptree pt_c_modulemap;
    for ( const auto& c : children) {
        c->modulemap_dumping_impl( pt_c_modulemap);
    }
    pt_modulemap.add_child( name, pt_c_modulemap);
}

void Root::enable_logging( const std::string& values)
{
    std::unordered_set<std::string> tokens;
    boost::split( tokens, values, boost::is_any_of( std::string(",")));
    enable_logging_impl( tokens);
}

void Root::portmap_dumping( pt::ptree& pt_portmap) const
{
    for ( const auto& elem : portmap->map) {
        pt::ptree pt_cluster;
        pt::ptree pt_write_port;
        pt::ptree pt_read_ports;
        pt_write_port.put( "fanout", elem.second.writer->get_fanout());
        pt_write_port.put( "bandwidth", elem.second.writer->get_bandwidth());
        for ( const auto& read_port : elem.second.readers) {
            pt_read_ports.put( "latency", read_port->get_latency());
        }
        pt_cluster.add_child( "write_port", pt_write_port);
        pt_cluster.add_child( "read_ports", pt_read_ports);
        pt_portmap.add_child( elem.first, pt_cluster);
    }
}

void Root::modulemap_dumping( pt::ptree& pt_modulemap) const
{
    modulemap_dumping_impl( pt_modulemap);
}

void Root::topology_dumping_impl( pt::ptree& pt_topology) const
{
    pt::ptree pt_modules;
    pt::ptree pt_portmap;
    pt::ptree pt_modulemap;

    module_dumping( pt_modules);
    portmap_dumping( pt_portmap);
    modulemap_dumping( pt_modulemap);

    pt_topology.add_child( "modules", pt_modules);
    pt_topology.add_child( "portmap", pt_portmap);
    pt_topology.add_child( "modulemap", pt_modulemap);
}

void Root::topology_dumping( bool dump, const std::string& filename) const
{
    pt::ptree pt_topology;
    if ( dump) {
        topology_dumping_impl( pt_topology);
        try {
            if ( filename.empty()) {
                std::cout << "*************Module topology dump***************" << std::endl;
                pt::write_json( std::cout, pt_topology);
                std::cout << "************************************************" << std::endl;
            }
            else {
                pt::write_json( filename, pt_topology);
                std::cout << std::endl << "Module topology dumped in topology.json" << std::endl;
            }
        }
        catch( pt::json_parser_error& e) {
            std::cerr << e.what();
            throw e;
        }
    }
}