project(Test-infra-config)



set(SRCCPPS unit_test.cpp ../config.cpp
    ../../../main.cpp
    ../../../simulator.cpp
    ../../../infra/elf_parser/elf_parser.cpp 
    ../../../infra/memory/memory.cpp 
    ../../../infra/ports/ports.cpp 
    ../../../infra/cache/cache_tag_array.cpp 
    ../../../mips/mips_instr.cpp 
    ../../../mips/mips_register.cpp 
    ../../../core/perf_sim.cpp 
    ../../../func_sim/func_sim.cpp
    ../../../bypass/data_bypass.cpp
    ../../../fetch/fetch.cpp
    ../../../writeback/writeback.cpp)

include_directories(../../.. ./.)


add_executable(${PROJECT_NAME} ${SRCCPPS})
target_link_libraries(${PROJECT_NAME} gtest ${LIBELF_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
