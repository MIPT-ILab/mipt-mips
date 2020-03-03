/**
 * Standalone cache simulator
 * @author Pavel Kryukov
 */
 
#ifndef CACHE_RUNNER_H
#define CACHE_RUNNER_H

#include <infra/types.h>

#include <iostream>
#include <memory>

class CacheTagArray;

struct CacheRunnerResults
{
    uint64 accesses = 0;
    uint64 hits = 0;
    uint64 compulsory_misses = 0;

    [[nodiscard]] auto get_hit_rate() const noexcept  { return accesses == 0 ? 0 : double( hits) / accesses; }
    [[nodiscard]] auto get_miss_rate() const noexcept { return 1 - get_hit_rate(); }
    [[nodiscard]] auto get_misses() const noexcept { return accesses - hits; }
    [[nodiscard]] auto get_compulsory_miss_subrate() const noexcept { return get_misses() == 0 ? 0 : double( compulsory_misses) / get_misses(); }

    friend std::ostream& operator<<( std::ostream& out, const CacheRunnerResults& rhs);
};

class CacheRunner
{
public:
    CacheRunner() = default;
    virtual ~CacheRunner() = default;
    CacheRunner( const CacheRunner&) = delete;
    CacheRunner( CacheRunner&&) = delete;
    CacheRunner& operator=( const CacheRunner&) = delete;
    CacheRunner& operator=( CacheRunner&&) = delete;

    static std::unique_ptr<CacheRunner> create( CacheTagArray* cache);

    virtual CacheRunnerResults run( const std::string& filename) = 0;
};

#endif
