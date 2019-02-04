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

#include <memory>

template<typename T>
class PortQueue
{
    struct Deleter
    {
        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-no-malloc, hicpp-no-malloc)
        void operator()(T *p) { std::free(p); }
    };

    std::unique_ptr<T, Deleter> arena = nullptr;
    const T* arena_end = nullptr;
    T* p_front = nullptr;
    T* p_back = nullptr;
    bool wrap = false;

    void advance_ptr( T** p) noexcept
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        ++*p;
        if ( *p == arena_end) {
            *p = arena.get();
            wrap = !wrap;
        }
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
        // NOLINTNEXTLINE(cppcoreguidelines-no-malloc, hicpp-no-malloc)
        arena = std::unique_ptr<T, Deleter>(static_cast<T*>(std::malloc(sizeof(T) * size)));
        arena_end = arena.get() + size;
        p_front = p_back = arena.get();
        wrap = false;
    }

    template<typename... Args> void emplace( Args ... args) noexcept
    {
        new (p_back) T( std::forward<Args>( args)...);
        advance_ptr( &p_back);
    }

    bool full() const noexcept
    {
        return (p_front == p_back && wrap) || arena == nullptr;
    }

    void pop() noexcept
    {
        p_front->~T();
        advance_ptr( &p_front);
    }

    bool empty() const noexcept
    {
        return p_front == p_back && !wrap;
    }

    const T& front() const noexcept
    {
        return *p_front;
    }
};

#endif
