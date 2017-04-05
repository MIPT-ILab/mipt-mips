#
# Building the single-cycle MIPS simulator
# @author Pavel Kryukov <pavel.kryukov@phystech.com>
# Copyright 2015 MIPT-MIPS iLab Project
#

# specifying relative path to the TRUNK
TRUNK= ../

# Including options and compiler flags
include $(TRUNK)/options.mk

#
# Enter for building func_memory standalone program
#
OBJS= func_memory.o elf_parser.o func_instr.o func_sim.o main.o
DEPS= $(OBJS:.o=.d)

func_sim: $(OBJS)
	@# don't forget to link ELF library using "-l elf"
	$(CXX) -o $@ $^ -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

-include $(DEPS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< $(INCL)
	$(CXX) $(CXXFLAGS) -MM -o $(patsubst %.o, %.d, $@) $< $(INCL)

clean:
	@-rm *.o *.d
	@-rm func_sim
