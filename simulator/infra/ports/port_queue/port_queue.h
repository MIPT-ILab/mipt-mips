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

template<typename T>
class PortQueue
{
    T* arena = nullptr;
    size_t arena_size = 0;
    size_t p_front = 0;
    size_t p_back = 0;
    bool wrap = false;

    void advance_ptr(size_t& p) noexcept
    {
        ++p;
        if ( p == arena_size) {
            p = 0;
            wrap = !wrap;
        }
    }

    void destroy()
    {
        while ( !empty())
            pop();
        std::free( arena); 
    }

public:
    PortQueue() = default;
    ~PortQueue()
    {
        destroy();
    }

    // These are not trivial, and I'm lazy to implement
    PortQueue( const PortQueue& src) = delete;
    PortQueue( PortQueue&& src) = delete;
    PortQueue& operator=( const PortQueue&) = delete;
    PortQueue& operator=( PortQueue&&) = delete;

    void resize( size_t size)
    {
        destroy();
        arena = reinterpret_cast<T*>(std::malloc(sizeof(T) * size));
        arena_size = size;
    }

    template<typename... Args> void emplace( Args ... args) noexcept
    {
        new (&arena[p_back]) T( std::forward<Args>( args)...);
        advance_ptr( p_back);
    }

    bool full() const noexcept
    {
        return p_front == p_back && (wrap || arena_size == 0);
    }

    void pop() noexcept
    {
        arena[p_front].~T();
        advance_ptr( p_front);
    }

    bool empty() const noexcept
    {
        return p_front == p_back && !wrap;
    }

    const T& front() const noexcept
    {
        return arena[ p_front];
    }
};

#endif
