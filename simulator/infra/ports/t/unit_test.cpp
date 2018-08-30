/**
 * Unit tests for ports
 * @author Denis Los
 */

#include <catch.hpp>

// Module
#include "../ports.h"

#include <cassert>
#include <map>

TEST_CASE( "Latency to string")
{
    CHECK( (5_cl).to_string() == "5");
    CHECK( (2_lt).to_string() == "2");
}

namespace ports {
    namespace testing {

        static const Latency PORT_LATENCY = 1_lt;
        static const uint32 PORT_FANOUT = 1;
        static const uint32 PORT_BANDWIDTH = 1;

        static const int DATA_LIMIT = 5;
        static const Cycle CLOCK_LIMIT = 10_cl;

        class Logic
        {
            public:
                enum class CheckCode
                {
                    DATA_FROM_A,
                    DATA_FROM_B,
                    DATA_INIT,
                    IS_FROM_A_READY,
                    IS_FROM_B_READY,
                    IS_INIT_READY,
                    IS_STOP_READY
                };

                bool check_readiness( Cycle cycle, CheckCode code, bool is_ready);
                bool check_data( Cycle cycle, CheckCode code, int data);

            private:
                static const int NONE = -1;
                static const Cycle EXPECTED_MAX_CYCLE;

                struct State
                {
                    bool is_from_A_ready;
                    bool is_from_B_ready;
                    bool is_init_ready;
                    int data_from_A;
                    int data_from_B;
                };

                std::map<Cycle, State> blogic=
                //  | CYCLE |IS_FROM_A_READY|IS_FROM_B_READY|IS_INIT_READY|DATA_FROM_A|DATA_FROM_B|
                {
                   { 0_cl, { false,          false,          false,        NONE,       NONE        }},
                   { 1_cl, { false,          false,          true,         NONE,       NONE        }},
                   { 2_cl, { true,           false,          false,        1,          NONE        }},
                   { 3_cl, { false,          true,           false,        NONE,       2           }},
                   { 4_cl, { true,           false,          false,        3,          NONE        }},
                   { 5_cl, { false,          true,           false,        NONE,       4           }},
                   { 6_cl, { true,           false,          false,        5,          NONE        }},
                   { 7_cl, { false,          true,           false,        NONE,       6           }},
                   { 8_cl, { false,          false,          false,        NONE,       NONE        }}
                };
        };

        const Cycle Logic::EXPECTED_MAX_CYCLE = 8_cl;


        class A
        {
            public:
                A();
                void clock( Cycle cycle);

            private:
                std::unique_ptr<WritePort<int>> to_B;
                std::unique_ptr<ReadPort<int>> from_B;

                std::unique_ptr<ReadPort<int>> init;
                std::unique_ptr<WritePort<bool>> stop;

                void process_data( int data, Cycle cycle);
        };



        class B
        {
            public:
                B();
                void clock( Cycle cycle);

            private:
                std::unique_ptr<WritePort<int>> to_A;
                std::unique_ptr<ReadPort<int>> from_A;

                void process_data ( int data, Cycle cycle);
        };

        static Logic logic{};
    } // namespace testing
} // namespace ports

using ports::testing::Logic;

TEST_CASE( "test_ports: Test_Ports_A_B")
{
    ports::testing::A a;;
    ports::testing::B b;;

    WritePort<int> init( "init_A", ports::testing::PORT_BANDWIDTH, ports::testing::PORT_FANOUT);
    ReadPort<bool> stop( "stop", ports::testing::PORT_LATENCY);;

    // connect all the ports
    init_ports();;

    // init object A by value 0
    init.write( 0, 0_cl);;

    for ( auto cycle = 0_cl; cycle < ports::testing::CLOCK_LIMIT; cycle.inc())
    {
        //check the stop port from object A
        bool is_ready;
        is_ready = stop.is_ready( cycle);;
        CHECK( ports::testing::logic.check_readiness( cycle,
                                                            Logic::CheckCode::IS_STOP_READY,
                                                            is_ready));
        if ( is_ready)
        {
            stop.read( cycle);;
            break;
        }

        a.clock( cycle);
        b.clock( cycle);

        clean_up_ports( cycle);;
    }

    destroy_ports();;
}


namespace ports {
    namespace testing {

        bool Logic::check_readiness( Cycle cycle, CheckCode code, bool is_ready)
        {
            if ( cycle > EXPECTED_MAX_CYCLE)
                return false;

            bool is_ok = false;
            switch ( code)
            {
                case CheckCode::IS_STOP_READY:
                    // STOP port should be ready only in the expected max cycle
                    return (cycle == EXPECTED_MAX_CYCLE) == is_ready;

                case CheckCode::IS_INIT_READY:
                    is_ok = blogic[cycle].is_init_ready == is_ready;

                    // If it is ready it will be read in the current cycle
                    // so blogic should be updated to not interfere with
                    // subsequent checks in the same cycle
                    if ( is_ok && is_ready)
                        blogic[cycle].is_init_ready = false;

                    break;

                case CheckCode::IS_FROM_A_READY:
                    is_ok = blogic[cycle].is_from_A_ready == is_ready;

                    // If it is ready it will be read in the current cycle
                    // so blogic should be updated to not interfere with
                    // subsequent checks in the same cycle
                    if ( is_ok && is_ready)
                        blogic[cycle].is_from_A_ready = false;

                    break;

                case CheckCode::IS_FROM_B_READY:
                    is_ok = blogic[cycle].is_from_B_ready == is_ready;

                    // If it is ready it will be read in the current cycle
                    // so blogic should be updated to not interfere with
                    // subsequent checks in the same cycle
                    if ( is_ok && is_ready)
                        blogic[cycle].is_from_B_ready = false;

                    break;

                default:
                    break;
            }

            return is_ok;
        }

        bool Logic::check_data( Cycle cycle, CheckCode code, int data)
        {
            if ( cycle > EXPECTED_MAX_CYCLE || data < 0)
                return false;

            switch ( code)
            {
                case CheckCode::DATA_INIT:
                    // data from init port should be equal to 0
                    // merely in cycle 1
                    return cycle == 1_cl && data == 0;

                case CheckCode::DATA_FROM_A:
                    return blogic[cycle].data_from_A == data;

                case CheckCode::DATA_FROM_B:
                    return blogic[cycle].data_from_B == data;

                default:
                    return false;
            };
        }



        A::A()
            : to_B{ make_write_port<int>( "A_to_B", PORT_BANDWIDTH, PORT_FANOUT)}
            , from_B{ make_read_port<int>( "B_to_A", PORT_LATENCY)}

            , init{ make_read_port<int> ( "init_A", PORT_LATENCY)}
            , stop{ make_write_port<bool> ( "stop", PORT_BANDWIDTH, PORT_FANOUT)}
        { }

        void A::process_data( int data, Cycle cycle)
        {
            ++data;

            // If data limit is exceeded
            // stop signal will be sent
            if ( data > DATA_LIMIT)
            {
                stop->write( true, cycle);;
                return;
            }

            to_B->write( data, cycle);;
        }

        void A::clock( Cycle cycle)
        {
            int data;

            bool is_init_ready;
            bool is_from_B_ready;
            while( true)
            {
                is_init_ready = init->is_ready( cycle);;
                CHECK( ports::testing::logic.check_readiness( cycle,
                                                                    Logic::CheckCode::IS_INIT_READY,
                                                                    is_init_ready));

                is_from_B_ready = from_B->is_ready( cycle);;
                CHECK( ports::testing::logic.check_readiness( cycle,
                                                                    Logic::CheckCode::IS_FROM_B_READY,
                                                                    is_from_B_ready));

                if( is_init_ready)
                {
                    data = init->read( cycle);;
                    CHECK( ports::testing::logic.check_data( cycle,
                                                                   Logic::CheckCode::DATA_INIT,
                                                                   data));
                }
                else if ( is_from_B_ready)
                {
                    data = from_B->read( cycle);;
                    CHECK( ports::testing::logic.check_data( cycle,
                                                                   Logic::CheckCode::DATA_FROM_B,
                                                                   data));
                }
                else
                    break;

                process_data( data, cycle);
            }
        }



        B::B()
            : to_A{ make_write_port<int>( "B_to_A", PORT_BANDWIDTH, PORT_FANOUT)}
            , from_A{ make_read_port<int>( "A_to_B", PORT_LATENCY)}
        { }

        void B::process_data( int data, Cycle cycle)
        {
            ++data;

            to_A->write( data, cycle);;
        }

        void B::clock( Cycle cycle)
        {
            bool is_from_A_ready;
            is_from_A_ready = from_A->is_ready( cycle);;
            CHECK( ports::testing::logic.check_readiness( cycle,
                                                                Logic::CheckCode::IS_FROM_A_READY,
                                                                is_from_A_ready));
            if ( is_from_A_ready)
            {
                int data;
                data = from_A->read( cycle);;
                CHECK( ports::testing::logic.check_data( cycle,
                                                               Logic::CheckCode::DATA_FROM_A,
                                                               data));

                process_data( data, cycle);;
            }
        }

    } // namespace testing
} // namespace ports

