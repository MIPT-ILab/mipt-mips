#! cmake
#! Building the scalar MIPS CPU simulator
#! Soshin Konstantin
#! soshinkonstantinv@gmail.com
#! https://github.com/SoshinK

cmake_minimum_required(VERSION 2.6)

include(CheckCXXCompilerFlag)

project(mipt-mips CXX)

set(default_build_type "Release")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

set(CPPS infra/macro_test.cpp
    infra/elf_parser/elf_parser.cpp
    infra/memory/memory.cpp
    infra/config/config.cpp
    infra/ports/ports.cpp
    infra/cache/cache_tag_array.cpp
    bypass/data_bypass.cpp
    fetch/fetch.cpp
    core/perf_sim.cpp
    func_sim/func_sim.cpp
    mips/mips_instr.cpp
    mips/mips_register/mips_register.cpp
    simulator.cpp
    writeback/writeback.cpp
    )

set(TESTS
# Test infrastructure
    infra/cache
    infra/elf_parser
    infra/memory
    infra/config
    infra/instrcache
    infra/ports
    infra/string
# Test MIPS
    mips/mips_register
# Test units
    bpu
# Overall tests
    func_sim
    core
    )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wold-style-cast -Werror")

check_cxx_compiler_flag("-std=c++17" HAS_STDCXX_17)
if (HAS_STDCXX_17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
else()
    check_cxx_compiler_flag("-std=c++1z" HAS_STDCXX_1z)
    if (HAS_STDCXX_1z)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
    else()
      message("The compiler doesn't support the C++17 standard")
      exit()
    endif()
endif()

set(CMAKE_CXX_FLAGS_DEBUG " -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE " -O3 -g -march=native")
set(CMAKE_LD_FLAGS_RELEASE " -flto")

add_executable(${PROJECT_NAME} main.cpp)

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
        /usr/local/Cellar/libelf/0.8.13_1/include/libelf
        ENV CPATH)

if(NOT LIBELF_INCLUDE_DIRS)
    message(SEND_ERROR "Can't find libelf.h")
endif()

find_library (LIBELF_LIBRARIES
    NAMES
        libelf
    PATHS
        /usr/lib
        ENV LIBRARY_PATH
        ENV LD_LIBRARY_PATH)

if(NOT LIBELF_LIBRARIES)
    message(SEND_ERROR "Can't find libelf")
endif()

include_directories(${LIBELF_INCLUDE_DIRS})

add_library (mipt-mips-src STATIC ${CPPS} ${LIBELF_LIBRARIES} ${Boost_LIBRARIES})

target_link_libraries(${PROJECT_NAME}  mipt-mips-src ${LIBELF_LIBRARIES} ${Boost_LIBRARIES})

#clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

###### googletest ######
add_subdirectory(../googletest/googletest ./build)
enable_testing()

add_definitions(-DTEST_PATH=\"${CMAKE_CURRENT_LIST_DIR}/../traces\")
#==================
foreach(ITER IN LISTS TESTS)
    #copy all filesfrom */t to build
    string(CONCAT GLOBBING_EXPR ${ITER} "/t/*.*")
    file(GLOB INPFILES LIST_DIRECTORIES false ${GLOBBING_EXPR})
    foreach(ITER2 IN LISTS INPFILES)
        file(COPY ${ITER2} DESTINATION ./.)
    endforeach()
    
    string(CONCAT SRC_UNIT_TEST ${ITER} "/t/unit_test.cpp")
    string(REGEX REPLACE "/" "_"  EXEC_NAME_NOTFULL ${ITER})
    string(CONCAT EXEC_NAME ${EXEC_NAME_NOTFULL} "_test")
    
    add_executable(${EXEC_NAME} ${SRC_UNIT_TEST})
    target_link_libraries(${EXEC_NAME} gtest mipt-mips-src ${Boost_LIBRARIES} ${LIBELF_LIBRARIES})
    add_test(NAME ${EXEC_NAME} COMMAND ${EXEC_NAME})

endforeach()
#==================
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
