/**
 * A simple example of using of ports
 * @author Alexander Titov
 * Copyright 2011 MDSP team
 */

#include <iostream>

#include "../ports.h"

static uint PORT_LATENCY = 1;
static uint PORT_FANOUT = 1;
static uint PORT_BW = 1;

static uint DATA_LIMIT = 5;   // exit when this data value is exceeded
static uint CLOCK_LIMIT = 10; // how many cycles to execute

class A
{
        std::unique_ptr<WritePort<int>> _to_B;
        std::unique_ptr<ReadPort<int>> _from_B;

        std::unique_ptr<ReadPort<int>> _init;
        std::unique_ptr<WritePort<bool>> _stop;

        // process recieved data
        void processData ( int data, int cycle);

public:

        A ();
        ~A () { }

        /* "Clock" A object, i.e. execute all actions
         * that it performs for a cycle: read ports,
         * process data and so on.
         */
        void clock ( int cycle);
};

class B
{
        std::unique_ptr<WritePort<int>> _to_A;
        std::unique_ptr<ReadPort<int>> _from_A;

        // process recieved data
        void processData ( int data,  int cycle);

public:

        B ();
        ~B () { }

        /* "Clock" B object, i.e. execute all actions
         * that it performs for a cycle: read ports,
         * process data and so on.
         */
        void clock ( int cycle);
};

int main()
{
        A _a;
        B _b;

        WritePort<int> _init( "Init_A", PORT_BW, PORT_FANOUT);
        ReadPort<bool> _stop( "Stop", PORT_LATENCY);

        // Connect all the ports.
        init_ports();

        // Init A object by 0 value
        _init.write(0, 0);

        for ( int cycle = 0; cycle < CLOCK_LIMIT; cycle++)
        {
                std::cout << "\n--- cycle " << cycle << "----\n";

                // check the stop port from A object
                bool tmp;
                if ( _stop.read( &tmp, cycle))
                {
                        std::cout << "-------------------------------\n\n"
                                 << "A stop signal is recieved.\n"
                                 << "Calculation is COMPLETED in cycle " << cycle << ".\n\n";
                        return 0;
                }

                // execute each module
                _a.clock( cycle);
                _b.clock( cycle);

                check_ports( cycle);
        }

        std::cout << "-------------------------------\n\n"
                 << "Calculation is FINISHED by CLOCK_LIMIT (=" << CLOCK_LIMIT << ").\n\n";

        destroy_ports();

        return 0;
}

//=================================================================
//                 Implementation of A class
//=================================================================

A::A()
{
        _to_B = make_write_port<int> ( "A_to_B", PORT_BW, PORT_FANOUT);
        _from_B = make_read_port<int> ( "B_to_A", PORT_LATENCY);

        _init = make_read_port<int> ( "Init_A", PORT_LATENCY);
        _stop = make_write_port<bool> ( "Stop", PORT_BW, PORT_FANOUT);
}

void A::processData ( int data, int cycle)
{
        // perform calculation
        ++data;
        std::cout << "\t\tProcess data: new value = " << data << "\n";

        // If data limit is exceeded
        // then send a stop signal
        if ( data > DATA_LIMIT)
        {
                std::cout << "\t\t\t data limit is exceeded => "
                         << "send a stop signal\n";
                _stop->write( true, cycle);

                return;
        }

        std::cout << "\t\t\tsend it to B\n";
        _to_B->write( data, cycle);
}

void A::clock ( int cycle)
{
        std::cout << "Clock of A:\n";

        int data;

        // Read all the port in order
        // and break the loop if there is no message to read.
        while( true)
        {
                if( _init->read( &data, cycle))
                {
                        std::cout << "\tread the init port: data = " << data << "\n";

                } else if ( _from_B->read( &data, cycle))
                {
                        std::cout << "\tread the port from B: data = " << data << "\n";

                } else
                {
                        std::cout << "\tnothing to read\n";
                        break;
                }

                this->processData( data, cycle);
        }
}

//=================================================================
//                 Implementation of B class
//=================================================================

B::B ()
{
        _to_A = make_write_port<int> ( "B_to_A", PORT_BW, PORT_FANOUT);
        _from_A = make_read_port<int> ( "A_to_B", PORT_LATENCY);
}

void B::processData ( int data, int cycle)
{
        // perform calculation
        ++data;

        std::cout << "\t\tProcess data: new value = "
                        << data << "\n" << "\t\t\tsend it to A\n";

        _to_A->write( data, cycle);
}

void B::clock ( int cycle)
{
        std::cout << "Clock of B:\n";

        int data;

        if ( _from_A->read( &data, cycle))
        {
                std::cout << "\tread the port from A: data = " << data << "\n";
                this->processData( data, cycle);

        } else
        {
                std::cout << "\tnothing to read\n";
        }
}

