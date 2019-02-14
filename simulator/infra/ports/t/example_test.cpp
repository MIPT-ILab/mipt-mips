/**
 * Unit tests for ports
 * @author Denis Los
 */

#include "../ports.h"
#include <catch.hpp>
#include <map>

static const int NONE = -1;
static const int DATA_LIMIT = 5;
static const Cycle EXPECTED_MAX_CYCLE = 8_cl;
static const Cycle CLOCK_LIMIT = 10_cl;

enum CheckCode {  MODULE_A, MODULE_B };

static bool check_data( Cycle cycle, CheckCode code, int data)
{
    static std::map<Cycle, std::pair<int, int>> script =
    // | CYCLE |MODULE_A|MODULE_B|
    {
       { 0_cl, { NONE,    NONE }},
       { 1_cl, { NONE,    NONE }},
       { 2_cl, { 1,       NONE }},
       { 3_cl, { NONE,    2    }},
       { 4_cl, { 3,       NONE }},
       { 5_cl, { NONE,    4    }},
       { 6_cl, { 5,       NONE }},
       { 7_cl, { NONE,    6    }},
       { 8_cl, { NONE,    NONE }}
    };

    if ( cycle > EXPECTED_MAX_CYCLE)
        return false;

    switch ( code)
    {
    case MODULE_A: return script[cycle].first  == data;
    case MODULE_B: return script[cycle].second == data;
    default: return false;
    };
}

static bool check_readiness( Cycle cycle, CheckCode code, bool is_ready)
{
    return check_data( cycle, code, NONE) != is_ready;
}

class A
{
public:
    A( std::shared_ptr<PortMap> map)
        : to_B( map, "A_to_B", PORT_BW, PORT_FANOUT)
        , from_B( map, "B_to_A", PORT_LATENCY)
        , init( map, "init_A", PORT_LATENCY)
        , stop( map, "stop", PORT_BW, PORT_FANOUT)
    { }

    void clock( Cycle cycle)
    {
        bool is_init_ready = init.is_ready( cycle);
        bool is_from_B_ready = from_B.is_ready( cycle);

        if ( cycle != 1_cl)
            CHECK( !is_init_ready);
        CHECK( check_readiness( cycle, MODULE_B, is_from_B_ready));

        if ( is_init_ready)
        {
            auto data = init.read( cycle);
            CHECK( cycle == 1_cl );
            CHECK( data == 0 );
            to_B.write( data + 1, cycle);
        }
        else if ( is_from_B_ready)
        {
            auto data = from_B.read( cycle);
            CHECK( check_data( cycle, MODULE_B, data));
            if ( data >= DATA_LIMIT)
                stop.write( true, cycle);
            else
                to_B.write( data + 1, cycle);
        }
    }

private:
    WritePort<int> to_B;
    ReadPort<int> from_B;
    ReadPort<int> init;
    WritePort<bool> stop;
};

class B
{
public:
    B( std::shared_ptr<PortMap> map)
        : to_A( map, "B_to_A", PORT_BW, PORT_FANOUT)
        , from_A( map, "A_to_B", PORT_LATENCY)
    { };

    void clock( Cycle cycle)
    {
        bool is_from_A_ready = from_A.is_ready( cycle);
        CHECK( check_readiness( cycle, MODULE_A, is_from_A_ready));
        if ( is_from_A_ready)
        {
            auto data = from_A.read( cycle);
            CHECK( check_data( cycle, MODULE_A, data));
            to_A.write( data + 1, cycle);
        }
    }

private:
    WritePort<int> to_A;
    ReadPort<int> from_A;
};

TEST_CASE( "Latency to string")
{
    CHECK( (5_cl).to_string() == "5");
    CHECK( (2_lt).to_string() == "2");
}

TEST_CASE( "test_ports: Test_Ports_A_B")
{
    auto map = PortMap::create_port_map();
    A a( map);
    B b( map);

    WritePort<int> init( map, "init_A", PORT_BW, PORT_FANOUT);
    ReadPort<bool> stop( map, "stop", PORT_LATENCY);

    map->init();
    CHECK( init.get_fanout() == 1);

    // init object A by value 0
    init.write( 0, 0_cl);

    for ( auto cycle = 0_cl; cycle < CLOCK_LIMIT; cycle.inc())
    {
        if ( stop.is_ready( cycle))
        {
            CHECK( cycle == EXPECTED_MAX_CYCLE);
            stop.read( cycle);
            break;
        }

        CHECK( cycle < EXPECTED_MAX_CYCLE);
        a.clock( cycle);
        b.clock( cycle);

        map->clean_up( cycle);
    }
}
