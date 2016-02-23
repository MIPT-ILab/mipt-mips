/**
 * A simple example of using of ports
 * @author Alexander Titov
 * Copyright 2011 MDSP team
 */

#include <iostream>

#include <ports.h>

using namespace std;

#define PORT_LATENCY 1
#define PORT_FANOUT 1
#define PORT_BW 1

#define DATA_LIMIT 5 // exit when this data value is exceeded
#define CLOCK_LIMIT 10 // how many cycles to execute

class A 
{
        WritePort<int>* _to_B;
        ReadPort<int>* _from_B;

        ReadPort<int>* _init;
        WritePort<bool>* _stop;
        
        // process recieved data
        void processData ( int data, int cycle);

public:
        
        A ();
        ~A ();
        
        /* "Clock" A object, i.e. execute all actions
         * that it performs for a cycle: read ports,
         * process data and so on.
         */
        void clock ( int cycle);
};

class B
{
        WritePort<int>* _to_A;
        ReadPort<int>* _from_A;

        // process recieved data
        void processData ( int data,  int cycle);

public:
        
        B ();
        ~B ();

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
        // MUST be after declaration of all ports!
        // MUST be for each port type!
        Port<int>::init();
        Port<bool>::init();

        // Init A object by 0 value
        _init.write(0, 0);

        for ( int cycle = 0; cycle < CLOCK_LIMIT; cycle++)
        {
                cout << "\n--- cycle " << cycle << "----\n";

                // check the stop port from A object
                bool tmp;
                if ( _stop.read( &tmp, cycle))
                {
                        cout << "-------------------------------\n\n"
                                 << "A stop signal is recieved.\n"
                             << "Calculation is COMPLETED in cycle " << cycle << ".\n\n";
                        return 0;
                }

                // execute each module
                _a.clock( cycle);
                _b.clock( cycle);
        }
        
        cout << "-------------------------------\n\n"
                 << "Calculation is FINISHED by CLOCK_LIMIT (=" << CLOCK_LIMIT << ").\n\n";
        return 0;
}

//=================================================================
//                 Implementation of A class
//=================================================================

A::A()
{
        _to_B = new WritePort<int> ( "A_to_B", PORT_BW, PORT_FANOUT);
        _from_B = new ReadPort<int> ( "B_to_A", PORT_LATENCY);
                
        _init = new ReadPort<int> ( "Init_A", PORT_LATENCY);
        _stop = new WritePort<bool> ( "Stop", PORT_BW, PORT_FANOUT);
}

A::~A ()
{
        delete _to_B;
        delete _from_B;
        delete _init;
        delete _stop; 
}
        
void A::processData ( int data, int cycle)
{
        // perform calculation
        ++data;
        cout << "\t\tProcess data: new value = " << data << "\n";

        // If data limit is exceeded 
        // then send a stop signal
        if ( data > DATA_LIMIT)
        {
                cout << "\t\t\t data limit is exceeded => "
                     << "send a stop signal\n";
                _stop->write( true, cycle);

                return;
        }

        cout << "\t\t\tsend it to B\n";
        _to_B->write( data, cycle);
}

void A::clock ( int cycle)
{
        cout << "Clock of A:\n";

        int data;
                
        // Read all the port in order 
        // and break the loop if there is no message to read.
        while( true)
        {
                if( _init->read( &data, cycle))
                {
                        cout << "\tread the init port: data = " << data << "\n";

                } else if ( _from_B->read( &data, cycle))
                {
                        cout << "\tread the port from B: data = " << data << "\n";

                } else
                {
                        cout << "\tnothing to read\n";
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
        _to_A = new WritePort<int> ( "B_to_A", PORT_BW, PORT_FANOUT);
        _from_A = new ReadPort<int> ( "A_to_B", PORT_LATENCY);
}

B::~B ()
{
        delete _to_A;
        delete _from_A; 
}
        
void B::processData ( int data, int cycle)
{       
        // perform calculation
        ++data;

        cout << "\t\tProcess data: new value = " 
                        << data << "\n" << "\t\t\tsend it to A\n";

        _to_A->write( data, cycle);
}

void B::clock ( int cycle)
{
        cout << "Clock of B:\n";

        int data;
                
        if ( _from_A->read( &data, cycle))
        {
                cout << "\tread the port from A: data = " << data << "\n";
                this->processData( data, cycle);

        } else
        {
                cout << "\tnothing to read\n";
        }
}