/*
 * simulator.cpp - interface for simulator
 * Copyright 2018 MIPT-MIPS
 */

// Configurations
#include <infra/config/config.h>
#include <infra/exception.h>
 
// Simulators
#include <func_sim/func_sim.h>
#include <modules/core/perf_sim.h>

// ISAs
#include <mips/mips.h>
#include <risc_v/risc_v.h>

#include "simulator.h"

namespace config {
    static AliasedValue<std::string> isa = { "I", "isa", "mars", "modeled ISA"};
    static AliasedSwitch disassembly_on = { "d", "disassembly", "print disassembly"};
    static AliasedSwitch functional_only = { "f", "functional-only", "run functional simulation only"};
} // namespace config

struct InvalidISA final : Exception
{
    explicit InvalidISA(const std::string& isa)
        : Exception("Invalid ISA", isa)
    { }
};

class SimulatorFactory {
    struct Builder {
        virtual std::unique_ptr<Simulator> get_funcsim( bool log) = 0;
        virtual std::unique_ptr<CycleAccurateSimulator> get_perfsim( bool log) = 0;
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
        std::unique_ptr<CycleAccurateSimulator> get_perfsim( bool log) final { return std::make_unique<PerfSim<T>>( log); }
    };

    using Map = std::map<std::string, std::unique_ptr<Builder>>;
    const Map map;

    // Use old-fashioned generation since initializer-lists don't work with unique_ptrs
    static Map generate_map() {
        Map my_map;
        my_map.emplace("mipsI",  std::make_unique<TBuilder<MIPSI>>());
        my_map.emplace("mipsII",  std::make_unique<TBuilder<MIPSII>>());
        my_map.emplace("mipsIII",  std::make_unique<TBuilder<MIPSIII>>());
        my_map.emplace("mipsIV",  std::make_unique<TBuilder<MIPSIV>>());
        my_map.emplace("mips32", std::make_unique<TBuilder<MIPS32>>());
        my_map.emplace("mips64", std::make_unique<TBuilder<MIPS64>>());
        my_map.emplace("mars",   std::make_unique<TBuilder<MARS>>());
        my_map.emplace("mars64", std::make_unique<TBuilder<MARS64>>());
        my_map.emplace("mipsIel",  std::make_unique<TBuilder<MIPSI_LE>>());
        my_map.emplace("mipsIIel",  std::make_unique<TBuilder<MIPSII_LE>>());
        my_map.emplace("mipsIIIel",  std::make_unique<TBuilder<MIPSIII_LE>>());
        my_map.emplace("mipsIVel",  std::make_unique<TBuilder<MIPSIV_LE>>());
        my_map.emplace("mips32el", std::make_unique<TBuilder<MIPS32_LE>>());
        my_map.emplace("mips64el", std::make_unique<TBuilder<MIPS64_LE>>());
        my_map.emplace("marseb",   std::make_unique<TBuilder<MARS_BE>>());
        my_map.emplace("mars64eb", std::make_unique<TBuilder<MARS64_BE>>());
        my_map.emplace("mipsIeb",  std::make_unique<TBuilder<MIPSI_BE>>());
        my_map.emplace("mipsIIeb",  std::make_unique<TBuilder<MIPSII_BE>>());
        my_map.emplace("mipsIIIeb",  std::make_unique<TBuilder<MIPSIII_BE>>());
        my_map.emplace("mipsIVeb",  std::make_unique<TBuilder<MIPSIV_BE>>());
        my_map.emplace("mips32eb", std::make_unique<TBuilder<MIPS32_BE>>());
        my_map.emplace("mips64eb", std::make_unique<TBuilder<MIPS64_BE>>());
        my_map.emplace("marseb",   std::make_unique<TBuilder<MARS_BE>>());
        my_map.emplace("mars64eb", std::make_unique<TBuilder<MARS64_BE>>());
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
            std::cout << "Supported ISAs:" << std::endl;
            for ( const auto& map_name : map)
                std::cout << "\t" << map_name.first << std::endl;

            throw InvalidISA( name);
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
    
    static SimulatorFactory& get_instance() {
        static SimulatorFactory sf;
        return sf;
    }
};
    
std::shared_ptr<Simulator>
Simulator::create_simulator( const std::string& isa, bool functional_only, bool log)
{
    if ( functional_only)
        return SimulatorFactory::get_instance().get_funcsim( isa, log);

    return CycleAccurateSimulator::create_simulator( isa, log);
}

std::shared_ptr<Simulator>
Simulator::create_configured_simulator()
{
    return create_simulator( config::isa, config::functional_only, config::disassembly_on);
}

std::shared_ptr<Simulator>
Simulator::create_configured_isa_simulator( const std::string& isa)
{
    return create_simulator( isa, config::functional_only, config::disassembly_on);
}

std::shared_ptr<CycleAccurateSimulator>
CycleAccurateSimulator::create_simulator( const std::string& isa, bool log)
{
    return SimulatorFactory::get_instance().get_perfsim( isa, log);
}

