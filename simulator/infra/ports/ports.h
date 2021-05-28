/**
 * ports.h - template for simulation of ports.
 * @author Pavel Kryukov
 * Copyright 2017-2019 MIPT-MIPS team
 */

#ifndef PORTS_H
#define PORTS_H

#include <infra/exception.h>
#include <infra/log.h>
#include <infra/ports/port_queue/port_queue.h>
#include <infra/ports/timing.h>
#include <infra/types.h>

#include <boost/property_tree/ptree_fwd.hpp>

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
    static std::shared_ptr<PortMap> create_port_map();
    void init() const;

    friend class BasicWritePort;
    friend class BasicReadPort;
    friend class Root;

    void add_port( class BasicWritePort* port);
    void add_port( class BasicReadPort* port);

    boost::property_tree::ptree dump() const;

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
    const std::string& get_key() const noexcept { return k; }

    static constexpr const Latency LATENCY = 1_lt;
    static constexpr const Latency LONG_LATENCY = 30_lt;
    static constexpr const uint32 FANOUT = 1;
    static constexpr const uint32 BW = 1;

protected:
    Port( std::shared_ptr<PortMap> port_map, std::string key);
    std::shared_ptr<PortMap> get_port_map() const noexcept { return pm; }

    Cycle get_last_cycle() const noexcept { return last_cycle; }
    void update_last_cycle( Cycle cycle) noexcept
    {
        assert( last_cycle <= cycle);
        last_cycle = cycle;
    }

private:
    const std::shared_ptr<PortMap> pm;
    const std::string k;
    Cycle last_cycle = 0_cl;
};

class BasicReadPort : public Port
{
public:
    auto get_latency() const noexcept { return _latency; }

protected:
    BasicReadPort( const std::shared_ptr<PortMap>& port_map, const std::string& key, Latency latency);

private:
    friend class PortMap;
    virtual void init( uint32 bandwidth) = 0;
    const Latency _latency;
};

class BasicWritePort : public Port
{
public:
    auto get_fanout() const noexcept { return _fanout; }
    auto get_bandwidth() const noexcept { return initialized_bandwidth; }

protected:
    BasicWritePort( const std::shared_ptr<PortMap>& port_map, const std::string& key, uint32 bandwidth);
    void base_init( const std::vector<BasicReadPort*>& readers);

    void increment_write_counter( Cycle cycle)
    {
        write_counter = get_last_cycle() == cycle ? write_counter + 1 : 0;
        update_last_cycle( cycle);
        if ( write_counter > get_bandwidth())
            throw PortError( get_key() + " port is overloaded by bandwidth");
    }

private:
    friend class PortMap;
    virtual void init( const std::vector<BasicReadPort*>& readers) = 0;

    uint32 write_counter = 0;
    uint32 initialized_bandwidth = 0;
    std::size_t _fanout = 0;
    
    const uint32 installed_bandwidth = 0;
};

template<class T> class ReadPort;

template<class T> class WritePort : public BasicWritePort
{
public:
    WritePort( const std::shared_ptr<PortMap>& port_map, const std::string& key, uint32 bandwidth)
        : BasicWritePort( port_map, key, bandwidth)
    { }

    void write( T&& what, Cycle cycle)
    {
        increment_write_counter( cycle);
        basic_write( std::forward<T>( what), cycle);
    }

    void write( const T& what, Cycle cycle)
    {
        increment_write_counter( cycle);
        basic_write( T( what), cycle);
    }

private:
    void init( const std::vector<BasicReadPort*>& readers) final;
    void add_reader( BasicReadPort* readers);
    void basic_write( T&& what, Cycle cycle) noexcept( std::is_nothrow_copy_constructible<T>::value);

    std::vector<ReadPort<T>*> destinations = {};
};

template<class T> class ReadPort : public BasicReadPort
{
public:
    ReadPort( const std::shared_ptr<PortMap>& port_map, const std::string& key, Latency latency)
        : BasicReadPort( port_map, key, latency)
    { }

    bool is_ready( Cycle cycle) noexcept
    {
        cleanup_stale_data( cycle);
        return !queue.empty() && std::get<Cycle>(queue.front()) == cycle;
    }

    T read( Cycle cycle)
    {
        if ( !is_ready( cycle))
            throw PortError( get_key() + " has no data to read in cycle:" + cycle.to_string());
        return pop_front();
    }

private:
    friend class WritePort<T>;
    void emplaceData( T&& what, Cycle cycle)
        noexcept( std::is_nothrow_copy_constructible<T>::value)
    {
        Cycle cycle_to_read = cycle + get_latency();
        cleanup_stale_data( cycle);
        queue.emplace( std::move( what), cycle_to_read);
    }

    void cleanup_stale_data( Cycle cycle) noexcept
    {
        update_last_cycle( cycle);
        while ( !queue.empty() && std::get<Cycle>(queue.front()) < cycle)
           queue.pop();
    }

    void init( uint32 bandwidth) final;

    T pop_front() noexcept(std::is_nothrow_copy_constructible<T>::value)
    {
        T tmp( std::move( std::get<T>(queue.front())));
        queue.pop();
        return tmp;
    }

    PortQueue<std::pair<T, Cycle>> queue;
};

// Has to be out of class due to VS bug
// https://developercommunity.visualstudio.com/content/problem/457098/extern-template-instantiation-does-not-work-for-vi.html
template<class T>
void ReadPort<T>::init( uint32 bandwidth)
{
    // +1 to handle reads-after-writes
    queue.resize( ( get_latency().to_size_t() + 1) * bandwidth);
}

// Methods operating with ReadPort<T> are also declared out of class
template<class T>
void WritePort<T>::basic_write( T&& what, Cycle cycle)
    noexcept( std::is_nothrow_copy_constructible<T>::value)
{
    // Copy data to all ports, but move to the first one
    auto it = std::next( destinations.begin());
    for ( ; it != destinations.end(); ++it)
        (*it)->emplaceData( T( what), cycle); // Force copy ctor

    destinations.front()->emplaceData( std::move( what), cycle);
}

template<class T>
void WritePort<T>::init( const std::vector<BasicReadPort*>& readers)
{
    base_init( readers);
    destinations.reserve( readers.size());
    for (const auto& r : readers)
        add_reader( r);
}

template<class T>
void WritePort<T>::add_reader( BasicReadPort* reader)
{
    auto r = dynamic_cast<ReadPort<T>*>( reader);
    if ( r == nullptr)
        throw PortError( get_key() + " has type mismatch between write and read ports");

    destinations.emplace_back( r);
}

#endif // PORTS_H
