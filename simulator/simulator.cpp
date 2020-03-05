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

void CPUModel::duplicate_all_registers_to( CPUModel* model) const
{
    auto max = model->max_cpu_register();
    for ( size_t i = 0; i < max; ++i)
        model->write_cpu_register( i, read_cpu_register( i));
}

class SimulatorFactory {
    struct Builder {
        virtual std::unique_ptr<Simulator> get_funcsim( bool log) = 0;
        virtual std::unique_ptr<CycleAccurateSimulator> get_perfsim() = 0;
        std::string_view isa;
        Builder() = default;
        virtual ~Builder() = default;
        Builder( const Builder&) = delete;
        Builder( Builder&&) = delete;
        Builder& operator=( const Builder&) = delete;
        Builder& operator=( Builder&&) = delete;
    };

    template<typename T, Endian e>
    struct TBuilder : public Builder {
        explicit TBuilder( std::string_view i) { isa = i; }
        std::unique_ptr<Simulator> get_funcsim( bool log) final { return std::make_unique<FuncSim<T>>( e, log, isa); }
        std::unique_ptr<CycleAccurateSimulator> get_perfsim() final { return std::make_unique<PerfSim<T>>( e, isa); }
    };

    using Map = std::map<std::string, std::unique_ptr<Builder>>;
    const Map map;

    template<typename T, Endian e>
    static void emplace( Map* map, std::string_view name)
    {
        map->emplace( name, std::make_unique<TBuilder<T, e>>( name));
    }

    // Use old-fashioned generation since initializer-lists don't work with unique_ptrs
    static Map generate_map() {
        Map my_map;
        emplace<MIPSI, Endian::little>( &my_map, "mipsI");
        emplace<MIPSII, Endian::little>( &my_map, "mipsII");
        emplace<MIPSIII, Endian::little>( &my_map, "mipsIII");
        emplace<MIPSIV, Endian::little>( &my_map, "mipsIV");
        emplace<MIPS32, Endian::little>( &my_map, "mips32");
        emplace<MIPS64, Endian::little>( &my_map, "mips64");
        emplace<MARS, Endian::little>( &my_map, "mars");
        emplace<MARS64, Endian::little>( &my_map, "mars64");
        emplace<MIPSI, Endian::little>( &my_map, "mipsIel");
        emplace<MIPSII, Endian::little>( &my_map, "mipsIIel");
        emplace<MIPSIII, Endian::little>( &my_map, "mipsIIIel");
        emplace<MIPSIV, Endian::little>( &my_map, "mipsIVel");
        emplace<MIPS32, Endian::little>( &my_map, "mips32el");
        emplace<MIPS64, Endian::little>( &my_map, "mips64el");
        emplace<MARS, Endian::big>( &my_map, "marseb");
        emplace<MARS64, Endian::big>( &my_map, "mars64eb");
        emplace<MIPSI, Endian::big>( &my_map, "mipsIeb");
        emplace<MIPSII, Endian::big>( &my_map, "mipsIIeb");
        emplace<MIPSIII, Endian::big>( &my_map, "mipsIIIeb");
        emplace<MIPSIV, Endian::big>( &my_map, "mipsIVeb");
        emplace<MIPS32, Endian::big>( &my_map, "mips32eb");
        emplace<MIPS64, Endian::big>( &my_map, "mips64eb");
        emplace<RISCV32, Endian::little>( &my_map, "riscv32");
        emplace<RISCV64, Endian::little>( &my_map, "riscv64");
        emplace<RISCV128, Endian::little>( &my_map, "riscv128");
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

    auto get_perfsim( const std::string& name) const
    {
        return get_factory( name)->get_perfsim();
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

    return CycleAccurateSimulator::create_simulator( isa);
}

std::shared_ptr<Simulator>
Simulator::create_simulator( const std::string& isa, bool functional_only)
{
    return create_simulator( isa, functional_only, false);
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
CycleAccurateSimulator::create_simulator( const std::string& isa)
{
    return SimulatorFactory::get_instance().get_perfsim( isa);
}

