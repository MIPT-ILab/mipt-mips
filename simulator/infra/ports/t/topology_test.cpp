/**
 * Topology unit test
 * @author Eric Konks
 */

#include <infra/ports/module.h>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <catch.hpp>

namespace pt = boost::property_tree;

static auto read_json_from_file( const std::string& path)
{
    pt::ptree t;
    read_json( path, t);
    return t;
}

struct SomeTopology : public Root
{
    struct TestModule : public Module {
        pt::ptree topology_load_from_module() { return topology_dumping_impl(); }
        explicit TestModule( Module* root, const std::string& name) : Module( root, name) { }
    };
    struct A : public TestModule {
        WritePort<int>* to_C;
        WritePort<int>* to_D;
        ReadPort<int>* from_C;
        ReadPort<int>* from_D;
        explicit A( Module* root) : TestModule( root, "A")
        {
            to_C = make_write_port<int>( "A_to_C", Port::BW);
            to_D = make_write_port<int>( "A_to_D", Port::BW);
            from_C = make_read_port<int>( "C_to_A", Port::LATENCY);
            from_D = make_read_port<int>( "D_to_A", Port::LATENCY);
        }
    };
    struct B : public TestModule {
        WritePort<int>* to_C;
        WritePort<int>* to_D;
        ReadPort<int>* from_C;
        ReadPort<int>* from_D;
        explicit B( Module* root) : TestModule( root, "B")
        {
            to_C = make_write_port<int>( "B_to_C", Port::BW);
            to_D = make_write_port<int>( "B_to_D", Port::BW);
            from_C = make_read_port<int>( "C_to_B", Port::LATENCY);
            from_D = make_read_port<int>( "D_to_B", Port::LATENCY);
        }
    };
    struct C : public TestModule {
        WritePort<int>* to_A;
        WritePort<int>* to_B;
        ReadPort<int>* from_A;
        ReadPort<int>* from_B;
        explicit C( Module* root) : TestModule( root, "C")
        {
            to_A = make_write_port<int>( "C_to_A", Port::BW);
            to_B = make_write_port<int>( "C_to_B", Port::BW);
            from_A = make_read_port<int>( "A_to_C", Port::LATENCY);
            from_B = make_read_port<int>( "B_to_C", Port::LATENCY);
        }
    };
    struct D : public TestModule {
        WritePort<int>* to_A;
        WritePort<int>* to_B;
        ReadPort<int>* from_A;
        ReadPort<int>* from_B;
        explicit D( Module* root) : TestModule( root, "D")
        {
            to_A = make_write_port<int>( "D_to_A", Port::BW);
            to_B = make_write_port<int>( "D_to_B", Port::BW);
            from_A = make_read_port<int>( "A_to_D", Port::LATENCY);
            from_B = make_read_port<int>( "B_to_D", Port::LATENCY);
        }
    };
    A a;
    B b;
    C c;
    D d;
    void topology_save( const std::string& filename) {  topology_dumping( true, filename); }
    bool check_if_dumps() const { return sout.enabled(); }
    SomeTopology() 
        : Root( "test-root"), a( this), b( &a), c( this), d( &c)
    {
        init_portmap();
    }
};

TEST_CASE( "Topology: dump into file from root")
{
    SomeTopology t;
    t.topology_save( "topology_test.json");
    auto topology = read_json_from_file( "topology_test.json");
    auto exp_topology = read_json_from_file( TEST_PATH + std::string( "/topology_root_test.json"));
    CHECK( exp_topology.get_child( "modules") == topology.get_child( "modules"));
    for ( const pt::ptree::value_type &v : exp_topology.get_child( "portmap")) {
        CHECK( v.second == topology.get_child( "portmap." + v.first));
    }
    CHECK( exp_topology.get_child( "modulemap") == topology.get_child( "modulemap"));
}

TEST_CASE( "Topology: dump into file from module")
{
    SomeTopology t;
    auto topology = t.a.topology_load_from_module();
    auto exp_topology = read_json_from_file( TEST_PATH + std::string( "/topology_module_test.json"));
    CHECK( exp_topology.get_child( "modules") == topology.get_child( "modules"));
    for ( const pt::ptree::value_type &v : exp_topology.get_child( "portmap")) {
        CHECK( v.second == topology.get_child( "portmap." + v.first));
    }
    CHECK( exp_topology.get_child( "modulemap") == topology.get_child( "modulemap"));
}
