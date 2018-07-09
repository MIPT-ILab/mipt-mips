/*
 * simulator.cpp - interface for simulator
 * Copyright 2018 MIPT-MIPS
 */
 
// Simulators
#include <func_sim/func_sim.h>
#include <modules/core/perf_sim.h>

// ISAs
#include <mips/mips.h>
#include <risc_v/risc_v.h>

#include "simulator.h"

class SimulatorFactory {
    struct Builder {
        virtual std::unique_ptr<Simulator> get_funcsim( bool log) = 0;
        virtual std::unique_ptr<Simulator> get_perfsim( bool log) = 0;
        Builder() = default;
        virtual ~Builder() = default;
        Builder( const Builder&) = delete;
        Builder( Builder&&) = delete;
        Builder& operator=( const Builder&) = delete;
        Builder& operator=( Builder&&) = delete;
    };

    template<typename T>
    struct TBuilder : public Builder {
        TBuilder() = default;
        std::unique_ptr<Simulator> get_funcsim( bool log) final { return std::make_unique<FuncSim<T>>( log); }
        std::unique_ptr<Simulator> get_perfsim( bool log) final { return std::make_unique<PerfSim<T>>( log); }
    };

    using Map = std::map<std::string, std::unique_ptr<Builder>>;
    const Map map;

    // Use old-fashioned generation since initializer-lists don't work with unique_ptrs
    static Map generate_map() {
        Map my_map;
        my_map.emplace("mips1",  std::make_unique<TBuilder<MIPSI>>());
        my_map.emplace("mips2",  std::make_unique<TBuilder<MIPSII>>());
        my_map.emplace("mips3",  std::make_unique<TBuilder<MIPSIII>>());
        my_map.emplace("mips4",  std::make_unique<TBuilder<MIPSIV>>());
        my_map.emplace("mips32", std::make_unique<TBuilder<MIPS32>>());
        my_map.emplace("mips64", std::make_unique<TBuilder<MIPS64>>());
        my_map.emplace("riscv32", std::make_unique<TBuilder<RISCV32>>());
        my_map.emplace("riscv64", std::make_unique<TBuilder<RISCV64>>());
        my_map.emplace("riscv128", std::make_unique<TBuilder<RISCV128>>());
        return my_map;
    }

    auto get_factory( const std::string& name) const 
    {
        auto it = map.find( name);
        if ( it == map.end())
        {
             std::cerr << "ERROR. Invalid ISA option " << name << std::endl
                       << "Supported ISAs:" << std::endl;
             for ( const auto& map_name : map)
                 std::cerr << "\t" << map_name.first << std::endl;

             std::exit( EXIT_FAILURE);
        }

        return it->second.get();
    }

public:
    SimulatorFactory() : map( generate_map()) { }

    auto get_funcsim( const std::string& name, bool log) const
    {
        return get_factory( name)->get_funcsim( log);
    }

    auto get_perfsim( const std::string& name, bool log) const
    {
        return get_factory( name)->get_perfsim( log);
    }
};
    
std::unique_ptr<Simulator>
Simulator::create_simulator( const std::string& isa, bool functional_only, bool log)
{
    static const SimulatorFactory factory;
    if ( functional_only)
        return factory.get_funcsim( isa, log);

    return factory.get_perfsim( isa, log);
}

