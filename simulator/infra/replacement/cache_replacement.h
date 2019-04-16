/**
 * CacheReplacement.h
 * Header for the Cache replacement algorithms module.
 * @author Oleg Ladin, Denis Los, Andrey Agrachev
 */

#ifndef CACHEREPLACEMENT_H
#define CACHEREPLACEMENT_H

#include "../exception.h"

#include <memory>

struct CacheReplacementException final : Exception
{
    explicit CacheReplacementException( const std::string& msg)
        : Exception( msg)
    { }
};

struct CacheReplacementInterface
{
    virtual ~CacheReplacementInterface() = default;
    virtual void touch( std::size_t) = 0;
    virtual void set_to_erase( std::size_t) = 0;
    virtual void allocate( std::size_t) = 0;
    virtual std::size_t update() = 0;
    virtual std::size_t get_ways() const = 0;
};

std::unique_ptr<CacheReplacementInterface> create_cache_replacement( const std::string& name, std::size_t ways);

#endif // CACHEREPLACEMENT_H
