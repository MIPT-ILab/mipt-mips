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

#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

/*
 * Known bugs: it is possible to create a pair of ports with the same name
 * but different type
 */

template<class T> class ReadPort;
template<class T> class WritePort;

struct PortError : std::runtime_error {
    explicit PortError( const std::string& msg)
        : std::runtime_error(std::string("Port error: ") + msg + '\n')
    { }
};

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

                static std::vector<BaseMap*> all_maps;
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
        /*
         * Map of ports
        */
        class Map : public BasePort::BaseMap
        {
        private:
        // Cluster of portMap — one writer and list of readers
            struct Cluster
            {
                WritePort<T>* writer = nullptr;
                std::vector<ReadPort<T>*> readers = {};
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

        // Number of tokens that can be added in one cycle;
        const uint32 _bandwidth;

        // Number of reader that can read from this port
        const uint32 _fanout;

        // List of readers
        std::vector<ReadPort<T>*> _destinations = {};

        // Variables for counting token in the last cycle
        Cycle _lastCycle = 0_cl;
        uint32 _writeCounter = 0;

        void init( std::vector<ReadPort<T>*> readers);

        void clean_up( Cycle cycle) {
            for ( const auto& reader : _destinations)
                reader->clean_up( cycle);
        }

        void prepare_to_write( Cycle cycle);

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
        template<typename U>
        void write( U&& what, Cycle cycle);

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

        // Latency is the number of cycles after which we may take data from port.
        const Latency _latency;

        // Queue of data that should be released
        struct Cell
        {
            T data = T();
            Cycle cycle = 0_cl;
            Cell() = delete;

            template<typename U>
            Cell( U&& v, Cycle c) : data( std::forward<U>( v)), cycle( c) { }
        };
        std::queue<Cell> _dataQueue;

        // Pushes data from WritePort
        template<typename U>
        void emplaceData( U&& what, Cycle cycle)
        {
            _dataQueue.emplace( std::forward<U>( what), cycle + _latency);
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
            this->portMap[ this->_key].readers.push_back( this);
        }

        // Is ready? method
        bool is_ready( Cycle cycle) const;

        // Read method
        T read( Cycle cycle);
};

/*
 * If port wasn't initialized, asserts.
 * If port is overloaded by bandwidth (more than _bandwidth token during one cycle, asserts).
*/
template<class T> void WritePort<T>::prepare_to_write( Cycle cycle)
{
    if ( !this->_init)
        throw PortError(this->_key + " WritePort was not initializated");

    if ( _lastCycle != cycle)
        _writeCounter = 0;

    _lastCycle = cycle;

    if ( _writeCounter >= _bandwidth)
        throw PortError(this->_key + " port is overloaded by bandwidth");

    // If we can add something more on that cycle, forwarding it to all ReadPorts.
    _writeCounter++;
}

/*
 * Write method.
 *
 * First argument is data itself.
 * Second argument is the current cycle number.
 *
 * Forwards data to all connected ReadPorts
*/
template<class T> template<typename U>
void WritePort<T>::write( U&& what, Cycle cycle)
{
    static_assert( std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>,
                                  std::remove_cv_t<std::remove_reference_t<U>>
                   >,"Type mismatch between WritePort type and pushed value");
    prepare_to_write( cycle);

    // Copy data to all ports except first one
    auto it = std::next( this->_destinations.begin());
    for ( ; it != this->_destinations.end(); ++it) {
        const T& ref = what; // Force copy ctor
        (*it)->emplaceData( ref, cycle);
    }

    // Move data to the first port
    this->_destinations.front()->emplaceData( std::forward<U>( what), cycle);
}

/*
 * Initialize cluster of ports.
 *
 * If there're any unconnected ports or fanout overload, asserts.
 * If ther's fanout underload, warnings.
*/
template<class T> void WritePort<T>::init( std::vector<ReadPort<T>*> readers)
{
    _destinations = std::move( readers);
    this->_init = true;

    // Initializing ports with setting their init flags.
    uint32 readersCounter = _destinations.size();
    for ( const auto reader : _destinations)
        reader->_init = true;

    if ( readersCounter == 0)
        throw PortError( this->_key + " has no ReadPorts");
    if ( readersCounter > _fanout)
        throw PortError( this->_key + " WritePort is overloaded by fanout");
    if ( readersCounter != _fanout)
        throw PortError( this->_key + " WritePort is underloaded by fanout");
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
        return;

    this->_init = false;

    for ( const auto reader : _destinations)
         reader->_init = false;

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
    if ( !is_ready( cycle))
        throw PortError( this->_key + " ReadPort was not ready for read at cycle=" + cycle.to_string());

    // data is successfully read
    T tmp = std::move( _dataQueue.front().data);
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
        if (writer == nullptr)
            throw PortError( cluster.first + " has no WritePort");

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

static constexpr const Latency PORT_LATENCY = 1_lt;
static constexpr const Latency PORT_LONG_LATENCY = 30_lt;
static constexpr const uint32 PORT_FANOUT = 1;
static constexpr const uint32 PORT_BW = 1;

#endif // PORTS_H

