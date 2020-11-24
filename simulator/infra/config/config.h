/*
 * config.h - class for analysing and handling of inputed arguments
 * Copyright 2017-2018 MIPT-MIPS
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <infra/exception.h>
#include <infra/types.h>

#include <iosfwd>
#include <map>
#include <string>

namespace popl {
    class OptionParser;
} // namespace popl

namespace config {

// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
void handleArgs( int argc, const char* const argv[], int start_index);
std::string help();

enum class Type {
    OPTIONAL,
    REQUIRED,
    SWITCH,
    PREDICATED
};

template<typename T> using Predicate = bool(*)(T);

template<typename T>
static inline Predicate<T> get_null_predicate()
{
    return [](T /* ignore */){ return true; };
}

template<typename T, Type type>
class BaseTValue
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
        return rhs.dump( out);
    }

protected:
    BaseTValue( std::string_view alias,
                std::string_view name,
                std::string_view desc,
                const T& default_value,
                Predicate<T> predicate) noexcept;

private:
    std::ostream& dump( std::ostream& out) const;

    T value = T();
};
    
template<typename T>
struct AliasedRequiredValue : BaseTValue<T, Type::REQUIRED>
{
    AliasedRequiredValue( std::string_view alias, std::string_view name, std::string_view desc) noexcept
        : BaseTValue<T, Type::REQUIRED>( alias, name, desc, T(), get_null_predicate<T>())
    { }
};

template<typename T>
struct AliasedValue : BaseTValue<T, Type::OPTIONAL>
{
    AliasedValue( std::string_view alias, std::string_view name, const T& val, std::string_view desc) noexcept
        : BaseTValue<T, Type::OPTIONAL>( alias, name, desc, val, get_null_predicate<T>())
    { }
};

template<>
struct AliasedValue<std::string> : BaseTValue<std::string, Type::OPTIONAL>
{
    AliasedValue( std::string_view alias, std::string_view name, std::string_view val, std::string_view desc) noexcept
        : BaseTValue<std::string, Type::OPTIONAL>( alias, name, desc, std::string( val), get_null_predicate<std::string>())
    { }
};

struct AliasedSwitch : BaseTValue<bool, Type::SWITCH>
{
    AliasedSwitch( std::string_view alias, std::string_view name, std::string_view desc) noexcept
        : BaseTValue<bool, Type::SWITCH>( alias, name, desc, false, get_null_predicate<bool>())
    { }
};

template<typename T>
struct Value : AliasedValue<T>
{
    Value( std::string_view name, const T& val, std::string_view desc) noexcept : AliasedValue<T>( "", name, val, desc) { }
};

template<typename T>
struct PredicatedValue : BaseTValue<T, Type::PREDICATED>
{
    PredicatedValue( std::string_view name, const T& val, std::string_view desc, Predicate<T> predicate) noexcept
        : BaseTValue<T, Type::PREDICATED>( "", name, desc, val, predicate)
    { }
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

} // namespace config

#endif  // CONFIG_H

