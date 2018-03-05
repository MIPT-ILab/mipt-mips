#cmake
#Building the scalar MIPS CPU simulator
#Soshin Konstantin
#soshinkonstantinv@gmail.com

cmake_minimum_required(VERSION 2.6)
project(mipt-mips)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wold-style-cast -Werror")

if(APPLE) 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z") 
else() 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

set(CPPS infra/macro_test.cpp
    infra/elf_parser/elf_parser.cpp 
    infra/memory/memory.cpp 
    infra/config/config.cpp 
    infra/ports/ports.cpp 
    infra/cache/cache_tag_array.cpp 
    bypass/data_bypass.cpp 
    fetch/fetch.cpp 
    mips/mips_instr.cpp 
    mips/mips_register.cpp 
    func_sim/func_sim.cpp 
    core/perf_sim.cpp 
    writeback/writeback.cpp 
    simulator.cpp)


find_path(SRCDIRS
    NAMES 
        CPPS
    PATHS
        ./.)


add_executable(${PROJECT_NAME} ${CPPS} main.cpp)

#include headers
include_directories(SRCDIRS ./.)

#include Boost libs
find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

#libelf
find_path(LIBELF_INCLUDE_DIRS
    NAMES
        libelf.h
    PATHS
        /usr/include
        /usr/include/libelf
        /usr/local/include
        /usr/local/include/libelf
        /opt/local/include
        /opt/local/include/libelf
        /sw/include
        /sw/include/libelf
        ENV CPATH)

if(NOT LIBELF_INCLUDE_DIRS)
    message(SEND_ERROR "Can't find libelf.h")
endif()

find_library (LIBELF_LIBRARIES
    NAMES
        elf
    PATHS
        /usr/lib
        /usr/local/lib
        /opt/local/lib
        /sw/lib
        ENV LIBRARY_PATH
        ENV LD_LIBRARY_PATH)

if(NOT LIBELF_LIBRARIES)
    message(SEND_ERROR "Can't find libelf")
endif()

include_directories(${LIBELF_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${LIBELF_LIBRARIES})

#clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#googletest
set(DEST ${CMAKE_CURRENT_BINARY_DIR})
message("${DEST}")

add_subdirectory(../googletest ./build)
enable_testing()

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_subdirectory(infra/cache/t)

add_subdirectory(infra/elf_parser/t)

add_subdirectory(infra/memory/t)

add_subdirectory(infra/config/t)

add_subdirectory(infra/instrcache/t)

add_subdirectory(infra/ports/t)

add_subdirectory(infra/string/t)

add_subdirectory(mips/t)

add_subdirectory(bpu/t)

add_subdirectory(func_sim/t)

add_subdirectory(core/t)

