#! cmake
#! Building the scalar MIPS CPU simulator
#! Soshin Konstantin
#! soshinkonstantinv@gmail.com
#! https://github.com/SoshinK

cmake_minimum_required(VERSION 2.6)
project(mipt-mips)

set(CPPS infra/macro_test.cpp
    infra/elf_parser/elf_parser.cpp 
    infra/memory/memory.cpp 
    infra/config/config.cpp 
    infra/ports/ports.cpp 
    infra/cache/cache_tag_array.cpp 
    bypass/data_bypass.cpp 
    fetch/fetch.cpp 
    mips/mips_instr.cpp  
    func_sim/func_sim.cpp 
    core/perf_sim.cpp 
    writeback/writeback.cpp 
    simulator.cpp
    mips/mips_register/mips_register.cpp
    )

set(TESTS 
    infra/cache
    infra/elf_parser
    infra/memory
    infra/config
    infra/instrcache
    infra/ports
    infra/string
    bpu
    func_sim
    core
    mips/mips_register
    )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wold-style-cast -Werror")

if(${DEBUG})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    set(CMAKE_LD_FLAGS "${CMAKE_CXX_FLAGS} -flto")
endif()

if(NOT ${RELEASE})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()

if(APPLE) 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z") 
else() 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

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
        elf
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
