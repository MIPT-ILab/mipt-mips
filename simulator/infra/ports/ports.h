/**
 * ports.h - template for simulation of ports.
 * @author Pavel Kryukov
 * Copyright 2017 MIPT-MIPS team
 */

#ifndef PORTS_H
#define PORTS_H

#include <map>
#include <queue>
#include <list>
#include <string>
#include <memory>

#include "../types.h"
#include "../log.h"

/*
 * Known bugs: it is possible to create a pair of ports with the same name
 * but different type
 */

template<class T> class ReadPort;
template<class T> class WritePort;

// Global port handlers
extern void init_ports();
extern void check_ports( uint64 cycle);
extern void destroy_ports();

class BasePort : protected Log
{
        friend void init_ports();
        friend void check_ports( uint64 cycle);
        friend void destroy_ports();

    protected:
        class BaseMap : public Log
        {
                friend void init_ports();
                friend void check_ports( uint64 cycle);
                friend void destroy_ports();

                virtual void init() const = 0;
                virtual void check( uint64 cycle) const = 0;
                virtual void destroy() = 0;

                static std::list<BaseMap*> all_maps;
            protected:
                BaseMap() : Log(true) { all_maps.push_back( this); }
                ~BaseMap() override = default;
        };

        // Key of port
        const std::string _key;

        // Init flag
        bool _init = false;

        // Constructor of port
        explicit BasePort( const std::string& key) : Log( true), _key( key) { }

        ~BasePort() override = default;
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

            std::map<std::string, Cluster> _map = { };

            // Init method
            void init() const final;

            // Finding lost elements
            void check( uint64 cycle) const final;

            // Destroy connections
            void destroy() final;

            // Constructors
            Map() noexcept : BaseMap() { }
            ~Map() final = default;

        public:
            decltype(auto) operator[]( const std::string& v) { return _map.operator[]( v); }

            // Singletone
            static Map& get_instance()
            {
                static Map instance;
                return instance;
            }
        };

        explicit Port( const std::string& key) : BasePort( key) { }

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
        uint32 _lastCycle = 0;
        uint32 _writeCounter = 0;

        void init( const ReadListType& readers);

        void check( uint64 cycle) const {
            for ( const auto& reader : _destinations)
                reader->check( cycle);
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
        WritePort<T>( const std::string& key, uint32 bandwidth, uint32 fanout) :
            Port<T>::Port( key), _bandwidth(bandwidth), _fanout(fanout)
        {
            if ( this->portMap[ key].writer != nullptr)
                serr << "Reusing of " << key
                     << " key for WritePort. Last WritePort will be used." << std::endl;

            this->portMap[ key].writer = this;
        }

        // Write Method
        void write( const T& what, uint64 cycle);

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
        using Log::serr;
        using Log::critical;

        // Latency is the number of cycles after which we may take data from port.
        const uint64 _latency;

        // Queue of data that should be released
        struct Cell
        {
            T data = T();
            uint64 cycle = 0;
            Cell() = delete;
            Cell( const T& v, uint64 c) : data( v), cycle( c) { }
        };
        std::queue<Cell> _dataQueue;

        // Pushes data from WritePort
        void pushData( const T& what, uint64 cycle)
        {
             _dataQueue.emplace(what, cycle + _latency); // NOTE: we copy data here
        }

        // Tests if there is any ungot data
        void check( uint64 cycle) const;
    public:
        /*
         * Constructor
         *
         * First argument is the connection key.
         * Second argument is the latency of port.
         *
         * Adds port to needed Map.
        */
        ReadPort<T>( const std::string& key, uint64 latency) :
            Port<T>::Port( key), _latency( latency), _dataQueue()
        {
            this->portMap[ key].readers.push_front( this);
        }

        // Read method
        bool read( T* address, uint64 cycle);
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
template<class T> void WritePort<T>::write( const T& what, uint64 cycle)
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
        {
            dst->pushData(what, cycle);
        }
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
    uint readersCounter = 0;
    for ( const auto reader : _destinations)
    {
        reader->_init = true;
        readersCounter++;
    }

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
    if ( this->_init == false)
        serr << "Destroying uninitialized WritePort " << this->_key << std::endl << critical;

    this->_init = false;

    for ( const auto reader : _destinations)
    {
        if ( reader->_init == false) 
            serr << "Destroying uninitialized ReadPort " << this->_key << std::endl << critical;

        reader->_init = false;
    }
    _destinations.clear();
}

/*
 * Read method
 *
 * First arguments is address, second is the number of cycle
 *
 * If there's nothing in port to give, returns false
 * If succesful, returns true
 * If uninitalized, asserts
*/
template<class T> bool ReadPort<T>::read( T* address, uint64 cycle)
{
    if ( !this->_init)
    {
        serr << this->_key << " ReadPort was not initializated" << std::endl << critical;
        return false;
    }

    if ( _dataQueue.empty())
        return false; // the port is empty

    if ( _dataQueue.front().cycle == cycle)
    {
        // data is successfully read
        *address = _dataQueue.front().data; // NOTE: we copy data here
        _dataQueue.pop();
        return true;
    }

    // there are some entries, but they are not ready to read
    return false;
}

/*
 * Tests if there is any data that can not be used ever.
*/
template<class T> void ReadPort<T>::check(uint64 cycle) const
{
    if ( !_dataQueue.empty() && _dataQueue.front().cycle < cycle)
    {
        serr << "In " << this->_key << " port data was added at "
             << (_dataQueue.front().cycle - _latency)
             << " clock and will not be readed" << std::endl << critical;
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
 * Function for founding lost elements at port
 *
 * Argument is the number of current cycle.
 * If some token couldn't be get in future, warnings
*/
template<class T> void Port<T>::Map::check( uint64 cycle) const
{
    for ( const auto& cluster : _map)
        cluster.second.writer->check(cycle);
}

// External methods
template<typename T, typename ... Args>
decltype(auto) make_write_port(Args ... args)
{
    return std::make_unique<WritePort<T>>(args...);
}

template<typename T, typename ... Args>
decltype(auto) make_read_port(Args ... args)
{
    return std::make_unique<ReadPort<T>>(args...);
}

#endif // PORTS_H

