#! cmake
#! Build file for the MIPT-MIPS tests
#! Kompan Vyacheslav, Soshin Konstantin
#! kompan.vo@phystech.edu, soshinkonstantinv@gmail.com
#! trexxet, https://github.com/SoshinK

set(TESTS
    infra/cache
    infra/elf_parser
    infra/memory
    infra/config
    infra/instrcache
    infra/ports
    mips/mips_register
    mips
    func_sim/rf
    risc_v/riscv_register
    modules/fetch/bpu
    func_sim
    modules/core
    )

add_definitions(-DTEST_PATH=\"${CMAKE_CURRENT_LIST_DIR}/../../traces\")
add_definitions(-DTEST_DATA_PATH=\"${CMAKE_CURRENT_LIST_DIR}/\")

# Build list of tests CPPs
foreach (ITER IN LISTS TESTS)
    string(CONCAT SRC_UNIT_TEST ${ITER} "/t/unit_test.cpp")
    list(APPEND TESTS_CPPS ${SRC_UNIT_TEST})
endforeach ()

# Add 'tests' target
add_executable(tests tests/test_main.cpp ${TESTS_CPPS})
target_link_libraries(tests mipt-mips-src ${Boost_LIBRARIES} ${LIBELF_LIBRARIES})

# Generate list for CTest
enable_testing()
add_test(all_tests tests)
