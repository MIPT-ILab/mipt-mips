#! cmake
#! Build file for the MIPT-MIPS tests
#! Kompan Vyacheslav, Soshin Konstantin
#! kompan.vo@phystech.edu, soshinkonstantinv@gmail.com
#! trexxet, https://github.com/SoshinK

# All tests are built into a single binary, list of tests can be viewed with --gtest_list_tests
# Specific tests can be called with --gtest_filter

set(gtest_force_shared_crt ON CACHE BOOL "Use shared (DLL) run-time lib for GTest" FORCE)
add_subdirectory(../googletest/googletest gtest-build)
include_directories(SYSTEM ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include(GoogleTest)

add_definitions(-DTEST_PATH=\"${CMAKE_CURRENT_LIST_DIR}/../traces\")

# Build list of tests CPPs
foreach(ITER IN LISTS TESTS)
    # If tests depend on some data files, copy them to build directory to ease access for tests
    # Actually copying files seems like a workaround, so should it be made in a nicer way?
    # For example, keeping these files in one fixed location, e.g. 'simulator/tests/'
    string(CONCAT FILE_MASK ${ITER} "/t/*")
    file(GLOB FILES LIST_DIRECTORIES false ${FILE_MASK})
    list(FILTER FILES EXCLUDE REGEX ".*cpp") # we don't need to copy cpps
    foreach(ITER2 IN LISTS FILES)
        file(COPY ${ITER2} DESTINATION ./.)
    endforeach()

    string(CONCAT SRC_UNIT_TEST ${ITER} "/t/unit_test.cpp")
    list(APPEND TESTS_CPPS ${SRC_UNIT_TEST})
endforeach()

# Add 'tests' target
add_executable(tests tests/test_main.cpp ${TESTS_CPPS})
target_link_libraries(tests gtest mipt-mips-src ${Boost_LIBRARIES} ${LIBELF_LIBRARIES})

# Generate list for CTest
enable_testing()
gtest_discover_tests(tests)
