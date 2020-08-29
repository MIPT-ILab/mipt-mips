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
    static const AliasedRequiredValue<uint32> size = { "s", "size", "Size of instruction level 1 cache (in bytes)"};
    static const AliasedRequiredValue<uint32> ways = { "w", "ways", "Amount of ways in instruction level 1 cache"};
    static const AliasedRequiredValue<std::string> file = { "t", "tracename", "file name with trace"};

    static const AliasedValue<std::string> replacement = { "r", "replacement", "LRU", "Cache replacement scheme"};
    static const Value<uint32> line_size = { "line_size", 64, "Line size of instruction level 1 cache (in bytes)"};
} // namespace config

class Main : public MainWrapper
{
    using MainWrapper::MainWrapper;
private:
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
    int impl( int argc, const char* argv[]) const final {
        config::handleArgs( argc, argv, 1);
        auto cache = CacheTagArray::create( config::replacement, config::size, config::ways, config::line_size, 32);
        std::cout << CacheRunner::create( cache.get())->run( config::file);
        return 0;
    }
};

int main( int argc, const char* argv[])
{
    return Main( "MIPT-V Standalone cache simulator.").run( argc, argv);
}
