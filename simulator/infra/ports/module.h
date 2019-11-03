/**
 * module.h - module template
 * @author Pavel Kryukov
 * Copyright 2019 MIPT-V team
 */

#ifndef INFRA_PORTS_MODULE_H
#define INFRA_PORTS_MODULE_H
 
#include <infra/log.h>
#include <infra/ports/ports.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <unordered_set>

namespace pt = boost::property_tree;

class Module : public Log
{
public:
    Module( Module* parent, std::string name);

protected:
    template<typename T>
    auto make_write_port( std::string key, uint32 bandwidth) 
    {
        topology_write_ports.put(key, "");
        return std::make_unique<WritePort<T>>( get_portmap(), std::move(key), bandwidth);
    }

    template<typename T>
    auto make_read_port( std::string key, Latency latency)
    {
        topology_read_ports.put(key, "");
        return std::make_unique<ReadPort<T>>( get_portmap(), std::move(key), latency);
    }

    void enable_logging_impl( const std::unordered_set<std::string>& names);

    void module_dumping( pt::ptree& pt_modules) const;
    void modulemap_dumping_impl( pt::ptree& pt_modulemap) const;

private:
    virtual std::shared_ptr<PortMap> get_portmap() const { return parent->get_portmap(); }
    void force_enable_logging();
    void force_disable_logging();

    void add_child( Module* module) { children.push_back( module); }

    Module* const parent;
    std::vector<Module*> children;
    const std::string name;
    pt::ptree topology_write_ports;
    pt::ptree topology_read_ports;
};

class Root : public Module
{
public:
    explicit Root( std::string name)
        : Module( nullptr, std::move( name))
        , portmap( PortMap::create_port_map())
    { }

protected:
    void init_portmap() { portmap->init(); }
    void enable_logging( const std::string& values);
    
    void topology_dumping( bool dump, const std::string& filename = "") const;
    void topology_dumping_impl( pt::ptree& pt_topology) const;
    void portmap_dumping( pt::ptree& pt_portmap) const;
    void modulemap_dumping( pt::ptree& pt_modulemap) const;

private:
    std::shared_ptr<PortMap> get_portmap() const final { return portmap; }
    std::shared_ptr<PortMap> portmap;
};

#endif
