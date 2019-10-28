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

void Module::enable_dumping_impl(pt::ptree& topology) {
    pt::ptree topology_module;
    topology_module.put("module_name", name);
    topology_module.add_child("write_ports", topology_write_ports);
    topology_module.add_child("read_ports", topology_read_ports);
    topology.push_back(std::make_pair("", topology_module));
    for ( const auto& c : children)
        c->enable_dumping_impl(topology);
}

void Root::enable_logging( const std::string& values)
{
    std::unordered_set<std::string> tokens;
    boost::split( tokens, values, boost::is_any_of( std::string(",")));
    enable_logging_impl( tokens);
}

void Root::enable_dumping(bool dump)
{
    if (dump) {
        pt::ptree topology;
        enable_dumping_impl(topology);
        pt::ptree result;
        result.add_child("topology", topology);
        pt::write_json("topology.json", result);
        std::cout << std::endl << "Topology dumped in topology.json" << std::endl;
    }
}