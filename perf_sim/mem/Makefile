# specifying relative path to the TRUNK
TRUNK= ../../

# paths to look for headers
vpath %.h $(TRUNK)/common

# option for C++ compiler specifying directories
# to search for headers
INCL= -I ./ -I $(TRUNK)/common/

#
# Enter for building func_memory stand alone program
#
Debug: miss_rate_sim

miss_rate_sim: cache_tag_array.o miss_rate_sim.o
	$(CXX) -g -o $@ $^
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

cache_tag_array.o: cache_tag_array.cpp cache_tag_array.h types.h
	$(CXX) -g -c $< $(INCL)

miss_rate_sim.o: miss_rate_sim.cpp cache_tag_array.h
	$(CXX) -g -c $< $(INCL)

clean:
	@-rm *.o
	@-rm perf_sim
