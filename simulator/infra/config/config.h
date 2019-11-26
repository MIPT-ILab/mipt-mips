/*
 * config.h - class for analysing and handling of inputed arguments
 * Copyright 2017-2018 MIPT-MIPS
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <infra/exception.h>
#include <infra/types.h>

#include <iostream>
#include <map>
#include <string>

namespace popl {
    class OptionParser;
} // namespace popl

namespace config {

class BaseValue
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
    friend void handleArgs( int argc, const char* const argv[], int start_index);
    friend std::string help();
protected:
    static popl::OptionParser& options();
    static void register_switch( const std::string& alias, const std::string& name, const std::string& desc, bool* value);
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
        if constexpr (std::is_same<T, bool>())
            out << std::boolalpha << rhs.value << std::noboolalpha; // NOLINT(bugprone-suspicious-semicolon)
        else
            out << std::dec << rhs.value;
        return out;
    }
};
    
template<typename T>
struct AliasedRequiredValue : BaseTValue<T>
{
    AliasedRequiredValue<T>( const std::string& alias, const std::string& name, const std::string& desc);
};

template<typename T>
struct AliasedValue : BaseTValue<T>
{
    AliasedValue<T>( const std::string& alias, const std::string& name, const T& val, const std::string& desc);
};

struct AliasedSwitch : BaseTValue<bool>
{
    AliasedSwitch( const std::string& alias, const std::string& name, const std::string& desc);
};

template<typename T>
struct Value : AliasedValue<T>
{
    Value( const std::string& name, const T& val, const std::string& desc) : AliasedValue<T>( "", name, val, desc) { }
};

template<typename T>
struct RequiredValue : AliasedRequiredValue<T>
{
    RequiredValue( const std::string& name, const std::string& desc) : AliasedRequiredValue<T>( "", name, desc) { }
};

struct Switch : AliasedSwitch
{
    Switch( const std::string& name, const std::string& desc) : AliasedSwitch( "", name, desc) { }
};

struct HelpOption : Exception
{
    HelpOption() : Exception( "Help") {}
};

struct InvalidOption : Exception
{
    explicit InvalidOption( const std::string& msg) : Exception( "Invalid option", msg) {}
};

// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
void handleArgs( int argc, const char* const argv[], int start_index);
std::string help();

} // namespace config

#endif  // CONFIG_H

