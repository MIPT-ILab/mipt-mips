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

#include <boost/align/aligned_alloc.hpp>
#include <memory>
#include <type_traits>

template<typename T>
class PortQueue
{
    static void* allocate( size_t size)
    {
        return boost::alignment::aligned_alloc( alignof(T), sizeof(T) * size);
    }

    struct Deleter
    {
        void operator()(void *p) { boost::alignment::aligned_free(p); }
    };

    std::unique_ptr<void, Deleter> arena = nullptr;
    T* arena_start = nullptr;
    const T* arena_end = nullptr;
    T* p_front = nullptr;
    T* p_back = nullptr;
    size_t occupied = 0;

    inline void advance_ptr( T* PortQueue::* p) noexcept
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        ++( this->*p);
        if (this->*p == arena_end)
            this->*p = arena_start;
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
        arena = std::unique_ptr<void, Deleter>( allocate( size));
        arena_start = static_cast<T*>( arena.get());
        arena_end = arena_start + size; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic
        p_front = p_back = arena_start;
        occupied = 0;
    }

    template<typename... Args> void emplace( Args ... args) noexcept
    {
        new (p_back) T( std::forward<Args>( args)...);
        advance_ptr( &PortQueue::p_back);
        ++occupied;
    }

    bool full() const noexcept
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        return arena_start + occupied == arena_end;
    }

    void pop() noexcept
    {
        p_front->~T();
        advance_ptr( &PortQueue::p_front);
        --occupied;
    }

    bool empty() const noexcept
    {
        return occupied == 0;
    }

    const T& front() const noexcept
    {
        return *p_front;
    }
};

#endif
