# 
# Building the elf_parser for MIPS ElF binaries
# @author Alexander Titov <alexander.igorevich.titov@gmail.com>
# Copyright 2012 uArchSim iLab Project
#

vpath %.h ../../common
vpath %.h ../elf_parser
vpath %.cpp ../elf_parser

# option for C++ compiler specifying directories 
# to search for headers
include=-I ./ -I ../elf_parser/ -I ../../common/

func_memory: func_memory.o elf_parser.o main.o
	@# don't forget to link ELF library using "-l elf"
	$(CXX) -o $@ $^ -l elf
	@echo "---------------------------------"
	@echo "$@ is built successifully"

func_memory.o: func_memory.cpp func_memory.h types.h
	$(CXX) -c $< $(include)

elf_parser.o: elf_parser.cpp elf_parser.h types.h
	$(CXX) -c $< $(include)

main.o: main.cpp func_memory.h types.h
	$(CXX) -c $< $(include)

clean:
	-rm *.o func_memory
