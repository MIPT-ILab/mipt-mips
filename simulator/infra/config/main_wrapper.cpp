/**
 * main_wrapper.cpp - wrapper for entry points of MIPT-V binaries
 * @author Pavel Kryukov
 * Copyright 2019 MIPT-V
 */

#include "main_wrapper.h"

#include <infra/config/config.h>
#include <infra/exception.h>

static const constexpr int EXCEPTION_EXIT_CODE = 2;
static const constexpr int NO_EXCEPTION_EXIT_CODE = 3;
static const constexpr int INVALID_OPTION_EXIT_CODE = 4;

int MainWrapper::run( int argc, const char* argv[]) const try {
    return impl( argc, argv);
}
catch ( const config::HelpOption&) {
    std::cout << desc << std::endl << std::endl << config::help() << std::endl;
    return 0;
}
catch ( const config::InvalidOption& e) {
    std::cerr << e.what() << std::endl
        << desc << std::endl << std::endl
        << config::help() << std::endl;
    return INVALID_OPTION_EXIT_CODE;
}
catch ( const Exception& e) {
    std::cerr << e.what() << std::endl;
    return EXCEPTION_EXIT_CODE;
}
catch ( const std::exception& e) {
    std::cerr << "System exception:\t\n" << e.what() << std::endl;
    return EXCEPTION_EXIT_CODE;
}
catch (...) {
    std::cerr << "Unknown exception\n";
    return NO_EXCEPTION_EXIT_CODE;
}
