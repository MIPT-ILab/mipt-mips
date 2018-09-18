# Build file for MIPT-MIPS GDB interface
# Vyacheslav Kompan
# Copyright 2018 MIPT-MIPS


set(GDB_SIM_COMMON_PATH ${CMAKE_CURRENT_LIST_DIR}/../../../../sim/common)

# Add mipt-mips-gdb target iff we are building under GDB
if (EXISTS ${GDB_SIM_COMMON_PATH})

    set(GDB_SIM_SRCS
            ${GDB_SIM_COMMON_PATH}/sim-utils.c
            ${GDB_SIM_COMMON_PATH}/callback.c
            ${GDB_SIM_COMMON_PATH}/sim-io.c)

    add_library(mipt-mips-gdb STATIC
            ${CPPS}
            ${CMAKE_CURRENT_LIST_DIR}/gdb_interface.cpp
            ${GDB_SIM_SRCS})

    #target_link_libraries(mipt-mips-gdb mipt-mips-src)

    # Output: libsim.a stored in "simulator/gdb"
    set_target_properties(mipt-mips-gdb PROPERTIES
            PREFIX ""
            SUFFIX ""
            OUTPUT_NAME "libsim.a"
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bin/")

    target_include_directories(mipt-mips-gdb SYSTEM PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/../../../..
            ${CMAKE_CURRENT_LIST_DIR}/../../../../bfd
            ${CMAKE_CURRENT_LIST_DIR}/../../../../include
            ${CMAKE_CURRENT_LIST_DIR}/../../../../sim/common)

endif (EXISTS ${GDB_SIM_COMMON_PATH})
