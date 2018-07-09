/*
 * config.h - class for analysing and handling of inputed arguments
 * Copyright 2017-2018 MIPT-MIPS
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <cstdlib>
#include <iostream>
#include <string>
#include <map>

#include <infra/types.h>

#include <popl.hpp>

namespace config {

class BaseValue
{
    friend void handleArgs( int argc, const char* argv[]);
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
    
    friend std::ostream& operator<<( std::ostream& out, const BaseTValue& rhs)
    {
        return out << rhs.value;
    }
};
    
template<typename T>
struct AliasedRequiredValue : public BaseTValue<T> {
    AliasedRequiredValue<T>( const std::string& alias, const std::string& name, const std::string& desc) noexcept
        : BaseTValue<T>( )
    {
        this->options().template add<popl::Value<T>, popl::Attribute::required>(alias, name, desc, T(), &this->value);
    }

    AliasedRequiredValue<T>() = delete;
};

template<typename T>
struct AliasedValue : public BaseTValue<T> {
    AliasedValue<T>( const std::string& alias, const std::string& name, const T& val, const std::string& desc) noexcept
        : BaseTValue<T>( val)
    {
        this->options().template add<popl::Value<T>>(alias, name, desc, val, &this->value);
    }

    AliasedValue<T>() = delete;
};

struct AliasedSwitch : public BaseTValue<bool> {
    AliasedSwitch( const std::string& alias, const std::string& name, const std::string& desc) noexcept
        : BaseTValue<bool>( false)
    {
        options().add<popl::Switch>(alias, name, desc, &this->value);
    }

    AliasedSwitch() = delete;
};

template<typename T>
struct Unaliased : public T
{
    Unaliased<T>() = delete;

    template<typename Arg1, typename Arg2, typename ... Args>
    Unaliased(Arg1&& arg1, Arg2&& arg2, Args&& ... args) noexcept
        : T( "", arg1, arg2, args...)
    { }
};

template<typename T> using Value = Unaliased<AliasedValue<T>>;
template<typename T> using RequiredValue = Unaliased<AliasedRequiredValue<T>>;
using Switch = Unaliased<AliasedSwitch>;

/* methods */
void handleArgs( int argc, const char* argv[]);

} // namespace config

#endif  // CONFIG_H

