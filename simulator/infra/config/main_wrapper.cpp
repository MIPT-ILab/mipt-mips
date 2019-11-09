/**
 * main_wrapper.cpp - wrapper for entry points of MIPT-V binaries
 * @author Pavel Kryukov
 * Copyright 2019 MIPT-V
 */

#include "main_wrapper.h"

#include <infra/config/config.h>
#include <infra/exception.h>

int MainWrapper::run( int argc, const char* argv[]) const try {
    impl( argc, argv);
    return 0;
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
