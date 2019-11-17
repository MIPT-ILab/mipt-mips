#! cmake
#! Build file for the scalar MIPS CPU simulator
#! Soshin Konstantin
#! soshinkonstantinv@gmail.com
#! https://github.com/SoshinK

cmake_minimum_required(VERSION 3.9)

project(mipt-mips)
enable_testing()
find_package(Boost REQUIRED)

# Options
set(default_build_type "Release")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Export for Clang-Tidy
option(ENABLE_COVERAGE "Generate coverage for codecov.io" OFF)
option(ENABLE_PROFILE "Generate coverage for gprof" OFF)
set(GDB_SOURCE_PATH "" CACHE PATH "Path to GDB source tree")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

set(CPPS
    infra/print128.cpp
    infra/config/main_wrapper.cpp
    infra/config/config.cpp
    infra/ports/module.cpp
    infra/ports/ports.cpp
    infra/cache/cache_tag_array.cpp
    infra/replacement/cache_replacement.cpp
    memory/memory.cpp
    memory/hierarchied_memory.cpp
    memory/plain_memory.cpp
    memory/elf/elf_loader.cpp
    memory/argv_loader/argv_loader.cpp
    func_sim/func_sim.cpp
    func_sim/driver/driver.cpp
    func_sim/traps/trap.cpp
    mips/mips_instr.cpp
    mips/mips_version.cpp
    mips/mips_register/mips_register.cpp
    mips/mips_driver.cpp
    risc_v/riscv_register/riscv_register.cpp
    risc_v/riscv_instr.cpp
    export/gdb/gdb_wrapper.cpp
    export/cen64/cen64_wrapper.cpp
    export/cache/runner.cpp
    kernel/kernel.cpp
    kernel/mars/mars_kernel.cpp
    modules/ports_instance.cpp
    modules/fetch/fetch.cpp
    modules/fetch/bpu/bpu.cpp
    modules/decode/decode.cpp
    modules/execute/execute.cpp
    modules/mem/mem.cpp
    modules/branch/branch.cpp
    modules/core/perf_sim.cpp
    modules/writeback/writeback.cpp
    modules/writeback/checker/checker.cpp
    simulator.cpp
)

set(TESTS_CPPS
    infra/t/unit_test.cpp
    infra/cache/t/unit_test.cpp
    infra/config/t/unit_test.cpp
    infra/instrcache/t/unit_test.cpp
    infra/replacement/t/unit_test.cpp
    infra/ports/port_queue/t/unit_test.cpp
    infra/ports/t/unit_test.cpp
    infra/ports/t/example_test.cpp
    infra/ports/t/topology_test.cpp
    memory/argv_loader/t/unit_tests.cpp
    memory/cen64/t/unit_test.cpp
    memory/t/check_coherency.cpp
    memory/t/unit_test.cpp
    kernel/t/replicant_test.cpp
    kernel/t/unit_test.cpp
    kernel/mars/t/unit_test.cpp
    mips/mips_register/t/unit_test.cpp
    mips/t/mips32_test.cpp
    mips/t/mips32_cp1_test.cpp
    mips/t/mips32_divmult_test.cpp
    mips/t/mips32_branch_test.cpp
    mips/t/mips32_memory_test.cpp
    mips/t/mips32_traps_test.cpp
    mips/t/mips64_test.cpp
    risc_v/riscv_register/t/unit_test.cpp
    risc_v/t/unit_test.cpp
    risc_v/t/riscv64_test.cpp
    func_sim/rf/t/unit_test.cpp
    func_sim/traps/t/unit_test.cpp
    func_sim/driver/t/unit_test.cpp
    func_sim/t/unit_test.cpp
    modules/fetch/bpu/t/unit_test.cpp
    modules/core/t/unit_test.cpp
    export/gdb/t/unit_test.cpp
    export/cache/t/unit_test.cpp
)

add_definitions(-DTEST_PATH=\"${CMAKE_CURRENT_LIST_DIR}/../tests\")
add_definitions(-DKERNEL_IMAGES=\"${CMAKE_CURRENT_LIST_DIR}/../kernels/\")

set(RISCV_DIR ${CMAKE_CURRENT_LIST_DIR}/../riscv-opcodes)
add_custom_command(
    OUTPUT ${RISCV_DIR}/riscv.opcode.gen.h
    COMMAND python ${RISCV_DIR}/parse_opcodes -c ${RISCV_DIR}/opcodes ${RISCV_DIR}/../riscv-bitmanip-opcodes ${RISCV_DIR}/opcodes-rvc ${RISCV_DIR}/opcodes-rvc-pseudo > ${RISCV_DIR}/riscv.opcode.gen.h
    DEPENDS ${RISCV_DIR}/parse_opcodes ${RISCV_DIR}/opcodes ${RISCV_DIR}/opcodes-rvc ${RISCV_DIR}/../riscv-bitmanip-opcodes
)

if (MSVC)
    # /Zc:rvalueCast is a workaround for VS non-standard behavior, see
    # https://developercommunity.visualstudio.com/content/problem/457095/cannot-bind-temporary-int-object-to-an-rvalue-refe.html
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /Zc:rvalueCast")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wold-style-cast -Wvla -Werror")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (ENABLE_COVERAGE)
    if (WIN32)
        message(FATAL_ERROR "CodeCov is not supported for Windows builds")
    endif()
    if (NOT CMAKE_COMPILER_IS_GNUCXX)
        message(FATAL_ERROR "CodeCov is supported only for GCC builds")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -g -DNDEBUG")
endif()

if (ENABLE_PROFILE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
endif()

if (GDB_SOURCE_PATH)
    message("Building GDB integration with ${GDB_SOURCE_PATH}")
    set(GDB_SIM_SRCS
            ${GDB_SOURCE_PATH}/sim/common/sim-utils.c
            ${GDB_SOURCE_PATH}/sim/common/callback.c
            ${GDB_SOURCE_PATH}/sim/common/sim-io.c
            ${CMAKE_CURRENT_LIST_DIR}/export/gdb/gdb_interface.cpp)

    add_definitions(-DHAVE_CONFIG_H)
    include_directories(${CMAKE_CURRENT_LIST_DIR}/export/gdb/)
    include_directories(SYSTEM
            ${GDB_SOURCE_PATH}/
            ${GDB_SOURCE_PATH}/bfd
            ${GDB_SOURCE_PATH}/include
            ${GDB_SOURCE_PATH}/sim/common)
else ()
    set(GDB_SIM_SRCS)
endif ()

if (ENABLE_IPO)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT result)
  if(result)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
  else()
    message(WARNING "IPO is not supported")
  endif()
endif()

include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(SYSTEM
        ${CMAKE_CURRENT_LIST_DIR}/../external
        ${RISCV_DIR}
        ${Boost_INCLUDE_DIRS})

add_library(mipt-mips-src STATIC ${GDB_SIM_SRCS} ${CPPS} ${RISCV_DIR}/riscv.opcode.gen.h)
add_library(mipt-mips-cen64-intf STATIC export/cen64/cen64_intf.cpp memory/cen64/cen64_memory.cpp)
add_executable(mipt-mips export/standalone/main.cpp)
add_executable(unit-tests EXCLUDE_FROM_ALL export/catch/catch.cpp ${TESTS_CPPS})
add_executable(cachesim export/cache/main.cpp)

target_link_libraries(mipt-mips-cen64-intf mipt-mips-src)
target_link_libraries(mipt-mips mipt-mips-src)
target_link_libraries(cachesim mipt-mips-src)
target_link_libraries(unit-tests mipt-mips-src mipt-mips-cen64-intf)

add_test(all_tests unit-tests)

if (GDB_SOURCE_PATH)
    set_target_properties(mipt-mips-src PROPERTIES
            PREFIX ""
            SUFFIX ""
            OUTPUT_NAME "libsim.a"
            ARCHIVE_OUTPUT_DIRECTORY "${GDB_SOURCE_PATH}/sim/mips")
endif (GDB_SOURCE_PATH)
