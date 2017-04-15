/**
 * ports.h - template for simulation of ports.
 * @author Pavel Kryukov
 * Copyright 2011 MDSP team
 */

#ifndef PORTS_H
#define PORTS_H

#include <map>
#include <queue>
#include <list>
#include <string>

#include <common/types.h>
#include <common/log.h>

template<class T> class PortMap;
template<class T> class ReadPort;
template<class T> class WritePort;

/*
 * Port class
*/
template<class T> class Port
{
    protected:
        // Key of port
        std::string _key;

        // Init flag
        bool _init;
    public:
        // Static ports Map to connect ports between for themselves;
        static PortMap<T>* portMap;

        // Call of initialization of portMap
        static inline void init()
        {
            portMap->init();
        };

        // Call of finding lost tokens in ports
        static inline void lost( uint32 cycle)
        {
            portMap->lost( cycle);
        };

        // Sets init flag as true.
        void setInit();

        // Constructor of port
        Port<T>( const std::string&);
};

/*
 * Init a map of ports (see bottom of file)
*/
template<class T> PortMap<T>* Port<T>::portMap = new PortMap<T>;

/*
 * Constructor
*/
template<class T> Port<T>::Port( const std::string& key)
{
    _key = key;
    _init = false;
}

/*
 * Setting init flag as true.
*/
template<class T> void Port<T>::setInit()
{
    _init = true;
}

/*
 * WritePort
 */
template<class T> class WritePort: public Port<T>, private Log
{
    private:
        // Number of tokens that can be added in one cycle;
        uint32 _bandwidth;

        // Number of reader that can read from this port
        uint32 _fanout;

        // List of readers
        using ReadListType = std::list<ReadPort<T>* >;
        ReadListType _destinations;

        // Variables for counting token in the last cycle
        uint32 _lastCycle;
        uint32 _writeCounter;

    public:
        // Constructor
        WritePort<T>( const std::string&, uint32, uint32);

        // Write Method
        void write( T, uint64);

        // Addes destination ReadPort to list
        void setDestination( const ReadListType&);

        // Returns fanout for test of connection
        uint32 getFanout() const;
};

/*
 * Constructor
 *
 * First argument is key which is used to connect ports.
 * Second is the bandwidth of port (how much data items can port get during one cycle).
 * Third is maximum number of ReadPorts.
 *
 * Adds port to needed PortMap.
*/
template<class T> WritePort<T>::WritePort( const std::string& key, uint32 bandwidth, uint32 fanout):
    Port<T>::Port( key), Log(true)
{
    _bandwidth = bandwidth;
    _fanout = fanout;

    _lastCycle = 0;
    _writeCounter = 0;

    this->portMap->addWritePort( this->_key, this);
}

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
template<class T> void WritePort<T>::write( T what, uint64 cycle)
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
        for ( auto* dst : this->_destinations)
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
 * Shows to WritePort list of his ReadPorts (from PortMap)
*/
template<class T> void WritePort<T>::setDestination(const ReadListType& source)
{
    _destinations.clear();
    _destinations.insert(_destinations.end(), source.begin(), source.end());
}

/*
 * Returns fanout
*/
template<class T> uint32 WritePort<T>::getFanout() const
{
    return _fanout;
}

/*
 * Read Port
*/
template<class T> class ReadPort: public Port<T>, private Log
{
    private:
        // Latency is the number of cycles after which we may take data from port.
        uint64 _latency;

        // Queue of data that should be released
        struct DataCage
        {
            T data;
            uint64 cycle;
        };
        typedef std::queue<DataCage> DataQueue;
        DataQueue _dataQueue;

    public:
        // Constructor
        ReadPort<T>( const std::string&, uint64);

        // Read method
        bool read( T*, uint64);

        // Pushes data from WritePort
        void pushData( T, uint64);

        // Tests if there is any ungot data
        bool selfTest( uint64, uint64*) const;
};

/*
 * Constructor
 *
 * First argument is the connection key.
 * Second argument is the latency of port.
 *
 * Adds port to needed PortMap.
*/
template<class T> ReadPort<T>::ReadPort( const std::string& key, uint64 latency):
    Port<T>::Port( key), Log(true)
{
    _latency = latency;

    this->portMap->addReadPort( this->_key, this);
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

    if ( _dataQueue.empty()) return false; // the port is empty

    if ( _dataQueue.front().cycle == cycle)
    {
		// data is successfully read
        *address = _dataQueue.front().data;
        _dataQueue.pop();
        return true;
    }
    else
    {
        // there are some entries, but they are not ready to read
		return false;
    }
}

/*
 * Receive data from WritePort.
*/
template<class T> void ReadPort<T>::pushData( T what, uint64 cycle)
{
    DataCage buffer;
    buffer.data = what;
    buffer.cycle = cycle + _latency;
    _dataQueue.push( buffer);
}

/*
 * Tests if there is any data that can not be used ever.
 *
 * Returns cycle number when data was added to WritePort.
*/
template<class T> bool ReadPort<T>::selfTest(uint64 cycle, uint64* wantedCycle) const
{
    if ( _dataQueue.empty()) return true;
    if ( _dataQueue.front().cycle < cycle)
    {
        *wantedCycle = _dataQueue.front().cycle - _latency;
        return false;
    }
    return true;
}

/*
 * Map of ports
*/
template<class T> class PortMap: public Log
{
    private:
        using ReadListType = std::list<ReadPort<T>* >;

        // Entry of portMap — one writer and list of readers
        struct Entry
        {
            WritePort<T>* writer;
            ReadListType readers;
        };

        // Type of map of Entry
        using MapType = std::map<std::string, Entry>;

        // Map itself
        MapType _map;
    public:
        // Adding methods
        void addWritePort( const std::string&, WritePort<T>*);
        void addReadPort( const std::string&, ReadPort<T>*);

        // Init method
        void init();

        // Finding lost elements
        void lost( uint64);

        PortMap();
};


template<class T> PortMap<T>::PortMap() : Log(true) { }

/*
 * Adding WritePort to the map.
*/
template<class T> void PortMap<T>::addWritePort( const std::string& key, WritePort<T>* pointer)
{
    if ( _map.find( key) == _map.end())
    {
    // If there's no record in this map, create it.
        Entry entry;
        entry.writer = 0;
        entry.readers.resize(0);
        _map.insert( std::pair<std::string, Entry>( key, entry));
    }
    else
    {
        if ( _map[key].writer)
        {
        // Warning of double using of same key for WritePort
            serr << "Reusing of " << key << " key for WritePort. Last WritePort will be used." << std::endl;
        }
    }
    _map[key].writer = pointer;
}

/*
 * Adding ReadPort to the map.
*/
template<class T> void PortMap<T>::addReadPort( const std::string& key, ReadPort<T>* pointer)
{
    if ( _map.find(key) == _map.end())
    {
    // If there's no record in this map, create it.
        Entry entry;
        entry.writer = 0;
        entry.readers.resize(0);
        _map.insert( std::pair<std::string, Entry>( key, entry));
    }
    _map[key].readers.push_front( pointer);
}

/*
 * Initialize map of ports.
 *
 * Iterates all map and sets destination list to all writePorts
 * If there're any unconnected ports or fanout overload, asserts.
 * If ther's fanout underload, warnings.
*/
template<class T> void PortMap<T>::init()
{
    for ( auto& entry : _map)
    {
        if ( !entry.second.writer)
        {
            serr << "No WritePort for " << entry.first << " key" << std::endl << critical;
        }

        WritePort<T>* writer = entry.second.writer;
        uint32 readersCounter = entry.second.readers.size();
        if ( !readersCounter)
        {
            serr << "No ReadPorts for " << entry.first << " key" << std::endl << critical;
            return;
        }
        if ( readersCounter > writer->getFanout())
        {
            serr << entry.first << " WritePort is overloaded by fanout" << std::endl << critical;
            return;
        }
        if ( readersCounter < writer->getFanout())
        {
            serr << entry.first << " WritePort is underloaded by fanout" << std::endl;
        }
        writer->setDestination( entry.second.readers);

        // Initializing ports with setting their init flags.
        for ( ReadPort<T>*& reader : entry.second.readers)
        {
            reader->setInit();
        }
        writer->setInit();
    }
}

/*
 * Function for founding lost elements at port
 *
 * Argument is the number of current cycle.
 * If some token couldn't be get in future, warnings
*/
template<class T> void PortMap<T>::lost( uint64 cycle)
{
    for ( auto& entry : _map)
    {
        for ( auto& reader : entry.second.readers)
        {
            uint64 addCycle;
            if ( !reader->selfTest( cycle, &addCycle))
            {
                serr << "In " << entry.first << " port data was added at " << addCycle
                     << " clock and will not be readed" << std::end;
            }
        }
    }
}

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
