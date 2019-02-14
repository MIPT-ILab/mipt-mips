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

    struct Cluster
    {
        class BasicWritePort* writer = nullptr;
        std::vector<class BasicReadPort*> readers = {};
    };
    std::unordered_map<std::string, Cluster> map = { };

    void add_port( class BasicWritePort* port);
    void add_port( class BasicReadPort* port);

    // Friendship to register themselves inside map
    friend class BasicReadPort;
    friend class BasicWritePort;
};

class Port : public Log
{
public:
    const std::string& get_key() const { return _key; }
    PortMap& get_port_map() const { return _portMap; }
protected:
    explicit Port( std::string key);
private:
    std::string _key;
    PortMap& _portMap = PortMap::get_instance();
};

class BasicReadPort : public Port
{
public:
    auto get_latency() const noexcept { return _latency; }
protected:
    BasicReadPort( const std::string& key, Latency latency);
private:
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

    void increment_write_counter()
    {
        ++write_counter;
        if ( write_counter > initialized_bandwidth)
            throw PortError( get_key() + " port is overloaded by bandwidth");
    }

    void reset_counter() noexcept
    {
        write_counter = 0;
    }
    
private:
    virtual void init( const std::vector<BasicReadPort*>& readers) = 0;

    // Friendship to get a list of read ports
    friend class PortMap;
    const uint32 _fanout;
    uint32 write_counter = 0;
    uint32 initialized_bandwidth = 0;
    const uint32 installed_bandwidth;
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
        push_to_destinations( std::forward<T>( what), cycle);
    }

    void write( const T& what, Cycle cycle)
    {
        increment_write_counter();
        push_to_destinations( std::move( T( what)), cycle);
    }
private:
    void push_to_destinations( T&& what, Cycle cycle)
        noexcept( std::is_nothrow_copy_constructible<T>::value)
    {
        // Copy data to all ports except first one
        auto it = std::next( destinations.begin());
        for ( ; it != destinations.end(); ++it)
            (*it)->emplaceData( std::move( T( what)), cycle); // Force copy ctor

        // Move data to the first port
        destinations.front()->emplaceData( std::forward<T>( what), cycle);
    }

    void init( const std::vector<BasicReadPort*>& readers) final
    {
        base_init( readers);
        destinations.reserve( readers.size());
        for (const auto& r : readers)
            add_port( r);
    }

    void add_port( BasicReadPort* r)
    {
        auto reader = port_cast( r);
        destinations.emplace_back( reader);
        reader->init( get_bandwidth());    
    }

    ReadPort<T>* port_cast( Port* p) const try
    {
        return dynamic_cast<ReadPort<T>*>( p);
    }
    catch ( const std::bad_cast&)
    {
        throw PortError( get_key() + " has type mismatch between write and read ports");
    }

    void clean_up( Cycle cycle) noexcept final
    {
        reset_counter();
        for ( const auto& reader : destinations)
            reader->clean_up( cycle);
    }

    std::vector<ReadPort<T>*> destinations = {};
};

template<class T> class ReadPort : public BasicReadPort
{
public:
    ReadPort<T>( const std::string& key, Latency latency) : BasicReadPort( key, latency) { }

    bool is_ready( Cycle cycle) const noexcept
    {
        return !queue.empty() && std::get<Cycle>(queue.front()) == cycle;
    }

    T read( Cycle cycle)
    {
        if ( !is_ready( cycle))
            throw PortError( get_key() + " ReadPort was not ready for read at cycle=" + cycle.to_string());

        T tmp( std::move( std::get<T>(queue.front())));
        queue.pop();
        return tmp;
    }
private:
    void emplaceData( T&& what, Cycle cycle)
        noexcept( std::is_nothrow_copy_constructible<T>::value)
    {
        queue.emplace( std::move( what), cycle + get_latency());
    }

    void init( uint32 bandwidth)
    {
        // +1 to handle reads-after-writes
        queue.resize( ( get_latency().to_size_t() + 1) * bandwidth);
    }

    void clean_up( Cycle cycle) noexcept
    {
        while ( !queue.empty() && std::get<Cycle>(queue.front()) < cycle)
            queue.pop();
    }

    // Friendship to obtain data directly from the port
    friend class WritePort<T>;
    PortQueue<std::pair<T, Cycle>> queue;
};

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
