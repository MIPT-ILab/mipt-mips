/**
 * ports.h - template for simulation of ports.
 * @author Pavel Kryukov
 * Copyright 2017-2018 MIPT-MIPS team
 */

#ifndef PORTS_H
#define PORTS_H

#include "../exception.h"
#include "../log.h"
#include "../types.h"
#include "port_queue/port_queue.h"
#include "timing.h"

#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

struct PortError final : Exception {
    explicit PortError( const std::string& msg)
        : Exception("Port error", msg)
    { }
};

class PortMap : public Log
{
private:
    struct Cluster
    {
        class BasicWritePort* writer = nullptr;
        std::vector<class Port*> readers = {};
    };

    std::unordered_map<std::string, Cluster> _map = { };

    PortMap() noexcept : Log( false) { }
public:
    decltype(auto) operator[]( const std::string& v) { return _map.operator[]( v); }

    static PortMap& get_instance()
    {
        static PortMap instance;
        return instance;
    }

    void init() const;
    void clean_up( Cycle cycle);
    void destroy();
};

class Port : public Log
{
protected:
    PortMap& portMap = PortMap::get_instance();
    const std::string _key;
    bool _init = false;
    explicit Port( std::string key) : Log( false), _key( std::move( key)) { }
};

class BasicWritePort : public Port
{
    friend class PortMap;
    const uint32 _bandwidth;
    const uint32 _fanout;
    Cycle _lastCycle = 0_cl;
    uint32 _writeCounter = 0;
protected:
    BasicWritePort( const std::string& key, uint32 bandwidth, uint32 fanout);
    virtual void init( const std::vector<Port*>& readers) = 0;
    virtual void destroy() = 0;
    virtual void clean_up( Cycle cycle) = 0;
    void check_init( const std::vector<Port*>& readers) const;
    void prepare_to_write( Cycle cycle);
public:
    auto get_fanout() const { return _fanout; }
    auto get_bandwidth() const { return _bandwidth; }
};

template<class T> class ReadPort;
    
template<class T> class WritePort : public BasicWritePort
{
    using Log::serr;
    std::vector<ReadPort<T>*> _destinations = {};
    void init( const std::vector<Port*>& readers) final;
    void clean_up( Cycle cycle) final;
    void destroy() final;
    ReadPort<T>* port_cast( Port* p) const;
public:
    WritePort<T>( const std::string& key, uint32 bandwidth, uint32 fanout) : BasicWritePort( key, bandwidth, fanout) { }
    void write( T&& what, Cycle cycle);
    void write( const T& what, Cycle cycle);
};

template<class T> class ReadPort : public Port
{
    friend class WritePort<T>;
private:
    using Log::sout;
    using Log::serr;

    const Latency _latency;
    PortQueue<std::pair<T, Cycle>> _dataQueue;

    void emplaceData( T&& what, Cycle cycle);
    void clean_up( Cycle cycle);
    void init( uint32 bandwidth);
public:
    ReadPort<T>( const std::string& key, Latency latency) :
        Port( key), _latency( latency), _dataQueue()
    {
        portMap[ _key].readers.push_back( this);
    }

    bool is_ready( Cycle cycle) const;
    T read( Cycle cycle);
};

template<class T>
void WritePort<T>::clean_up( Cycle cycle)
{
    for ( const auto& reader : _destinations)
        reader->clean_up( cycle);
}

template<class T>
void WritePort<T>::write( T&& what, Cycle cycle)
{
    prepare_to_write( cycle);

    // Copy data to all ports except first one
    auto it = std::next( _destinations.begin());
    for ( ; it != _destinations.end(); ++it)
        (*it)->emplaceData( std::move( T( what)), cycle); // Force copy ctor

    // Move data to the first port
    _destinations.front()->emplaceData( std::forward<T>( what), cycle);
}

template<class T>
void WritePort<T>::write( const T& what, Cycle cycle)
{
    write( std::move( T( what)), cycle);
}

template<class T>
ReadPort<T>* WritePort<T>::port_cast( Port* p) const try
{
    return dynamic_cast<ReadPort<T>*>( p);
}
catch ( const std::bad_cast&)
{
    throw PortError(_key + " has type mismatch between write and read ports");
}

template<class T>
void WritePort<T>::init( const std::vector<Port*>& readers)
{
    check_init( readers);

    _destinations.reserve( readers.size());
    for (const auto& r : readers)
        _destinations.emplace_back( port_cast( r));

    _init = true;
    for ( const auto& reader : _destinations)
        reader->init( get_bandwidth());
}

template<class T> void WritePort<T>::destroy()
{
    if ( !_init)
        return;

    for ( const auto& reader : _destinations)
         reader->_init = false;

    _destinations.clear();
    _init = false;
}

template<class T>
void ReadPort<T>::emplaceData( T&& what, Cycle cycle)
{
    assert( !_dataQueue.full());
    _dataQueue.emplace( std::move( what), cycle + _latency);
}

template<class T> bool ReadPort<T>::is_ready( Cycle cycle) const
{
    return !_dataQueue.empty() && std::get<Cycle>(_dataQueue.front()) == cycle;
}

template<class T> void ReadPort<T>::init( uint32 bandwidth)
{
    // +1 to handle reads-after-writes
    _init = true;
    _dataQueue.resize( ( _latency.to_size_t() + 1) * bandwidth);
}

template<class T> T ReadPort<T>::read( Cycle cycle)
{
    if ( !is_ready( cycle))
        throw PortError( _key + " ReadPort was not ready for read at cycle=" + cycle.to_string());

    T tmp( std::move( std::get<T>(_dataQueue.front())));
    _dataQueue.pop();
    return tmp;
}

template<class T> void ReadPort<T>::clean_up( Cycle cycle)
{
    while ( !_dataQueue.empty() && std::get<Cycle>(_dataQueue.front()) < cycle)
        _dataQueue.pop();
}

// External methods
template<typename T>
decltype(auto) make_write_port( std::string key, uint32 bandwidth, uint32 fanout) 
{
    return std::make_unique<WritePort<T>>( std::move(key), bandwidth, fanout);
}

template<typename T>
auto make_read_port( std::string key, Latency latency)
{
    return std::make_unique<ReadPort<T>>( std::move(key), latency);
}

static constexpr const Latency PORT_LATENCY = 1_lt;
static constexpr const Latency PORT_LONG_LATENCY = 30_lt;
static constexpr const uint32 PORT_FANOUT = 1;
static constexpr const uint32 PORT_BW = 1;

#endif // PORTS_H
