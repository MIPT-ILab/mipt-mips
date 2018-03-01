project(Test-infra-memory)

if(DEST)
set(INPUTFILES mips_bin_exmpl.out)
file(COPY ${INPUTFILES} DESTINATION ${DEST}/infra/memory/t)
endif()

set(SRCCPPS unit_test.cpp ../memory.cpp ../../elf_parser/elf_parser.cpp)


include_directories(../../.. ./.)


add_executable(${PROJECT_NAME} ${SRCCPPS})
target_link_libraries(${PROJECT_NAME} gtest ${LIBELF_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
