/**
 * arena.h - non-allocating container.
 * Used for critical units with type erasure
 * to speed up compilation time;
 * therefore, we use C-style memory
 * management here.
 * @author Pavel Kryukov
 * Copyright 2019 MIPT-MIPS team
 */
 
#ifndef ARENA_H
#define ARENA_H

#include <cstdlib>
#include <memory>

template<typename T>
class Arena
{
public:
    void allocate( std::size_t capacity)
    {
        arena = std::unique_ptr<void, Deleter>( allocate_memory( capacity));
        storage = get_aligned_storage( capacity);
    }

    template<typename... Args> void emplace( std::size_t position, Args&& ... args)
        noexcept( std::is_nothrow_constructible<T, Args...>::value)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        new (storage + position) T( std::forward<Args>( args)...);
    }

    void destroy( std::size_t position)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        (storage + position)->~T();
    }

    const T& operator[]( std::size_t position) const noexcept
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        return storage[position];
    }
private:
    static constexpr std::size_t get_size( std::size_t capacity) noexcept
    {
        return sizeof(T) * capacity;
    }

    auto get_aligned_storage( std::size_t capacity) const noexcept
    {
        void* ptr = arena.get();
        auto size = get_size( capacity);
        auto space = size + sizeof(T);
        return static_cast<T*>(std::align( alignof(T), size, ptr, space));
    }

    static void* allocate_memory( std::size_t capacity)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-no-malloc)
        return std::malloc( get_size( capacity) + sizeof(T));
    }

    struct Deleter
    {
        void operator()(void *p)
        {
            // NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-no-malloc)
            std::free(p);
        }
    };

    std::unique_ptr<void, Deleter> arena = nullptr;
    T* storage = nullptr;
};

#endif // ARENA_H
