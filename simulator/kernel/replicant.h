/*
 * replicant.h - adapter for several CPU models under single interface
 * @author Pavel Kryukov
 * Copyright 2019 MIPT-MIPS
 */

#include <simulator.h>

#include <memory>
#include <vector>

class CPUReplicant : public CPUModel
{
public:
    explicit CPUReplicant( const std::shared_ptr<CPUModel>& s) : primary( s) { }

    void add_replica( const std::shared_ptr<CPUModel>& s)
    {
        replicas.emplace_back( s);
        primary.lock()->duplicate_all_registers_to( s.get());
    }
    
    void set_target( const Target& target) final
    {
        primary.lock()->set_target( target);
        for ( auto& e : replicas)
            e.lock()->set_target( target);
    }

    Addr get_pc() const final { return primary.lock()->get_pc(); }
    std::string_view get_isa() const final { return primary.lock()->get_isa(); }
    size_t sizeof_register() const final { return primary.lock()->sizeof_register(); }
    size_t max_cpu_register() const final { return primary.lock()->max_cpu_register(); }
    uint64 read_cpu_register( size_t regno) const final { return primary.lock()->read_cpu_register( regno); }
    uint64 read_gdb_register( size_t regno) const final { return primary.lock()->read_gdb_register( regno); }
    uint64 read_csr_register( std::string_view name) const final { return primary.lock()->read_csr_register( name); }
    
    void write_cpu_register( size_t regno, uint64 value) final
    {
        primary.lock()->write_cpu_register( regno, value);
        for ( auto& e : replicas)
            e.lock()->write_cpu_register( regno, value);
    }

    void write_gdb_register( size_t regno, uint64 value) final
    {
        primary.lock()->write_gdb_register( regno, value);
        for ( auto& e : replicas)
            e.lock()->write_gdb_register( regno, value);
    }

    void write_csr_register( std::string_view name, uint64 value) final
    {
        primary.lock()->write_csr_register( name, value);
        for ( auto& e : replicas)
            e.lock()->write_csr_register( name, value);
    }

private:
    std::weak_ptr<CPUModel> primary;
    std::vector<std::weak_ptr<CPUModel>> replicas;
};
