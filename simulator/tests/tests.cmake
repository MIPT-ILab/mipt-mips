#! cmake
#! Build file for the MIPT-MIPS tests
#! Kompan Vyacheslav, Soshin Konstantin
#! kompan.vo@phystech.edu, soshinkonstantinv@gmail.com
#! trexxet, https://github.com/SoshinK

# All tests are built into a single binary, list of tests can be viewed with --gtest_list_tests
# Specific tests can be called with --gtest_filter

set(TESTS
        # Test infrastructure
        infra/cache
        infra/elf_parser
        infra/memory
        infra/config
        infra/instrcache
        infra/ports
        # Test MIPS
        mips/mips_register
        mips
        func_sim/rf
        # Test RISCV
        risc_v/riscv_register
        # Test units
        modules/fetch/bpu
        # Overall tests
        func_sim
        modules/core
        )

set(gtest_force_shared_crt ON CACHE BOOL "Use shared (DLL) run-time lib for GTest" FORCE)
add_subdirectory(../googletest/googletest gtest-build)
include_directories(SYSTEM ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
if (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} VERSION_LESS 3.10)
    include(tests/GoogleTest.cmake)
else (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} VERSION_LESS 3.10)
    include(GoogleTest)
endif (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} VERSION_LESS 3.10)

add_definitions(-DTEST_PATH=\"${CMAKE_CURRENT_LIST_DIR}/../../traces\")
add_definitions(-DTEST_DATA_PATH=\"${CMAKE_CURRENT_LIST_DIR}/\")

# Build list of tests CPPs
foreach (ITER IN LISTS TESTS)
    string(CONCAT SRC_UNIT_TEST ${ITER} "/t/unit_test.cpp")
    list(APPEND TESTS_CPPS ${SRC_UNIT_TEST})
endforeach ()

# Add 'tests' target
add_executable(tests tests/test_main.cpp ${TESTS_CPPS})
target_link_libraries(tests gtest mipt-mips-src ${Boost_LIBRARIES} ${LIBELF_LIBRARIES})

# Generate list for CTest
enable_testing()
gtest_discover_tests(tests)
