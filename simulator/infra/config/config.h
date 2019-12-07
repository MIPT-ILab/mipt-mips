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
};

enum class Type {
    OPTIONAL,
    REQUIRED,
    SWITCH
};

template<typename T, Type type>
class BaseTValue : public BaseValue
{
public:
    BaseTValue() = delete;

    // Converter is implicit intentionally, so bypass Clang-Tidy check
    // NOLINTNEXTLINE(hicpp-explicit-conversions, google-explicit-constructor)
    operator const T&() const { return value; }
    bool operator==( const T& rhs) const { return value == rhs; }
    bool operator!=( const T& rhs) const { return !operator==(rhs); }
    
    friend std::ostream& operator<<( std::ostream& out, const BaseTValue& rhs)
    {
        if constexpr (std::is_same<T, bool>())
            out << std::boolalpha << rhs.value << std::noboolalpha;
        else
            out << std::dec << rhs.value;
        return out;
    }

protected:
    BaseTValue( std::string_view alias, std::string_view name, std::string_view desc, const T& default_value ) noexcept;

private:
    T value = T();
};
    
template<typename T>
struct AliasedRequiredValue : BaseTValue<T, Type::REQUIRED>
{
    AliasedRequiredValue( std::string_view alias, std::string_view name, std::string_view desc) noexcept
        : BaseTValue<T, Type::REQUIRED>( alias, name, desc, T())
    { }
};

template<typename T>
struct AliasedValue : BaseTValue<T, Type::OPTIONAL>
{
    AliasedValue( std::string_view alias, std::string_view name, const T& val, std::string_view desc) noexcept
        : BaseTValue<T, Type::OPTIONAL>( alias, name, desc, val)
    { }
};

template<>
struct AliasedValue<std::string> : BaseTValue<std::string, Type::OPTIONAL>
{
    AliasedValue( std::string_view alias, std::string_view name, std::string_view val, std::string_view desc) noexcept
        : BaseTValue<std::string, Type::OPTIONAL>( alias, name, desc, std::string( val))
    { }
};

struct AliasedSwitch : BaseTValue<bool, Type::SWITCH>
{
    AliasedSwitch( std::string_view alias, std::string_view name, std::string_view desc) noexcept
        : BaseTValue<bool, Type::SWITCH>( alias, name, desc, false)
    { }
};

template<typename T>
struct Value : AliasedValue<T>
{
    Value( std::string_view name, const T& val, std::string_view desc) noexcept : AliasedValue<T>( "", name, val, desc) { }
};

template<>
struct Value<std::string> : AliasedValue<std::string>
{
    Value( std::string_view name, std::string_view val, std::string_view desc) noexcept : AliasedValue<std::string>( "", name, val, desc) { }
};

template<typename T>
struct RequiredValue : AliasedRequiredValue<T>
{
    RequiredValue( std::string_view name, std::string_view desc) noexcept : AliasedRequiredValue<T>( "", name, desc) { }
};

struct Switch : AliasedSwitch
{
    Switch( std::string_view name, std::string_view desc) noexcept : AliasedSwitch( "", name, desc) { }
};

// Thrown if help option is given by user
struct HelpOption : Exception
{
    HelpOption() : Exception( "Help") {}
};

// Thrown if users options are invalid
struct InvalidOption : Exception
{
    explicit InvalidOption( const std::string& msg) : Exception( "Invalid option", msg) {}
};

// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
void handleArgs( int argc, const char* const argv[], int start_index);
std::string help();

} // namespace config

#endif  // CONFIG_H

