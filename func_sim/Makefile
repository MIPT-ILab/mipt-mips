TRUNK= ../

vpath %.h $(TRUNK)/common
vpath %.h $(TRUNK)/func_sim/
vpath %.h $(TRUNK)/func_sim/elf_parser/
vpath %.h $(TRUNK)/func_sim/func_instr/
vpath %.h $(TRUNK)/func_sim/func_memory/
vpath %.cpp $(TRUNK)/func_sim/
vpath %.cpp $(TRUNK)/func_sim/elf_parser
vpath %.cpp $(TRUNK)/func_sim/func_instr/
vpath %.cpp $(TRUNK)/func_sim/func_memory/

INCL= -I ./ -I $(TRUNK)/common/ -I $(TRUNK)/func_sim/func_instr/ -I $(TRUNK)/func_sim/elf_parser/ -I $(TRUNK)/func_sim/func_memory/

func_sim: func_memory.o elf_parser.o func_instr.o func_sim.o main.o
	$(CXX) -o $@ $^ -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

func_instr.o: func_instr.cpp func_instr.h types.h
	$(CXX) -c $< $(INCL)

func_memory.o: func_memory.cpp func_memory.h types.h
	$(CXX) -c $< $(INCL)

elf_parser.o: elf_parser.cpp elf_parser.h types.h
	$(CXX) -c $< $(INCL)

func_sim.o: func_sim.cpp func_memory.h types.h func_instr.h
	$(CXX) -c $< $(INCL)

main.o: main.cpp func_sim.h types.h
	$(CXX) -c $< $(INCL)

clean:
	@-rm *.o
	@-rm func_sim

