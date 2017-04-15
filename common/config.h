/*
 * config.h - class for analysing and handling of inputed arguments
 * Copyright 2017 MIPT-MIPS
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <list>

#include <common/types.h>

/* boost - program option parsing */
#include <boost/program_options.hpp>

using bod = boost::program_options::options_description;

class Config
{
    class BasicValue {
        friend class Config;
        virtual void reg( bod& d) = 0;
    public:
        BasicValue(Config*);
    };

    template<typename T>
    class Value : public BasicValue {
        const std::string name;
        const std::string desc;
        const T default_value;
        const bool is_required;

        T value;

        void reg( bod&) final;
        Value<T>() = delete;
    public:
        Value<T>( Config* c, const char* name, const T& val, const char* desc, bool is_req = false)
            : BasicValue( c)
            , name( name)
            , desc( desc)
            , default_value( val)
            , is_required( is_req)
        { }

        operator const T&() const { return value; }
    };

    friend class BasicValue;
    std::list<BasicValue*> values;
public:

    /* constructors */
    Config()  { }
    ~Config() { }

    /* methods */
    int handleArgs( int argc, char** argv);
};


template<>
void Config::Value<bool>::reg(bod& d);

template<typename T>
void Config::Value<T>::reg(bod& d)
{
    namespace po = boost::program_options;
    if (is_required)
    {
        d.add_options()(name.c_str(),
                    po::value<T>( &value)->default_value( default_value)->required(),
                    desc.c_str());
    }
    else {
        d.add_options()(name.c_str(),
                    po::value<T>( &value)->default_value( default_value),
                    desc.c_str());
    }
}

#endif  // CONFIG_H
