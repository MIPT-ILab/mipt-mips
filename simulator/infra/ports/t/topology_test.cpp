/**
 * Topology unit test
 * @author Eric Konks
 */

#include "../module.h"
#include <catch.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;

static auto read_json_from_file( const std::string& path)
{
    pt::ptree t;
    read_json( TEST_PATH + path, t);
    return t;
}

struct SomeTopology : public Root
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
    void modulemap_load( pt::ptree* modulemap) const { modulemap_dumping( modulemap); }
    void portmap_load( pt::ptree* pt_portmap) const { portmap_dumping( pt_portmap); }
    void modules_load( pt::ptree* modules) const { module_dumping( modules); }
    void topology_load( pt::ptree* topology) const { topology_dumping_impl( topology); }
    void topology_save( const std::string& filename) { topology_dumping( true, filename); }
    bool check_if_dumps() const { return sout.enabled(); }
    explicit SomeTopology() : Root( "test-root")
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
    SomeTopology t;
    auto exp_modulemap = read_json_from_file( "/modulemap_test.json");
    pt::ptree modulemap;
    t.modulemap_load( &modulemap);
    CHECK( modulemap == exp_modulemap);
}

TEST_CASE( "Topology: portmap")
{
    SomeTopology t;
    auto exp_portmap = read_json_from_file( "/portmap_test.json");
    pt::ptree portmap;
    t.portmap_load( &portmap);
    for ( const pt::ptree::value_type &v : exp_portmap) {
        CHECK( v.second == portmap.get_child( v.first.data()));
    }
}

TEST_CASE( "Topology: modules")
{
    SomeTopology t;
    auto exp_modules = read_json_from_file( "/modules_test.json");
    pt::ptree modules;
    t.modules_load( &modules);
    CHECK( modules == exp_modules);
}

TEST_CASE( "Topology: topology")
{
    SomeTopology t;
    auto exp_topology = read_json_from_file( "/topology_test.json");
    pt::ptree topology;
    t.topology_load( &topology);
    CHECK( exp_topology.get_child( "modules") == topology.get_child( "modules"));
    for ( const pt::ptree::value_type &v : exp_topology.get_child( "portmap")) {
        CHECK( v.second == topology.get_child( "portmap." + std::string( v.first.data())));
    }
    CHECK( exp_topology.get_child( "modulemap") == topology.get_child( "modulemap"));
}

TEST_CASE( "Topology: dump into file")
{
    SomeTopology t;
    t.topology_save( "topology.json");
    CHECK( t.check_if_dumps());
}
