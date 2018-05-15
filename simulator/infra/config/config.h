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

/* POPL */
#include "popl.hpp"

namespace config {

class BaseValue
{
    friend void handleArgs( int argc, const char* argv[]);
protected:
    const std::string alias;
    const std::string name;
    const std::string desc;

    BaseValue( const char* alias, const char* name, const char* desc)
        : alias( alias), name( name), desc( desc) { }
    virtual ~BaseValue() = default;

    static popl::OptionParser& values() {
        static popl::OptionParser instance( "Allowed options");
        return instance;
    }
public:
    // Do not move or copy
    BaseValue( const BaseValue&) = delete;
    BaseValue( BaseValue&&) = delete;
    BaseValue& operator=( const BaseValue&) = delete;
    BaseValue& operator=( BaseValue&&) = delete;
};

template<typename T>
class AliasedRequiredValue : public BaseValue {
protected:
    T value;
    AliasedRequiredValue<T>( const char* alias, const char* name, const char* desc, int) noexcept
        : BaseValue( alias, name, desc)
        , value( ) { }
public:
    AliasedRequiredValue<T>( const char* alias, const char* name, const char* desc) noexcept
        : AliasedRequiredValue<T>( alias, name, desc, 1)
    {
        this->values().template add<popl::Value<T>, popl::Attribute::required>(alias, name, desc, T(), &value);
    }

    AliasedRequiredValue<T>() = delete;

    // Converter is implicit intentionally, so bypass Clang-Tidy check
    // NOLINTNEXTLINE(hicpp-explicit-conversions, google-explicit-constructor)
    operator const T&() const { return value; }
    
    friend std::ostream& operator<<( std::ostream& out, const AliasedRequiredValue& rhs)
    {
        return out << rhs.value;
    }
};

template<typename T>
class AliasedValue : public AliasedRequiredValue<T> {
protected:
    using Base = AliasedRequiredValue<T>;
    using Base::values;
    AliasedValue<T>( const char* alias, const char* name, const T& val, const char* desc, int x) noexcept
        : AliasedRequiredValue<T>( alias, name, desc, x)
        , default_value( val)
    {
         this->value = val;
    }
public:
    AliasedValue<T>( const char* alias, const char* name, const T& val, const char* desc) noexcept
        : AliasedValue<T>( alias, name, val, desc, 1)
    {
        popl::OptionParser& options = values();
        options.add<popl::Value<T>>(this->alias, this->name, this->desc, default_value, &this->value);
    }

    AliasedValue<T>() = delete;
private:
    const T default_value;
};

class AliasedSwitch : public AliasedValue<bool> {
public:
    AliasedSwitch( const char* alias, const char* name, const char* desc) noexcept
        : AliasedValue<bool>( alias, name, false, desc, 1)
    {
        values().add<popl::Switch>(this->alias, this->name, this->desc, &this->value);
    }

    AliasedSwitch() = delete;
};

template<typename T>
class Unaliased : public T
{
public:
    Unaliased<T>() = delete;
    template<typename ... Args> Unaliased(Args&& ... args) noexcept : T( "", args...)  { }
};

template<typename T> using Value = Unaliased<AliasedValue<T>>;
template<typename T> using RequiredValue = Unaliased<AliasedRequiredValue<T>>;
using Switch = Unaliased<AliasedSwitch>;

/* methods */
void handleArgs( int argc, const char* argv[]);

} // namespace config

#endif  // CONFIG_H

