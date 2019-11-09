/**
 * Standalone cache simulator
 * @author Pavel Kryukov
 */

#include "runner.h"

#include <infra/cache/cache_tag_array.h>
#include <infra/config/config.h>
#include <infra/macro.h>

namespace config {
    static AliasedRequiredValue<uint32> size = { "s", "size", "Size of instruction level 1 cache (in bytes)"};
    static AliasedRequiredValue<uint32> ways = { "w", "ways", "Amount of ways in instruction level 1 cache"};
    static AliasedRequiredValue<std::string> file = { "f", "filename", "file name with trace"};

    static AliasedValue<std::string> replacement = { "r", "replacement", "LRU", "Cache replacement scheme"};
    static AliasedValue<uint32> line_size = { "l", "line_size", 64, "Line size of instruction level 1 cache (in bytes)"};
} // namespace config

int main( int argc, const char* argv[]) try {
    config::handleArgs( argc, argv, 1);
    auto cache = CacheTagArray::create( config::replacement, config::size, config::ways, config::line_size, 32);
    std::cout << run_cache( cache.get(), config::file);
    return 0;
}
catch (const config::HelpOption& e) {
    std::cout << "MIPT-V Standalone cache simulator."
              << std::endl << std::endl << e.what() << std::endl;
    return 0;
}
catch (const Exception& e) {
    std::cerr << e.what() << std::endl;
    return 2;
}
catch (const std::exception& e) {
    std::cerr << "System exception:\t\n" << e.what() << std::endl;
    return 2;
}
catch (...) {
    std::cerr << "Unknown exception\n";
    return 3;
}
