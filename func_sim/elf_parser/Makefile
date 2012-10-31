# 
# Building the elf_parser for MIPS ElF binaries
# @author Alexander Titov <alexander.igorevich.titov@gmail.com>
# Copyright 2012 uArchSim iLab Project
#

vpath %.h ../../common

elf_parser: elf_parser.o main.o
	@# don't forget to link ELF library using "-l elf"
	g++ -o elf_parser elf_parser.o main.o -l elf
	@echo "---------------------------------"
	@echo "elf_parser is built successifully"

elf_parser.o: elf_parser.h elf_parser.cpp types.h
	g++ -c elf_parser.cpp -I ./ -I ../../common/

main.o: elf_parser.h main.cpp types.h
	g++ -c main.cpp -I ./ -I ../../common/

clean:
	-rm elf_parser.o main.o
