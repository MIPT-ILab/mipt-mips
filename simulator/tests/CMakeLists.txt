#! cmake
#! Build file for the MIPT-MIPS tests
#! Kompan Vyacheslav, Soshin Konstantin
#! kompan.vo@phystech.edu, soshinkonstantinv@gmail.com
#! trexxet, https://github.com/SoshinK

set(TESTS_CPPS
    infra/cache/t/unit_test.cpp
    infra/config/t/unit_test.cpp
    infra/instrcache/t/unit_test.cpp
    infra/ports/t/unit_test.cpp
    memory/t/unit_test.cpp
    mips/mips_register/t/unit_test.cpp
    mips/t/unit_test.cpp
    func_sim/rf/t/unit_test.cpp
    risc_v/riscv_register/t/unit_test.cpp
    modules/fetch/bpu/t/unit_test.cpp
    func_sim/t/unit_test.cpp
    func_sim/syscall/t/unit_test.cpp
    modules/core/t/unit_test.cpp
    )

add_definitions(-DTEST_PATH=\"${CMAKE_CURRENT_LIST_DIR}/../../traces\")
add_definitions(-DTEST_DATA_PATH=\"${CMAKE_CURRENT_LIST_DIR}/\")

# Add 'tests' target
add_executable(unit-tests EXCLUDE_FROM_ALL tests/main.cpp ${TESTS_CPPS})
target_link_libraries(unit-tests mipt-mips-src)

# Generate list for CTest
enable_testing()
add_test(all_tests unit-tests)
