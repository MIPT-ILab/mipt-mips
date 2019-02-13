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
    explicit Port( std::string key) : Log( false), _key( std::move( key)) { }
};

class BasicWritePort : public Port
{
    friend class PortMap;
    const uint32 _fanout;
    Cycle _lastCycle = 0_cl;
    uint32 _writeCounter = 0;
protected:
    uint32 initialized_bandwidth = 0;
    const uint32 installed_bandwidth;
    BasicWritePort( const std::string& key, uint32 bandwidth, uint32 fanout);
    virtual void init( const std::vector<Port*>& readers) = 0;
    virtual void destroy() = 0;
    virtual void clean_up( Cycle cycle) = 0;
    void check_init( const std::vector<Port*>& readers) const;
    void prepare_to_write( Cycle cycle);
public:
    auto get_fanout() const { return _fanout; }
    auto get_bandwidth() const { return initialized_bandwidth; }
};

// Make it inline since it is used often
inline void BasicWritePort::prepare_to_write( Cycle cycle)
{
    _writeCounter = _lastCycle == cycle ? _writeCounter + 1 : 0;
    _lastCycle = cycle;

    if ( _writeCounter > initialized_bandwidth)
        throw PortError(_key + " port is overloaded by bandwidth");
}

template<class T> class ReadPort;
    
template<class T> class WritePort : public BasicWritePort
{
    using Log::serr;
    std::vector<ReadPort<T>*> _destinations = {};
    void init( const std::vector<Port*>& readers) final;
    void clean_up( Cycle cycle) final;
    void destroy() final;
    ReadPort<T>* port_cast( Port* p) const;
    void basic_write( T&& what, Cycle cycle) noexcept( std::is_nothrow_copy_constructible<T>::value);
public:
    WritePort<T>( const std::string& key, uint32 bandwidth, uint32 fanout)
        : BasicWritePort( key, bandwidth, fanout)
    { }

    void write( T&& what, Cycle cycle)
    {
        prepare_to_write( cycle);
        basic_write( std::forward<T>( what), cycle);
    }

    void write( const T& what, Cycle cycle)
    {
        prepare_to_write( cycle);
        basic_write( std::move( T( what)), cycle);
    }
};

template<class T> class ReadPort : public Port
{
    friend class WritePort<T>;
private:
    using Log::sout;
    using Log::serr;

    const Latency _latency;
    PortQueue<std::pair<T, Cycle>> queue;

    void emplaceData( T&& what, Cycle cycle)
        noexcept( noexcept( queue.emplace( std::declval<T>(), cycle)))
    {
        queue.emplace( std::move( what), cycle + _latency);
    }

    void init( uint32 bandwidth)
    {
        // +1 to handle reads-after-writes
        queue.resize( ( _latency.to_size_t() + 1) * bandwidth);
    }
    void clean_up( Cycle cycle) noexcept;
public:
    ReadPort<T>( const std::string& key, Latency latency) :
        Port( key), _latency( latency), queue()
    {
        portMap[ _key].readers.push_back( this);
    }

    bool is_ready( Cycle cycle) const noexcept
    {
        return !queue.empty() && std::get<Cycle>(queue.front()) == cycle;
    }

    T read( Cycle cycle);
};

template<class T>
void WritePort<T>::clean_up( Cycle cycle)
{
    for ( const auto& reader : _destinations)
        reader->clean_up( cycle);
}

template<class T>
void WritePort<T>::basic_write( T&& what, Cycle cycle)
    noexcept( std::is_nothrow_copy_constructible<T>::value)
{
    // Copy data to all ports except first one
    auto it = std::next( _destinations.begin());
    for ( ; it != _destinations.end(); ++it)
        (*it)->emplaceData( std::move( T( what)), cycle); // Force copy ctor

    // Move data to the first port
    _destinations.front()->emplaceData( std::forward<T>( what), cycle);
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

    initialized_bandwidth = installed_bandwidth;
    for ( const auto& reader : _destinations)
        reader->init( get_bandwidth());
}

template<class T> void WritePort<T>::destroy()
{
    _destinations.clear();
    initialized_bandwidth = 0;
}

template<class T> T ReadPort<T>::read( Cycle cycle)
{
    if ( !is_ready( cycle))
        throw PortError( _key + " ReadPort was not ready for read at cycle=" + cycle.to_string());

    T tmp( std::move( std::get<T>(queue.front())));
    queue.pop();
    return tmp;
}

template<class T> void ReadPort<T>::clean_up( Cycle cycle) noexcept
{
    while ( !queue.empty() && std::get<Cycle>(queue.front()) < cycle)
        queue.pop();
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
