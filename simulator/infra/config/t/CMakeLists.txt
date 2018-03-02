project(Test-infra-config)



set(SRCCPPS unit_test.cpp ../config.cpp ../../../main.cpp ../../../fetch/fetch.cpp)


include_directories(../../.. ./.)


add_executable(${PROJECT_NAME} ${SRCCPPS})
target_link_libraries(${PROJECT_NAME} gtest ${LIBELF_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
