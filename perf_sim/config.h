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
        static std::list<BasicValue*> values;
        virtual void reg( bod& d) = 0;
    public:
        static std::list<BasicValue*>& get_values() { return values; }
        BasicValue() { values.push_back( this); }
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
        Value<T>( const char* name, const T& val, const char* desc, bool is_req = false)
            : BasicValue()
            , name( name)
            , desc( desc)
            , default_value( val)
            , is_required( is_req)
        { }

        operator const T&() const { return value; }
    };

public:
    /* variables */
    Value<std::string> binary_filename = { "binary,b", "", "input binary file", true};
    Value<uint64>      num_steps = { "numsteps,n", 1, "number of instructions to run", true};
    Value<bool>        disassembly_on = { "disassembly,d", false, "print disassembly"};
    Value<bool>        functional_only = { "functional-only,f", false, "run functional simulation only"};

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
