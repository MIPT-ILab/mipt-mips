/**
 * Ports unit test
 * @author Pavel Kryukov
 * Copyright 2019 MIPT-MIPS team
 */

#include "../module.h"
#include <catch.hpp>
#include <map>
#include <boost/algorithm/string/replace.hpp>

TEST_CASE( "Latency to string")
{
    CHECK( (5_cl).to_string() == "5");
    CHECK( (2_lt).to_string() == "2");
}

struct BaseTestRoot : public Root
{
    BaseTestRoot() : Root( "test-root") { }
};

TEST_CASE("Ports: no write port")
{
    struct TestRoot : public BaseTestRoot
    {
        TestRoot()
        {
            auto input = make_read_port<int>( "Key", PORT_LATENCY);
            CHECK_THROWS_AS( init_portmap(), PortError);
        }
    } tr;
}

TEST_CASE("Ports: no read port")
{
    struct TestRoot : public BaseTestRoot
    {
        TestRoot()
        {
            auto output = make_write_port<int>( "Yek", PORT_BW);
            CHECK_THROWS_AS( init_portmap(), PortError);
        }
    } tr;
}

TEST_CASE("Ports: two write ports")
{
    struct TestRoot : public BaseTestRoot
    {
        TestRoot()
        {
            auto input = make_read_port<int>( "Key", PORT_LATENCY);
            auto output = make_write_port<int>( "Key", PORT_BW);
            CHECK_THROWS_AS( make_write_port<int>( "Key", PORT_BW), PortError);
        }
    } tr;
}

TEST_CASE("Ports: type mismatch")
{
    struct TestRoot : public BaseTestRoot
    {
        TestRoot()
        {
            auto input = make_read_port<int>( "Key", PORT_LATENCY);
            auto output = make_write_port<std::string>( "Key", PORT_BW);
            CHECK_THROWS_AS( init_portmap(), PortError);
        }
    } tr;
}

struct PairOfPorts : public BaseTestRoot
{
    std::unique_ptr<ReadPort<int>> rp;
    std::unique_ptr<WritePort<int>> wp;

    explicit PairOfPorts( uint32 bw = PORT_BW, Latency lat = PORT_LATENCY)
    {
        rp = make_read_port<int>( "Key", lat);
        wp = make_write_port<int>( "Key", bw);
        init_portmap();
    }
};

TEST_CASE("Ports: simple transmission")
{
    PairOfPorts pop;
    pop.wp->write( 11, 0_cl);
    CHECK( !pop.rp->is_ready( 0_cl));
    CHECK( pop.rp->is_ready( 1_cl));
    CHECK( pop.rp->read( 1_cl) == 11);
}

TEST_CASE("Ports: throw if not ready")
{
    PairOfPorts pop;
    pop.wp->write( 1, 0_cl);
    CHECK_THROWS_AS( pop.rp->read( 0_cl), PortError);
}

TEST_CASE("Ports: read once")
{
    PairOfPorts pop;
    pop.wp->write( 11, 0_cl);
    pop.rp->read( 1_cl);
    CHECK( !pop.rp->is_ready( 1_cl));
}

TEST_CASE("Ports: overload bandwidth")
{
    PairOfPorts pop;
    pop.wp->write( 11, 0_cl);
    CHECK_THROWS_AS( pop.wp->write( 12, 0_cl), PortError);
}

TEST_CASE("Ports: sequential read")
{
    PairOfPorts pop;

    pop.wp->write( 10, 0_cl);
    pop.wp->write( 11, 1_cl);
    CHECK( pop.rp->is_ready( 1_cl));
    CHECK( pop.rp->read( 1_cl) == 10);
    CHECK( !pop.rp->is_ready( 1_cl));
    CHECK( pop.rp->read( 2_cl) == 11);
}

TEST_CASE("Ports: non-regular read")
{
    PairOfPorts pop;

    pop.wp->write( 10, 0_cl);
    pop.wp->write( 11, 1_cl);
    CHECK( pop.rp->is_ready( 1_cl));
    CHECK( pop.rp->is_ready( 2_cl));
    CHECK( pop.rp->read( 2_cl) == 11);
}

struct SomeHiearchy : public BaseTestRoot
{
    struct DumpCheckingModule : public Module
    {
        bool check_if_dumps() const { return sout.enabled(); }
        using Module::Module;
    };

    std::unique_ptr<DumpCheckingModule> a, b, c, d, e, f;
    explicit SomeHiearchy( const std::string& dump_string)
    {
        /* Root
         * | \
         * A  D
         * |  | \
         * B  E  F
         * |
         * C
         */
        a = std::make_unique<DumpCheckingModule>( this, "A");
        b = std::make_unique<DumpCheckingModule>( a.get(), "B");
        c = std::make_unique<DumpCheckingModule>( b.get(), "C");
        d = std::make_unique<DumpCheckingModule>( this, "D");
        e = std::make_unique<DumpCheckingModule>( d.get(), "E");
        f = std::make_unique<DumpCheckingModule>( d.get(), "F");
        enable_logging( dump_string);
    }
};

TEST_CASE("Module: dump nothing")
{
    SomeHiearchy h( "");
    CHECK_FALSE( h.a->check_if_dumps());
    CHECK_FALSE( h.b->check_if_dumps());
    CHECK_FALSE( h.c->check_if_dumps());
    CHECK_FALSE( h.d->check_if_dumps());
    CHECK_FALSE( h.e->check_if_dumps());
    CHECK_FALSE( h.f->check_if_dumps());
}

TEST_CASE("Module: dump all")
{
    SomeHiearchy h( "test-root");
    CHECK( h.a->check_if_dumps());
    CHECK( h.b->check_if_dumps());
    CHECK( h.c->check_if_dumps());
    CHECK( h.d->check_if_dumps());
    CHECK( h.e->check_if_dumps());
    CHECK( h.f->check_if_dumps());
}

TEST_CASE("Module: dump subtree")
{
    SomeHiearchy h( "A");
    CHECK( h.a->check_if_dumps());
    CHECK( h.b->check_if_dumps());
    CHECK( h.c->check_if_dumps());
    CHECK_FALSE( h.d->check_if_dumps());
    CHECK_FALSE( h.e->check_if_dumps());
    CHECK_FALSE( h.f->check_if_dumps());
}

TEST_CASE("Module: dump subtrees")
{
    SomeHiearchy h( "B,E");
    CHECK_FALSE( h.a->check_if_dumps());
    CHECK( h.b->check_if_dumps());
    CHECK( h.c->check_if_dumps());
    CHECK_FALSE( h.d->check_if_dumps());
    CHECK( h.e->check_if_dumps());
    CHECK_FALSE( h.f->check_if_dumps());
}

TEST_CASE("Module: exclusion")
{
    SomeHiearchy h( "test-root,!D,!C,F");
    CHECK( h.a->check_if_dumps());
    CHECK( h.b->check_if_dumps());
    CHECK_FALSE( h.c->check_if_dumps());
    CHECK_FALSE( h.d->check_if_dumps());
    CHECK_FALSE( h.e->check_if_dumps());
    CHECK( h.f->check_if_dumps());
}

struct SomeTopology : public BaseTestRoot
{
    struct A : public Module {
        std::unique_ptr<WritePort<int>> to_C;
        std::unique_ptr<WritePort<int>> to_D;
        std::unique_ptr<ReadPort<int>> from_C;
        std::unique_ptr<ReadPort<int>> from_D;
        explicit A( Module* root) : Module( root, "A")
        {
            to_C = make_write_port<int>( "A_to_C", PORT_BW);
            to_D = make_write_port<int>( "A_to_D", PORT_BW);
            from_C = make_read_port<int>( "C_to_A", PORT_LATENCY);
            from_D = make_read_port<int>( "D_to_A", PORT_LATENCY);
        }
    };
    struct B : public Module {
        std::unique_ptr<WritePort<int>> to_C;
        std::unique_ptr<WritePort<int>> to_D;
        std::unique_ptr<ReadPort<int>> from_C;
        std::unique_ptr<ReadPort<int>> from_D;
        explicit B( Module* root) : Module( root, "B")
        {
            to_C = make_write_port<int>( "B_to_C", PORT_BW);
            to_D = make_write_port<int>( "B_to_D", PORT_BW);
            from_C = make_read_port<int>( "C_to_B", PORT_LATENCY);
            from_D = make_read_port<int>( "D_to_B", PORT_LATENCY);
        }
    };
    struct C : public Module {
        std::unique_ptr<WritePort<int>> to_A;
        std::unique_ptr<WritePort<int>> to_B;
        std::unique_ptr<ReadPort<int>> from_A;
        std::unique_ptr<ReadPort<int>> from_B;
        explicit C( Module* root) : Module( root, "C")
        {
            to_A = make_write_port<int>( "C_to_A", PORT_BW);
            to_B = make_write_port<int>( "C_to_B", PORT_BW);
            from_A = make_read_port<int>( "A_to_C", PORT_LATENCY);
            from_B = make_read_port<int>( "B_to_C", PORT_LATENCY);
        }
    };
    struct D : public Module {
        std::unique_ptr<WritePort<int>> to_A;
        std::unique_ptr<WritePort<int>> to_B;
        std::unique_ptr<ReadPort<int>> from_A;
        std::unique_ptr<ReadPort<int>> from_B;
        explicit D( Module* root) : Module( root, "D")
        {
            to_A = make_write_port<int>( "D_to_A", PORT_BW);
            to_B = make_write_port<int>( "D_to_B", PORT_BW);
            from_A = make_read_port<int>( "A_to_D", PORT_LATENCY);
            from_B = make_read_port<int>( "B_to_D", PORT_LATENCY);
        }
    };
    std::unique_ptr<A> a;
    std::unique_ptr<B> b;
    std::unique_ptr<C> c;
    std::unique_ptr<D> d;
    void modulemap_load( pt::ptree& pt_modulemap) const { modulemap_dumping( pt_modulemap); }
    void portmap_load( pt::ptree& pt_portmap) const { portmap_dumping( pt_portmap); }
    void modules_load( pt::ptree& pt_modules) const { module_dumping( pt_modules); }
    void topology_load( pt::ptree& pt_topology) const { topology_dumping_impl( pt_topology); }
    void topology_save() { topology_dumping(true); }
    void topology_save(const std::string& filename) { topology_dumping(true, filename); }
    bool check_if_dumps() const { return sout.enabled(); }
    explicit SomeTopology()
    {
        /* Root
         * | \
         * A  C
         * |   \ 
         * B    D  
         */
        a = std::make_unique<A>( this);
        c = std::make_unique<C>( this);
        b = std::make_unique<B>( a.get());
        d = std::make_unique<D>( c.get());
        init_portmap();
    }
};

TEST_CASE( "Topology: modulemap")
{
    std::string file_path = __FILE__;
    boost::algorithm::replace_last( file_path, "unit_test.cpp", "modulemap_test.json");
    SomeTopology t;
    pt::ptree pt_exp_modulemap;
    pt::ptree pt_modulemap;
    CHECK_NOTHROW( read_json( file_path, pt_exp_modulemap));
    CHECK_NOTHROW( t.modulemap_load( pt_modulemap));
    CHECK( pt_modulemap == pt_exp_modulemap);
}

TEST_CASE( "Topology: portmap")
{
    std::string file_path = __FILE__;
    boost::algorithm::replace_last( file_path, "unit_test.cpp", "portmap_test.json");
    SomeTopology t;
    pt::ptree pt_exp_portmap;
    pt::ptree pt_portmap;
    CHECK_NOTHROW( read_json( file_path, pt_exp_portmap));
    CHECK_NOTHROW( t.portmap_load( pt_portmap));
    for ( const pt::ptree::value_type &v : pt_exp_portmap) {
        CHECK( v.second == pt_portmap.get_child(v.first.data()));
    }
}

TEST_CASE( "Topology: modules")
{
    std::string file_path = __FILE__;
    boost::algorithm::replace_last( file_path, "unit_test.cpp", "modules_test.json");
    SomeTopology t;
    pt::ptree pt_exp_modules;
    pt::ptree pt_modules;
    CHECK_NOTHROW( read_json( file_path, pt_exp_modules));
    CHECK_NOTHROW( t.modules_load( pt_modules));
    CHECK( pt_modules == pt_exp_modules);
}

TEST_CASE( "Topology: topology")
{
    std::string file_path = __FILE__;
    boost::algorithm::replace_last( file_path, "unit_test.cpp", "topology_test.json");
    SomeTopology t;
    pt::ptree pt_exp_topology;
    pt::ptree pt_topology;
    CHECK_NOTHROW( read_json( file_path, pt_exp_topology));
    CHECK_NOTHROW( t.topology_load( pt_topology));
    CHECK( pt_exp_topology.get_child("modules") == pt_topology.get_child( "modules"));
    for ( const pt::ptree::value_type &v : pt_exp_topology.get_child( "portmap")) {
        CHECK( v.second == pt_topology.get_child("portmap." + std::string(v.first.data())));
    }
    CHECK( pt_exp_topology.get_child("modulemap") == pt_topology.get_child("modulemap"));
}

TEST_CASE( "Topology: dump into sout")
{
    SomeTopology t;
    pt::ptree pt_topology;
    CHECK_NOTHROW( t.topology_save());
    CHECK( t.check_if_dumps());
}

TEST_CASE( "Topology: dump into file")
{
    SomeTopology t;
    pt::ptree pt_topology;
    CHECK_NOTHROW( t.topology_save("topology.json"));
    CHECK( t.check_if_dumps());
}