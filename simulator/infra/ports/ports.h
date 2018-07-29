/**
 * ports.h - template for simulation of ports.
 * @author Pavel Kryukov
 * Copyright 2017-2018 MIPT-MIPS team
 */

#ifndef PORTS_H
#define PORTS_H

#include "../log.h"
#include "../types.h"
#include "timing.h"

#include <list>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>

/*
 * Known bugs: it is possible to create a pair of ports with the same name
 * but different type
 */

template<class T> class ReadPort;
template<class T> class WritePort;

// Global port handlers
extern void init_ports();
extern void clean_up_ports( Cycle cycle);
extern void destroy_ports();

class BasePort : protected Log
{
        friend void init_ports();
        friend void clean_up_ports( Cycle cycle);
        friend void destroy_ports();

    protected:
        class BaseMap : public Log
        {
                friend void init_ports();
                friend void clean_up_ports( Cycle cycle);
                friend void destroy_ports();

                virtual void init() const = 0;
                virtual void clean_up( Cycle cycle) = 0;
                virtual void destroy() = 0;

                static std::list<BaseMap*> all_maps;
            protected:
                BaseMap() : Log( false) { all_maps.push_back( this); }
        };

        // Key of port
        const std::string _key;

        // Init flag
        bool _init = false;

        // Constructor of port
        explicit BasePort( std::string key) : Log( false), _key( std::move( key)) { }
};

/*
 * Port class
*/
template<class T> class Port : public BasePort
{
    protected:
        using ReadListType = std::list<ReadPort<T>* >;

        /*
         * Map of ports
        */
    protected:
        class Map : public BasePort::BaseMap
        {
        private:
        // Cluster of portMap — one writer and list of readers
            struct Cluster
            {
                WritePort<T>* writer = nullptr;
                ReadListType readers = {};
            };

            std::unordered_map<std::string, Cluster> _map = { };

            // Init method
            void init() const final;

            // Finding lost elements
            void clean_up( Cycle cycle) final;

            // Destroy connections
            void destroy() final;

            // Constructors
            Map() noexcept : BaseMap() { }
        public:
            decltype(auto) operator[]( const std::string& v) { return _map.operator[]( v); }

            // Singletone
            static Map& get_instance()
            {
                static Map instance;
                return instance;
            }
        };

        explicit Port( std::string key) : BasePort( std::move( key)) { }

        // ports Map to connect ports between for themselves;
        Map& portMap = Map::get_instance();
};

/*
 * WritePort
 */
template<class T> class WritePort : public Port<T>
{
        friend class Port<T>::Map;

        using Log::serr;
        using Log::critical;
        using ReadListType = typename Port<T>::ReadListType;

        // Number of tokens that can be added in one cycle;
        const uint32 _bandwidth;

        // Number of reader that can read from this port
        const uint32 _fanout;

        // List of readers
        ReadListType _destinations = {};

        // Variables for counting token in the last cycle
        Cycle _lastCycle = 0_Cl;
        uint32 _writeCounter = 0;

        void init( const ReadListType& readers);

        void clean_up( Cycle cycle) {
            for ( const auto& reader : _destinations)
                reader->clean_up( cycle);
        }

        // destroy all ports
        void destroy();
    public:
        /*
         * Constructor
         *
         * First argument is key which is used to connect ports.
         * Second is the bandwidth of port (how much data items can port get during one cycle).
         * Third is maximum number of ReadPorts.
         *
         * Adds port to needed Map.
        */
        WritePort<T>( std::string key, uint32 bandwidth, uint32 fanout) :
            Port<T>::Port( std::move( key)), _bandwidth(bandwidth), _fanout(fanout)
        {
            if ( this->portMap[ this->_key].writer != nullptr)
                serr << "Reusing of " << key
                     << " key for WritePort. Last WritePort will be used." << std::endl;

            this->portMap[ this->_key].writer = this;
        }

        // Write Method
        void write( const T& what, Cycle cycle);

        // Returns fanout for test of connection
        uint32 getFanout() const { return _fanout; }
};

/*
 * Read Port
*/
template<class T> class ReadPort: public Port<T>
{
        friend class WritePort<T>;
    private:
        using Log::sout;
        using Log::serr;
        using Log::critical;

        // Latency is the number of cycles after which we may take data from port.
        const Latency _latency;

        // Queue of data that should be released
        struct Cell
        {
            T data = T();
            Cycle cycle = 0_Cl;
            Cell() = delete;
            Cell( T v, Cycle c) : data( std::move( v)), cycle( c) { }
        };
        std::queue<Cell> _dataQueue;

        // Pushes data from WritePort
        void pushData( const T& what, Cycle cycle)
        {
             _dataQueue.emplace( what, cycle + _latency); // NOTE: we copy data here
        }

        // Tests if there is any ungot data
        void clean_up( Cycle cycle);
    public:
        /*
         * Constructor
         *
         * First argument is the connection key.
         * Second argument is the latency of port.
         *
         * Adds port to needed Map.
        */
        ReadPort<T>( std::string key, Latency latency) :
            Port<T>::Port( std::move( key)), _latency( latency), _dataQueue()
        {
            this->portMap[ this->_key].readers.push_front( this);
        }

        // Is ready? method
        bool is_ready( Cycle cycle) const;

        // Read method
        T read( Cycle cycle);
};

/*
 * Write method.
 *
 * First argument is data itself.
 * Second argument is the current cycle number.
 *
 * Forwards data to all connected ReadPorts
 *
 * If port wasn't initialized, asserts.
 * If port is overloaded by bandwidth (more than _bandwidth token during one cycle, asserts).
*/
template<class T> void WritePort<T>::write( const T& what, Cycle cycle)
{
    if ( !this->_init)
    {
    // If no init, asserts
        serr << this->_key << " WritePort was not initializated" << std::endl << critical;
        return;
    }
    if ( _lastCycle != cycle)
    {
        // If cycle number was changed, zero counter.
        _lastCycle = cycle;
        _writeCounter = 0;
    }
    if ( _writeCounter < _bandwidth)
    {
    // If we can add something more on that cycle, forwarding it to all ReadPorts.
        _writeCounter++;
        for ( auto dst : this->_destinations)
            dst->pushData( what, cycle);
    }
    else
    {
    // If we overloaded port's bandwidth, assert
        serr << this->_key << " port is overloaded by bandwidth" << std::endl << critical;
    }
}

/*
 * Initialize cluster of ports.
 *
 * If there're any unconnected ports or fanout overload, asserts.
 * If ther's fanout underload, warnings.
*/
template<class T> void WritePort<T>::init( const ReadListType& readers)
{
    _destinations = readers;
    this->_init = true;

    // Initializing ports with setting their init flags.
    uint32 readersCounter = _destinations.size();
    for ( const auto reader : _destinations)
        reader->_init = true;

    if ( readersCounter == 0)
        serr << "No ReadPorts for " << this->_key << " key" << std::endl << critical;
    else if ( readersCounter > _fanout)
        serr << this->_key << " WritePort is overloaded by fanout" << std::endl << critical;
    else if ( readersCounter != _fanout)
        serr << this->_key << " WritePort is underloaded by fanout" << std::endl;
}

/*
 * Destroy map of ports.
 *
 * Iterates all map and de-initalizes all port trees
 * Then destroys content of map
*/
template<class T> void WritePort<T>::destroy()
{
    if ( !this->_init)
        serr << "Destroying uninitialized WritePort " << this->_key << std::endl << critical;

    this->_init = false;

    for ( const auto reader : _destinations)
    {
        if ( !reader->_init)
            serr << "Destroying uninitialized ReadPort " << this->_key << std::endl << critical;

        reader->_init = false;
    }
    _destinations.clear();
}

/*
 * Ready method
 *
 * Checks if there is something to read in this cycle
 *
 * If there's nothing in port to give, returns false
 * If succesful, returns true
 * If uninitalized, generates error
*/
template<class T> bool ReadPort<T>::is_ready( Cycle cycle) const
{
    if ( !this->_init)
    {
        serr << this->_key << " ReadPort was not initializated" << std::endl << critical;
        return false;
    }

    // there are some entries and they are ready to be read
    return !_dataQueue.empty() && _dataQueue.front().cycle == cycle;
}

/*
 * Read method
 *
 * Returns data which should be read in this cycle
 *
 * If there was nothing to read, generates error (use is_ready before reading)
*/
template<class T> T ReadPort<T>::read( Cycle cycle)
{
    if ( !this->_init)
        serr << this->_key << " ReadPort was not initializated" << std::endl << critical;

    if ( _dataQueue.empty() || _dataQueue.front().cycle != cycle)
        serr << this->_key << " ReadPort was not ready for read at cycle=" << cycle << std::endl << critical;

    // data is successfully read
    auto tmp = _dataQueue.front().data; // NOTE: we copy data here
    _dataQueue.pop();
    return tmp;
}

/*
 * Tests if there is any data that can not be used ever.
*/
template<class T> void ReadPort<T>::clean_up( Cycle cycle)
{
    while ( !_dataQueue.empty() && _dataQueue.front().cycle < cycle) {
        sout << "In " << this->_key << " port data was added at "
             << (_dataQueue.front().cycle - _latency)
             << " clock and was not readed\n";
        _dataQueue.pop();
    }
}

/*
 * Initialize map of ports.
 *
 * Iterates all map and initalizes all port trees
*/
template<class T> void Port<T>::Map::init() const
{
    for ( const auto& cluster : _map)
    {
        auto writer = cluster.second.writer;
        if ( writer == nullptr)
            serr << "No WritePort for " << cluster.first << " key" << std::endl << critical;

        writer->init( cluster.second.readers);
    }
}

/*
 * Destroy map of ports.
 *
 * Iterates all map and de-initalizes all port trees
 * Then destroys content of map
*/
template<class T> void Port<T>::Map::destroy()
{
    for ( const auto& cluster : _map)
        cluster.second.writer->destroy();

    _map.clear();
}

/*
 * Find lost elements inside port
 *
 * Argument is the number of current cycle.
 * If some token couldn't be get in future, warnings
*/
template<class T> void Port<T>::Map::clean_up( Cycle cycle)
{
    for ( const auto& cluster : _map)
        cluster.second.writer->clean_up( cycle);
}

// External methods
template<typename T, typename... Args>
decltype(auto) make_write_port(Args... args)
{
    return std::make_unique<WritePort<T>>(args...);
}

template<typename T, typename... Args>
decltype(auto) make_read_port(Args... args)
{
    return std::make_unique<ReadPort<T>>(args...);
}

static constexpr const Latency PORT_LATENCY = 1_Lt;
static constexpr const Latency PORT_LONG_LATENCY = 30_Lt;
static constexpr const uint32 PORT_FANOUT = 1;
static constexpr const uint32 PORT_BW = 1;

#endif // PORTS_H

