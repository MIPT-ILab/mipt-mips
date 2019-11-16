/**
 * main_wrapper.cpp - wrapper for entry points of MIPT-V binaries
 * @author Pavel Kryukov
 * Copyright 2019 MIPT-V
 */

#include "main_wrapper.h"

#include <infra/config/config.h>
#include <infra/exception.h>

// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
int MainWrapper::run( int argc, const char* argv[]) const try {
    return impl( argc, argv);
}
catch ( const config::HelpOption& e) {
    std::cout << desc << std::endl << std::endl << e.what() << std::endl;
    return 0;
}
catch ( const Exception& e) {
    std::cerr << e.what() << std::endl;
    return 2;
}
catch ( const std::exception& e) {
    std::cerr << "System exception:\t\n" << e.what() << std::endl;
    return 2;
}
catch (...) {
    std::cerr << "Unknown exception\n";
    return 3;
}
