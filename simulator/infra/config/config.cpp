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

template<typename T>
AliasedRequiredValue<T>::AliasedRequiredValue( const std::string& alias, const std::string& name, const std::string& desc)
    : BaseTValue<T>( T())
{
    BaseValue::options().template add<popl::Value<T>, popl::Attribute::required>(alias, name, desc, T(), &this->value);
}

template<typename T>
AliasedValue<T>::AliasedValue( const std::string& alias, const std::string& name, const T& val, const std::string& desc)
    : BaseTValue<T>( val)
{
    BaseValue::options().template add<popl::Value<T>>(alias, name, desc, val, &this->value);
}

AliasedSwitch::AliasedSwitch( const std::string& alias, const std::string& name, const std::string& desc)
    : BaseTValue<bool>( false)
{
    BaseValue::options().add<popl::Switch>(alias, name, desc, &this->value);
}

template struct AliasedRequiredValue<std::string>;
template struct AliasedValue<std::string>;

template struct AliasedRequiredValue<uint64>;
template struct AliasedValue<uint64>;

template struct AliasedRequiredValue<uint32>;
template struct AliasedValue<uint32>;

} // namespace config
