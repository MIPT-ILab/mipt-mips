/*
 * config.h - class for analysing and handling of inputed arguments
 * Copyright 2017-2018 MIPT-MIPS
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <infra/exception.h>
#include <infra/types.h>

#include <popl.hpp>

#include <iostream>
#include <map>
#include <string>

namespace config {

class BaseValue
{
    friend void handleArgs( int argc, const char* const argv[], int start_index);
protected:
    static popl::OptionParser& options() {
        static popl::OptionParser instance( "Allowed options");
        return instance;
    }
};

template<typename T>
class BaseTValue : public BaseValue
{
protected:
    T value = T();
    explicit BaseTValue<T>( T val ) noexcept : value( std::move(val)) { }
    BaseTValue<T>( ) = default;
public:
    // Converter is implicit intentionally, so bypass Clang-Tidy check
    // NOLINTNEXTLINE(hicpp-explicit-conversions, google-explicit-constructor)
    operator const T&() const { return value; }
    bool operator==( const T& rhs) const { return value == rhs; }
    bool operator!=( const T& rhs) const { return !operator==(rhs); }
    
    friend std::ostream& operator<<( std::ostream& out, const BaseTValue& rhs)
    {
        if constexpr (std::is_same_v<T, bool>)
            return out << std::boolalpha << rhs.value << std::noboolalpha; // NOLINT(bugprone-suspicious-semicolon)

        return out << std::dec << rhs.value;
    }
};
    
template<typename T>
struct AliasedRequiredValue : BaseTValue<T>
{
    AliasedRequiredValue<T>( const std::string& alias, const std::string& name, const std::string& desc)
        : BaseTValue<T>( )
    {
        this->options().template add<popl::Value<T>, popl::Attribute::required>(alias, name, desc, T(), &this->value);
    }
};

template<typename T>
struct AliasedValue : BaseTValue<T>
{
    AliasedValue<T>( const std::string& alias, const std::string& name, const T& val, const std::string& desc)
        : BaseTValue<T>( val)
    {
        this->options().template add<popl::Value<T>>(alias, name, desc, val, &this->value);
    }
};

struct AliasedSwitch : BaseTValue<bool>
{
    AliasedSwitch( const std::string& alias, const std::string& name, const std::string& desc)
        : BaseTValue<bool>( false)
    {
        options().add<popl::Switch>(alias, name, desc, &this->value);
    }
};

template<typename T>
struct Value : AliasedValue<T>
{
    Value( const std::string& name, const T& val, const std::string& desc)
        : AliasedValue<T>( "", name, val, desc)
    { }
};

template<typename T>
struct RequiredValue : AliasedRequiredValue<T>
{
    RequiredValue( const std::string& name, const std::string& desc)
        : AliasedRequiredValue<T>( "", name, desc)
    { }
};

struct Switch : AliasedSwitch
{
    Switch( const std::string& name, const std::string& desc)
        : AliasedSwitch( "", name, desc)
    { }
};

struct HelpOption : Exception {
    explicit HelpOption( const std::string& msg) : Exception( "Help", msg) {}
};

/* methods */
void handleArgs( int argc, const char* const argv[], int start_index);

} // namespace config

#endif  // CONFIG_H

