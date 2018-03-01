project(Test-mips)

set(SRCCPPS unit_test.cpp 
../mips_instr.cpp 
../mips_memory.cpp 
../../infra/memory/memory.cpp 
../../infra/elf_parser/elf_parser.cpp)
 

include_directories(../.. ./.)


add_executable(${PROJECT_NAME} ${SRCCPPS})
target_link_libraries(${PROJECT_NAME} gtest ${LIBELF_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
