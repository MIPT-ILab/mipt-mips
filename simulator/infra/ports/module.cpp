/**
 * module.cpp - module template
 * @author Pavel Kryukov
 * Copyright 2019 MIPT-V team
 */



#include "module.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;

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

void Module::module_dumping( pt::ptree* modules) const
{
    pt::ptree module;
    pt::ptree write_ports;
    pt::ptree read_ports;
    for ( const auto& k : topology_read_ports)
        read_ports.put(k, "");
    for ( const auto& k : topology_write_ports)
        write_ports.put(k, "");
    module.add_child( "write_ports", write_ports);
    module.add_child( "read_ports", read_ports);
    modules->add_child( name, module);
    for ( const auto& c : children)
        c->module_dumping( modules);
}

void Module::modulemap_dumping( pt::ptree* modulemap) const
{
    pt::ptree c_modulemap;
    for ( const auto& c : children)
        c->modulemap_dumping( &c_modulemap);
    modulemap->add_child( name, c_modulemap);
}

void Root::enable_logging( const std::string& values)
{
    std::unordered_set<std::string> tokens;
    boost::split( tokens, values, boost::is_any_of( std::string(",")));
    enable_logging_impl( tokens);
}

void Root::portmap_dumping( pt::ptree* pt_portmap) const
{
    portmap->dump( pt_portmap);
}

void Root::topology_dumping_impl( pt::ptree* topology) const
{
    pt::ptree modules;
    pt::ptree pt_portmap;
    pt::ptree modulemap;

    module_dumping( &modules);
    portmap_dumping( &pt_portmap);
    modulemap_dumping( &modulemap);

    topology->add_child( "modules", modules);
    topology->add_child( "portmap", pt_portmap);
    topology->add_child( "modulemap", modulemap);
}

void Root::topology_dumping( bool dump, const std::string& filename)
{
    pt::ptree topology;
    if ( !dump)
        return;
    topology_dumping_impl( &topology);
    pt::write_json( filename, topology);
    sout.enable();
    sout << std::endl << "Module topology dumped in topology.json" << std::endl;
}
