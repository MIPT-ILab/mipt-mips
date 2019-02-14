/**
 * ports.h - template for simulation of ports.
 * @author Pavel Kryukov
 * Copyright 2017-2019 MIPT-MIPS team
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
public:
    static PortMap& get_instance();

    void init() const;
    void clean_up( Cycle cycle);
    void destroy();

private:
    PortMap() noexcept;

    friend class BasicWritePort;
    friend class BasicReadPort;
    void add_port( class BasicWritePort* port);
    void add_port( class BasicReadPort* port);

    struct Cluster
    {
        class BasicWritePort* writer = nullptr;
        std::vector<class BasicReadPort*> readers = {};
    };

    std::unordered_map<std::string, Cluster> map = { };
};

class Port : public Log
{
public:
    const std::string& get_key() const noexcept { return _key; }
    PortMap& get_port_map() const noexcept { return portMap; }

protected:
    explicit Port( std::string key);

private:
    const std::string _key;
    PortMap& portMap = PortMap::get_instance();
};

class BasicReadPort : public Port
{
public:
    auto get_latency() const noexcept { return _latency; }

protected:
    BasicReadPort( const std::string& key, Latency latency);

private:
    friend class BasicWritePort;
    virtual void init( uint32 bandwidth) = 0;
    const Latency _latency;
};

class BasicWritePort : public Port
{
public:
    auto get_fanout() const noexcept { return _fanout; }
    auto get_bandwidth() const noexcept { return initialized_bandwidth; }

protected:
    BasicWritePort( const std::string& key, uint32 bandwidth, uint32 fanout);
    void base_init( const std::vector<BasicReadPort*>& readers);

    void reset_write_counter() noexcept { write_counter = 0; }
    void increment_write_counter()
    {
        ++write_counter;
        if ( write_counter > get_bandwidth())
            throw PortError( get_key() + " port is overloaded by bandwidth");
    }

private:
    friend class PortMap;
    virtual void init( const std::vector<BasicReadPort*>& readers) = 0;
    virtual void clean_up( Cycle cycle) noexcept = 0;

    uint32 write_counter = 0;
    uint32 initialized_bandwidth = 0;

    const uint32 _fanout = 0;
    const uint32 installed_bandwidth = 0;
};

template<class T> class ReadPort;
    
template<class T> class WritePort : public BasicWritePort
{
public:
    WritePort<T>( const std::string& key, uint32 bandwidth, uint32 fanout)
        : BasicWritePort( key, bandwidth, fanout)
    { }

    void write( T&& what, Cycle cycle)
    {
        increment_write_counter();
        basic_write( std::forward<T>( what), cycle);
    }

    void write( const T& what, Cycle cycle)
    {
        increment_write_counter();
        basic_write( std::move( T( what)), cycle);
    }
    
private:
    void init( const std::vector<BasicReadPort*>& readers) final;
    ReadPort<T>* port_cast( Port* p) const;
    
    void clean_up( Cycle cycle) noexcept final;

    void basic_write( T&& what, Cycle cycle) noexcept( std::is_nothrow_copy_constructible<T>::value);

    std::vector<ReadPort<T>*> destinations = {};
};

// Has to be out of class due to VS bug
// https://developercommunity.visualstudio.com/content/problem/457098/extern-template-instantiation-does-not-work-for-vi.html
template<class T>
void ReadPort<T>::init( uint32 bandwidth)
{
    // +1 to handle reads-after-writes
    queue.resize( ( get_latency().to_size_t() + 1) * bandwidth);
}

template<class T> class ReadPort : public BasicReadPort
{
public:
    ReadPort<T>( const std::string& key, Latency latency) : BasicReadPort( key, latency) { }

    bool is_ready( Cycle cycle) const noexcept
    {
        return !queue.empty() && std::get<Cycle>(queue.front()) == cycle;
    }

    T read( Cycle cycle) noexcept(std::is_nothrow_copy_constructible<T>::value)
    {
        assert( is_ready( cycle));
        T tmp( std::move( std::get<T>(queue.front())));
        queue.pop();
        return tmp;
    }

private:
    friend class WritePort<T>;
    void emplaceData( T&& what, Cycle cycle)
        noexcept( std::is_nothrow_copy_constructible<T>::value)
    {
        queue.emplace( std::move( what), cycle + get_latency());
    }

    void init( uint32 bandwidth) final;
    void clean_up( Cycle cycle) noexcept
    {
        while ( !queue.empty() && std::get<Cycle>(queue.front()) < cycle)
           queue.pop();
    }

    PortQueue<std::pair<T, Cycle>> queue;
};

template<class T>
void WritePort<T>::clean_up( Cycle cycle) noexcept
{
    reset_write_counter();
    for ( const auto& reader : destinations)
        reader->clean_up( cycle);
}

template<class T>
void WritePort<T>::basic_write( T&& what, Cycle cycle)
    noexcept( std::is_nothrow_copy_constructible<T>::value)
{
    // Copy data to all ports except first one
    auto it = std::next( destinations.begin());
    for ( ; it != destinations.end(); ++it)
        (*it)->emplaceData( std::move( T( what)), cycle); // Force copy ctor

    // Move data to the first port
    destinations.front()->emplaceData( std::forward<T>( what), cycle);
}

template<class T>
ReadPort<T>* WritePort<T>::port_cast( Port* p) const try
{
    return dynamic_cast<ReadPort<T>*>( p);
}
catch ( const std::bad_cast&)
{
    throw PortError( get_key() + " has type mismatch between write and read ports");
}

template<class T>
void WritePort<T>::init( const std::vector<BasicReadPort*>& readers)
{
    base_init( readers);
    destinations.reserve( readers.size());
    for (const auto& r : readers)
        destinations.emplace_back( port_cast( r));
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
