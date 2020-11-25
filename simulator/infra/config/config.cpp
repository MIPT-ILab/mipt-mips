/*
 * config.cpp - implementation of Config class
 * Copyright 2017-2018 MIPT-MIPS
 */

#include "config.h"
#include <popl.hpp>

#include <cassert>
#include <iostream>
#include <sstream>

namespace config {

static const AliasedSwitch help_option = { "h", "help", "print help"};

static popl::OptionParser& options()
{
    static popl::OptionParser instance( "Allowed options");
    return instance;
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
void handleArgs( int argc, const char* const argv[], int start_index) try
{
    options().parse( argc, argv, start_index);

    if ( help_option)
        throw HelpOption();
}
catch ( const popl::invalid_option& e)
{
    throw InvalidOption( e.what());
}

std::string help()
{
    return options().help();
}

template<typename T, Type type>
static void add_option( std::string_view alias, std::string_view name, std::string_view desc, const T& default_value, Predicate<T> predicate, T* value) noexcept try
{
    (void)default_value;
    (void)predicate;
    if constexpr ( type == Type::SWITCH)
        options().add<popl::Switch>( std::string( alias), std::string( name), std::string( desc), value);
    else if constexpr ( type == Type::OPTIONAL)
        options().add<popl::Value<T>>( std::string( alias), std::string( name), std::string( desc), default_value, value);
    else if constexpr ( type == Type::PREDICATED)
        options().add<popl::BoundedValue<T>>( std::string( alias), std::string( name), std::string( desc), predicate, default_value, value);
    else
        options().add<popl::Value<T>, popl::Attribute::required>( std::string( alias), std::string( name), std::string( desc), default_value, value);
}
catch ( const std::exception& e)
{
    std::cerr << "Bad option setup for '" << name << "' \n" << e.what() << std::endl;
}

template<typename T, Type type>
BaseTValue<T, type>::BaseTValue( std::string_view alias, std::string_view name, std::string_view desc, const T& default_value, Predicate<T> predicate) noexcept
{
    // Workaround for Visual Studio bug
    // https://developercommunity.visualstudio.com/content/problem/846216/false-positive-c4297-for-constructor.html
    add_option<T, type>( alias, name, desc, default_value, predicate, &value);
}

template<typename T, Type type>
std::ostream& BaseTValue<T, type>::dump( std::ostream& out) const
{
    if constexpr (std::is_same<T, bool>())
        out << std::boolalpha << value << std::noboolalpha;
    else
        out << std::dec << value;
    return out;
}

template class BaseTValue<std::string, Type::OPTIONAL>;
template class BaseTValue<std::string, Type::REQUIRED>;
template class BaseTValue<std::string, Type::PREDICATED>;
template class BaseTValue<uint32, Type::PREDICATED>;
template class BaseTValue<uint32, Type::OPTIONAL>;
template class BaseTValue<uint32, Type::REQUIRED>;
template class BaseTValue<uint64, Type::PREDICATED>;
template class BaseTValue<uint64, Type::OPTIONAL>;
template class BaseTValue<uint64, Type::REQUIRED>;
template class BaseTValue<bool, Type::SWITCH>;

} // namespace config
