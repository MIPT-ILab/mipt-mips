/*
 * config.cpp - implementation of Config class
 * Copyright 2017-2018 MIPT-MIPS
 */

#include "config.h"
#include <popl.hpp>
#include <sstream>

namespace config {

static AliasedSwitch help_option = { "h", "help", "print help"};

// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
void handleArgs( int argc, const char* const argv[], int start_index) try
{
    BaseValue::options().parse( argc, argv, start_index);

    if ( help_option)
        throw HelpOption();
}
catch ( const popl::invalid_option& e) {
    throw InvalidOption( e.what());
}

std::string help()
{
    return BaseValue::options().help();
}

popl::OptionParser& BaseValue::options()
{
    static popl::OptionParser instance( "Allowed options");
    return instance;
}

template<typename T, Type type>
static void add_option( popl::OptionParser* options, std::string_view alias, std::string_view name, std::string_view desc, const T& default_value, T* value) noexcept try
{
    (void)default_value;
    if constexpr ( type == Type::SWITCH)
        options->add<popl::Switch>( std::string( alias), std::string( name), std::string( desc), value);
    else if constexpr ( type == Type::OPTIONAL)
        options->add<popl::Value<T>>( std::string( alias), std::string( name), std::string( desc), default_value, value);
    else
        options->add<popl::Value<T>, popl::Attribute::required>( std::string( alias), std::string( name), std::string( desc), default_value, value);
}
catch ( const std::runtime_error& e)
{
    std::cerr << "Bad option setup for '" << name << "' (" << e.what() << ")" << std::endl;
    std::terminate();
}
catch ( ...)
{
    std::cerr << "Bad option setup for '" << name << "' ( unknown exception )" << std::endl;
    std::terminate();
}   

template<typename T, Type type>
BaseTValue<T, type>::BaseTValue( std::string_view alias, std::string_view name, std::string_view desc, const T& default_value) noexcept
{
    // Workaround for Visual Studio bug
    // https://developercommunity.visualstudio.com/content/problem/846216/false-positive-c4297-for-constructor.html
    add_option<T, type>( &options(), alias, name, desc, default_value, &value);
}

template class BaseTValue<std::string, Type::OPTIONAL>;
template class BaseTValue<std::string, Type::REQUIRED>;
template class BaseTValue<uint32, Type::OPTIONAL>;
template class BaseTValue<uint32, Type::REQUIRED>;
template class BaseTValue<uint64, Type::OPTIONAL>;
template class BaseTValue<uint64, Type::REQUIRED>;
template class BaseTValue<bool, Type::SWITCH>;

} // namespace config
