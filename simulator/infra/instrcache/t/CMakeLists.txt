project(Test-infra-instrcache)

set(SRCCPPS unit_test.cpp ../../../mips/mips_instr.cpp)


include_directories(../../.. ./.)

set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra -Wpedantic -Wold-style-cast")
add_executable(${PROJECT_NAME} ${SRCCPPS})
target_link_libraries(${PROJECT_NAME} gtest ${LIBELF_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
