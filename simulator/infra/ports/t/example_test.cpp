/**
 * Unit tests for ports
 * @author Denis Los
 */

#include "../module.h"
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

class A : public Module
{
public:
    explicit A( Module* root) : Module( root, "A")
    {
        to_B = make_write_port<int>( "A_to_B", PORT_BW);
        from_B = make_read_port<int>( "B_to_A", PORT_LATENCY);
        init = make_read_port<int>( "init_A", PORT_LATENCY);
        stop = make_write_port<bool>( "stop", PORT_BW);
    }

    void clock( Cycle cycle)
    {
        bool is_init_ready = init->is_ready( cycle);
        bool is_from_B_ready = from_B->is_ready( cycle);

        if ( cycle != 1_cl)
            CHECK( !is_init_ready);
        CHECK( check_readiness( cycle, MODULE_B, is_from_B_ready));

        if ( is_init_ready)
        {
            auto data = init->read( cycle);
            CHECK( cycle == 1_cl );
            CHECK( data == 0 );
            to_B->write( data + 1, cycle);
        }
        else if ( is_from_B_ready)
        {
            auto data = from_B->read( cycle);
            CHECK( check_data( cycle, MODULE_B, data));
            if ( data >= DATA_LIMIT)
                stop->write( true, cycle);
            else
                to_B->write( data + 1, cycle);
        }
    }

private:
    WritePort<int>* to_B;
    ReadPort<int>* from_B;
    ReadPort<int>* init;
    WritePort<bool>* stop;
};

class B : public Module
{
public:
    explicit B(  Module* root) : Module( root, "B")
    {
        to_A = make_write_port<int>( "B_to_A", PORT_BW);
        from_A = make_read_port<int>( "A_to_B", PORT_LATENCY);
    };

    void clock( Cycle cycle)
    {
        bool is_from_A_ready = from_A->is_ready( cycle);
        CHECK( check_readiness( cycle, MODULE_A, is_from_A_ready));
        if ( is_from_A_ready)
        {
            auto data = from_A->read( cycle);
            CHECK( check_data( cycle, MODULE_A, data));
            to_A->write( data + 1, cycle);
        }
    }

private:
    WritePort<int>* to_A;
    ReadPort<int>* from_A;
};

class TestRoot : public Root
{
public:
    TestRoot() : Root( "test-root"), a( this), b( this)
    {
        init = make_write_port<int>( "init_A", PORT_BW);
        stop = make_read_port<bool>( "stop", PORT_LATENCY);
        init_portmap();
        CHECK( init->get_fanout() == 1);

        // init object A by value 0
        init->write( 0, 0_cl);
    }

    bool clock( Cycle cycle)
    {
        if ( stop->is_ready( cycle))
        {
            CHECK( cycle == EXPECTED_MAX_CYCLE);
            stop->read( cycle);
            return true;
        }

        CHECK( cycle < EXPECTED_MAX_CYCLE);
        a.clock( cycle);
        b.clock( cycle);
        return false;
    }
private:
    A a;
    B b;

    WritePort<int>* init;
    ReadPort<bool>* stop;
};

TEST_CASE( "test_ports: Test_Ports_A_B")
{
    TestRoot tr;

    for ( auto cycle = 0_cl; cycle < CLOCK_LIMIT; cycle.inc())
        if ( tr.clock( cycle))
            break;
}
