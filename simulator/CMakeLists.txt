#! cmake
#! Build file for the scalar MIPS CPU simulator
#! Soshin Konstantin
#! soshinkonstantinv@gmail.com
#! https://github.com/SoshinK

cmake_minimum_required(VERSION 3.9)

project(mipt-mips CXX)

set(default_build_type "Release")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Export for Clang-Tidy
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/external)

option(ENABLE_COVERAGE "Generate coverage for codecov.io" OFF)
option(ENABLE_PROFILE "Generate coverage for gprof" OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

set(CPPS infra/macro_test.cpp
    infra/memory/memory.cpp
    infra/memory/elf/elf_loader.cpp
    infra/config/config.cpp
    infra/ports/ports.cpp
    infra/cache/cache_tag_array.cpp
    modules/fetch/fetch.cpp
    modules/fetch/bpu/bpu.cpp
    modules/decode/decode.cpp
    modules/execute/execute.cpp
    modules/mem/mem.cpp
    modules/core/perf_sim.cpp
    modules/writeback/writeback.cpp
    func_sim/func_sim.cpp
    mips/mips_instr.cpp
    mips/mips_version.cpp
    mips/mips_register/mips_register.cpp
    risc_v/riscv_register/riscv_register.cpp
    simulator.cpp
    )

# Compilation flags
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /wd4505 /wd4244 /wd4996 /wd4267 /wd4702 /GR-")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wold-style-cast -Wvla -Werror -fno-rtti")
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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -g")
endif()

if (ENABLE_PROFILE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
endif()

#include(CheckIPOSupported)
#check_ipo_supported(RESULT result)
#if(result)
#  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
#else()
#  message(WARNING "IPO is not supported")
#endif()

# Headers
include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(SYSTEM ${CMAKE_CURRENT_LIST_DIR}/../external)

# Boost
find_package(Boost REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

# Targets
add_library(mipt-mips-src STATIC ${CPPS})

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} mipt-mips-src)

# Tests
include(${CMAKE_CURRENT_LIST_DIR}/tests/CMakeLists.txt)
