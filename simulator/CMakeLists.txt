#! cmake
#! Build file for the MIPS-V and MIPT-MIPS simulators
#! Soshin Konstantin
#! soshinkonstantinv@gmail.com
#! https://github.com/SoshinK

cmake_minimum_required(VERSION 3.13.5) # for add_link_options

project(mipt-mips)
enable_testing()
find_package(Boost REQUIRED)

# Options
set(default_build_type "Release")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Export for Clang-Tidy
option(ENABLE_COVERAGE "Generate coverage for codecov.io" OFF)
option(ENABLE_PROFILE "Generate coverage for gprof" OFF)
option(ENABLE_IPO "Enable interprocedural optimizations" OFF)
option(ENABLE_ASAN "Enable address sanitizing in debug build" ON)
option(ENABLE_UBSAN "Enable UB sanitizing in debug build" ON)
set(GDB_SOURCE_PATH "" CACHE PATH "Path to GDB source tree")

if(NOT DEFINED PYTHON3_COMMAND)
  set(PYTHON3_COMMAND python3)
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

include(ExternalProject)
set(ELFIO_PREFIX ${CMAKE_HOME_DIRECTORY}/elfio)
ExternalProject_Add(elfio
    PREFIX ${ELFIO_PREFIX}
    GIT_REPOSITORY https://github.com/serge1/ELFIO.git
    GIT_TAG Release_3.8
    CONFIGURE_COMMAND cmake -E echo "Skipping configure step."
    BUILD_COMMAND cmake -E echo "Skipping build step."
    INSTALL_COMMAND cmake -E echo "Skipping install step."
)
include_directories(SYSTEM ${ELFIO_PREFIX}/src/elfio)

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
    mips/t/mips_version_test.cpp
    risc_v/riscv_register/t/unit_test.cpp
    risc_v/t/unit_test.cpp
    risc_v/t/riscv64_test.cpp
    risc_v/t/riscv32_driver_test.cpp
    func_sim/rf/t/unit_test.cpp
    func_sim/traps/t/unit_test.cpp
    func_sim/driver/t/unit_test.cpp
    func_sim/t/alu_test.cpp
    func_sim/t/unit_test.cpp
    modules/fetch/bpu/t/unit_test.cpp
    modules/core/t/unit_test.cpp
    modules/branch/t/unit_test.cpp
    export/gdb/t/unit_test.cpp
    export/cache/t/unit_test.cpp
)

add_definitions(-DTEST_PATH=\"${CMAKE_CURRENT_LIST_DIR}/../tests\")
add_definitions(-DKERNEL_IMAGES=\"${CMAKE_CURRENT_LIST_DIR}/../kernels/\")

set(RISCV_DIR ${CMAKE_CURRENT_LIST_DIR}/../riscv-opcodes)
file(GLOB_RECURSE RISCV_OPCODES ${RISCV_DIR}/opcodes-*)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_LIST_DIR}/riscv.opcode.gen.h
    COMMAND ${PYTHON3_COMMAND} ${RISCV_DIR}/parse_opcodes -c ${RISCV_OPCODES} > ${CMAKE_CURRENT_LIST_DIR}/riscv.opcode.gen.h
    DEPENDS ${RISCV_DIR}/parse_opcodes ${RISCV_OPCODES}
)

if (MSVC)
    # /Zc:rvalueCast is a workaround for VS non-standard behavior, see
    # https://developercommunity.visualstudio.com/content/problem/457095/cannot-bind-temporary-int-object-to-an-rvalue-refe.html
    #
    # -D_SCL_SECURE_NO_WARNINGS is a workaround for Boost warning, see
    # https://docs.microsoft.com/en-us/cpp/standard-library/scl-secure-no-warnings?view=vs-2019
    # https://lists.boost.org/Archives/boost/2008/09/141771.php
    #
    # Warning 4244 is required to reduce excessive narrow casting
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /Zc:rvalueCast -D_SCL_SECURE_NO_WARNINGS /wd4244 ")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wold-style-cast -Wvla -Werror")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
endif()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
    add_link_options(-Wno-unused-command-line-argument)
endif()

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
if (COMPILER_SUPPORTS_MARCH_NATIVE)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
endif()

if (ENABLE_COVERAGE)
    message(STATUS "Coverage collection enabled")
    add_definitions(-DNDEBUG) # exclude 'assert' macro
    if (NOT MSVC)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} --coverage -g")
        add_link_options(--coverage)
        if (NOT CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
            link_libraries(gcov)
        endif()
     endif()
endif()

if (ENABLE_PROFILE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
    add_link_options(-pg)
endif()

if (ENABLE_ASAN AND NOT MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -static-libasan")
    endif()
endif()

if (ENABLE_UBSAN AND NOT MSVC)
   set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined")
   set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=undefined -lgcc_s")
   if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
       set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} --rtlib=compiler-rt")
   endif()
endif()

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

add_library(mipt-mips-src OBJECT
    infra/log.cpp
    infra/target.cpp
    infra/config/main_wrapper.cpp
    infra/config/config.cpp
    infra/ports/module.cpp
    infra/ports/ports.cpp
    infra/ports/timing.cpp
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
    mips/mips_register/mips_register.cpp
    mips/mips_driver.cpp
    risc_v/riscv_register/riscv_register.cpp
    risc_v/riscv_instr.cpp
    risc_v/riscv_driver.cpp
    export/gdb/gdb_wrapper.cpp
    export/cen64/cen64_wrapper.cpp
    export/cache/runner.cpp
    kernel/kernel.cpp
    kernel/mars/mars_kernel.cpp
    modules/ports_instance.cpp
    modules/fetch/fetch.cpp
    modules/fetch/bpu/bpu.cpp
    modules/fetch/bpu/bp_interface.cpp
    modules/decode/decode.cpp
    modules/execute/execute.cpp
    modules/mem/mem.cpp
    modules/branch/branch.cpp
    modules/core/perf_sim.cpp
    modules/writeback/writeback.cpp
    modules/writeback/checker/checker.cpp
    simulator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/riscv.opcode.gen.h
)

add_dependencies(mipt-mips-src elfio)

add_library(mipt-mips-cen64-intf STATIC export/cen64/cen64_intf.cpp memory/cen64/cen64_memory.cpp)
add_executable(mipt-mips export/standalone/main.cpp)
add_executable(unit-tests EXCLUDE_FROM_ALL export/catch/catch.cpp ${TESTS_CPPS})
add_executable(cachesim export/cache/main.cpp)

target_link_libraries(mipt-mips-cen64-intf mipt-mips-src)
target_link_libraries(mipt-mips mipt-mips-src)
target_link_libraries(unit-tests mipt-mips-src)
target_link_libraries(cachesim mipt-mips-src)

# Symlink for new name
if (NOT MSVC)
    add_custom_target(mipt-v ALL COMMAND ${CMAKE_COMMAND} -E create_symlink mipt-mips mipt-v)
    add_dependencies(mipt-v mipt-mips)
endif()

target_link_libraries(unit-tests mipt-mips-cen64-intf)

add_test(all_tests unit-tests)

if (GDB_SOURCE_PATH)
    message("Building GDB integration with ${GDB_SOURCE_PATH}")

    add_definitions(-DHAVE_CONFIG_H)
    include_directories(${CMAKE_CURRENT_LIST_DIR}/export/gdb/)
    include_directories(SYSTEM
            ${GDB_BUILD_PATH}/
            ${GDB_BUILD_PATH}/bfd
            ${GDB_SOURCE_PATH}/include
            ${GDB_SOURCE_PATH}/sim/common)

    add_library(gdb-mips-sim STATIC
            ${GDB_SOURCE_PATH}/sim/common/sim-utils.c
            ${GDB_SOURCE_PATH}/sim/common/callback.c
            ${GDB_SOURCE_PATH}/sim/common/sim-io.c
            ${CMAKE_CURRENT_LIST_DIR}/export/gdb/gdb_interface.cpp)
  
    target_link_libraries(gdb-mips-sim mipt-mips-src)
    set_target_properties(gdb-mips-sim PROPERTIES
            PREFIX ""
            SUFFIX ""
            OUTPUT_NAME "libsim.a"
            ARCHIVE_OUTPUT_DIRECTORY "${GDB_BUILD_PATH}/sim/mips")
endif (GDB_SOURCE_PATH)
