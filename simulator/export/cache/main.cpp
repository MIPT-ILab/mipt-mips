/**
 * Standalone cache simulator
 * @author Pavel Kryukov
 */

#include "runner.h"

#include <infra/cache/cache_tag_array.h>
#include <infra/config/config.h>
#include <infra/config/main_wrapper.h>
#include <infra/macro.h>

namespace config {
    static AliasedRequiredValue<uint32> size = { "s", "size", "Size of instruction level 1 cache (in bytes)"};
    static AliasedRequiredValue<uint32> ways = { "w", "ways", "Amount of ways in instruction level 1 cache"};
    static AliasedRequiredValue<std::string> file = { "f", "filename", "file name with trace"};

    static AliasedValue<std::string> replacement = { "r", "replacement", "LRU", "Cache replacement scheme"};
    static AliasedValue<uint32> line_size = { "l", "line_size", 64, "Line size of instruction level 1 cache (in bytes)"};
} // namespace config

class Main : public MainWrapper
{
    using MainWrapper::MainWrapper;
private:
    int impl( int argc, const char* argv[]) const final {
        config::handleArgs( argc, argv, 1);
        auto cache = CacheTagArray::create( config::replacement, config::size, config::ways, config::line_size, 32);
        std::cout << run_cache( cache.get(), config::file);
        return 0;
    }
};

int main( int argc, const char* argv[])
{
    return Main( "MIPT-V Standalone cache simulator.").run( argc, argv);
}
