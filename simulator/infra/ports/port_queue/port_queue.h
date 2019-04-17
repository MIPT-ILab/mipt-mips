/**
 * port_queue.h - non-allocating queue container.
 * The speed of that unit is critical, while we need type erasure
 * to speed up compilation time; therefore, we use C-style memory
 * management here.
 * @author Pavel Kryukov
 * Copyright 2019 MIPT-MIPS team
 */

#ifndef PORT_QUEUE_H
#define PORT_QUEUE_H

#include <infra/arena.h>

template<typename T>
class PortQueue
{
    Arena<T> arena{};
    size_t p_front = 0;
    size_t p_back = 0;
    size_t occupied = 0;
    size_t capacity = 0;

    template<size_t PortQueue::* p>
    inline void advance_ptr() noexcept
    {
        ++(this->*p);
        if (this->*p == capacity)
            this->*p = 0;
    }

    void clear()
    {
        while ( !empty())
            pop();
    }

public:
    PortQueue() = default;
    ~PortQueue()
    {
        clear();
    }

    // These are not trivial, and I'm lazy to implement
    PortQueue( const PortQueue& src) = delete;
    PortQueue( PortQueue&& src) = delete;
    PortQueue& operator=( const PortQueue&) = delete;
    PortQueue& operator=( PortQueue&&) = delete;

    void resize( size_t size)
    {
        clear();
        arena.allocate( size);
        p_front = p_back = occupied = 0;
        capacity = size;
    }

    template<typename... Args> void emplace( Args&& ... args)
        noexcept( std::is_nothrow_constructible<T, Args...>::value)
    {
        arena.emplace( p_back, std::forward<Args>( args)...);
        advance_ptr<&PortQueue::p_back>();
        ++occupied;
    }

    bool full() const noexcept
    {
        return occupied == capacity;
    }

    void pop() noexcept
    {
        arena.destroy( p_front);
        advance_ptr<&PortQueue::p_front>();
        --occupied;
    }

    bool empty() const noexcept
    {
        return occupied == 0;
    }

    const T& front() const noexcept
    {
        return arena[p_front];
    }
};

#endif
