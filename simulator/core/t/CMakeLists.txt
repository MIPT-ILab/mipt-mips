project(Test-core)

add_definitions(-DTEST_PATH=\"${CMAKE_CURRENT_BINARY_DIR}/../../../../traces\")

set(SRCCPPS unit_test.cpp 
		../../infra/elf_parser/elf_parser.cpp 
                ../../infra/memory/memory.cpp 
                ../../infra/config/config.cpp 
                ../../infra/ports/ports.cpp 
                ../../infra/cache/cache_tag_array.cpp 
                ../../mips/mips_instr.cpp 
                ../../mips/mips_register.cpp 
                ../perf_sim.cpp 
		../../func_sim/func_sim.cpp
		../../bypass/data_bypass.cpp
		../../fetch/fetch.cpp
		../../writeback/writeback.cpp)


include_directories(../.. ./.)

add_executable(${PROJECT_NAME} ${SRCCPPS})
target_link_libraries(${PROJECT_NAME} gtest ${LIBELF_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
