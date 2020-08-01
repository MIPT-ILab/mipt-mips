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

#include <algorithm>

namespace config {
    static const AliasedValue<std::string> isa = { "I", "isa", "mars", "modeled ISA"};
    static const AliasedSwitch disassembly_on = { "d", "disassembly", "print disassembly"};
    static const AliasedSwitch functional_only = { "f", "functional-only", "run functional simulation only"};
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
        Builder() = default;
        virtual ~Builder() = default;
        Builder( const Builder&) = delete;
        Builder( Builder&&) = delete;
        Builder& operator=( const Builder&) = delete;
        Builder& operator=( Builder&&) = delete;
    };

    template<typename T>
    struct TBuilder : public Builder {
        const std::string isa;
        const std::endian e;
        TBuilder( std::string_view isa, std::endian e) : isa( isa), e( e) { }
        std::unique_ptr<Simulator> get_funcsim( bool log) final { return std::make_unique<FuncSim<T>>( e, log, isa); }
        std::unique_ptr<CycleAccurateSimulator> get_perfsim() final { return std::make_unique<PerfSim<T>>( e, isa); }
    };

    std::map<std::string, std::unique_ptr<Builder>> map;

    template<typename T>
    void emplace( std::string_view name, std::endian e)
    {
        map.emplace( name, std::make_unique<TBuilder<T>>( name, e));
    }

    template<typename T>
    void emplace_all_endians( std::string name)
    {
        emplace<T>( name, std::endian::little);
        emplace<T>( name + "le", std::endian::little);
        if constexpr ( std::is_base_of_v<IsMIPS, T>)
            emplace<T>( name + "be", std::endian::big);
    }

    std::string get_supported_isa_message() const
    {
        std::ostringstream oss;
        oss << "Supported ISAs:" << std::endl;
        for ( const auto& isa : get_supported_isa())
            oss << "\t" << isa << std::endl;

        return oss.str();
    }

    auto get_factory( const std::string& name) const try
    {
        return map.at( name).get();
    }
    catch ( const std::out_of_range&)
    {
        throw InvalidISA( name + "\n" + get_supported_isa_message());
    }

    SimulatorFactory()
    {
        emplace_all_endians<MIPSI>( "mipsI");
        emplace_all_endians<MIPSII>( "mipsII");
        emplace_all_endians<MIPSIII>( "mipsIII");
        emplace_all_endians<MIPSIV>( "mipsIV");
        emplace_all_endians<MIPS32>( "mips32");
        emplace_all_endians<MIPS64>( "mips64");
        emplace_all_endians<MARS>( "mars");
        emplace_all_endians<MARS64>( "mars64");
        emplace_all_endians<RISCV32>( "riscv32");
        emplace_all_endians<RISCV64>( "riscv64");
        emplace_all_endians<RISCV128>( "riscv128");
    }

public:
    static SimulatorFactory& get_instance()
    {
        static SimulatorFactory sf;
        return sf;
    }

    std::vector<std::string> get_supported_isa() const
    {
        std::vector<std::string> result( map.size());
        std::transform( map.begin(), map.end(), result.begin(), [](const auto& e) { return e.first; });
        return result;
    }

    auto get_funcsim( const std::string& name, bool log) const
    {
        return get_factory( name)->get_funcsim( log);
    }

    auto get_perfsim( const std::string& name) const
    {
        return get_factory( name)->get_perfsim();
    }
};

std::vector<std::string>
Simulator::get_supported_isa()
{
    return SimulatorFactory::get_instance().get_supported_isa();
}

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
    return create_configured_isa_simulator( config::isa);
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

