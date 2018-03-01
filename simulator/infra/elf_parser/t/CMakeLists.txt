project(Test-infra-elf_parser)

set(SRCCPPS unit_test.cpp ../elf_parser.cpp)

add_definitions(-DTEST_PATH=\"${CMAKE_CURRENT_BINARY_DIR}/../../../../traces/tt.core.out\")

include_directories(../../.. ./.)


add_executable(${PROJECT_NAME} ${SRCCPPS})
target_link_libraries(${PROJECT_NAME} gtest ${LIBELF_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
