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

void Root::enable_logging( const std::string& values)
{
    std::unordered_set<std::string> tokens;
    boost::split( tokens, values, boost::is_any_of( std::string(",")));
    enable_logging_impl( tokens);
}
