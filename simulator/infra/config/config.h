/*
 * config.h - class for analysing and handling of inputed arguments
 * Copyright 2017 MIPT-MIPS
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <string>
#include <map>

#include <infra/types.h>

// small hack to include boost only in config.cpp
namespace boost { namespace program_options {
    class options_description;
} // namespace program_options 
} // namespace boost

namespace config {

using bod = boost::program_options::options_description;

class BaseValue
{
    friend void handleArgs( int argc, char** argv);
    virtual void reg( bod& d) = 0;

    static std::map<std::string, BaseValue*>& values() {
        static std::map<std::string, BaseValue*> instance;
        return instance;
    }

protected:
    const std::string name;
    const std::string desc;

    BaseValue( const char* name, const char* desc) : name( name), desc( desc)
    {
        if (values().find( name) != values().end()) {
            std::cerr << "ERROR. Duplicate option with the same name ("
                      << name << ")" << std::endl;
            std::exit( EXIT_FAILURE);
        }
        values()[name] = this;
    }
    virtual ~BaseValue() = default;
};

template<typename T>
class RequiredValue : public BaseValue {
protected:
    T value;
    
    void reg( bod& d) override;
    RequiredValue<T>() = delete;
public:
    RequiredValue<T>( const char* name, const char* desc) noexcept
        : BaseValue( name, desc)
        , value( )
    { }
    
    operator const T&() const { return value; }
    ~RequiredValue() override = default;
};

template<typename T>
class Value : public RequiredValue<T> {
    const T default_value;

    void reg( bod& d) override;
    Value<T>() = delete;
public:
    Value<T>( const char* name, const T& val, const char* desc) noexcept
        : RequiredValue<T>( name, desc)
        , default_value( val)
    { this->value = val; }

    ~Value() final = default;
};

/* methods */
void handleArgs( int argc, char** argv);

} // namespace config

#endif  // CONFIG_H

