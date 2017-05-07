/*
 * config.h - class for analysing and handling of inputed arguments
 * Copyright 2017 MIPT-MIPS
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <list>

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

    static std::list<BaseValue*>& values() {
        static std::list<BaseValue*> instance;
        return instance;
    }

protected:
    BaseValue() { values().push_back( this); }
    virtual ~BaseValue() = default;
};

template<typename T>
class Value : public BaseValue {
    const std::string name;
    const std::string desc;
    const T default_value;
    const bool is_required;

    T value;

    void reg( bod& d) final;
    Value<T>() = delete;
public:
    Value<T>( const char* name, const T& val, const char* desc, bool is_req = false) noexcept
        : BaseValue( )
        , name( name)
        , desc( desc)
        , default_value( val)
        , is_required( is_req)
        , value( val)
    { }

    operator const T&() const { return value; }
    ~Value() final = default;
};

/* methods */
void handleArgs( int argc, char** argv);

} // namespace config

#endif  // CONFIG_H

