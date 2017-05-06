.PHONY: all clean

include ../../simulator/options.mk

all: lib/libgtest.a

lib/libgtest.a: lib/gtest-all.o
	ar -rv lib/libgtest.a lib/gtest-all.o

lib/gtest-all.o: lib/src/gtest-all.cc
	$(CXX) $(CXXFLAGS) -Wno-effc++ -I lib/include -I lib -c $< -o $@

lib/src/gtest-all.cc: release-1.8.0.tar.gz
	rm -rf lib
	tar xfm release-1.8.0.tar.gz
	mv googletest-release-1.8.0/googletest lib
	rm -rf googletest-release-1.8.0

release-1.8.0.tar.gz:
	wget https://github.com/google/googletest/archive/release-1.8.0.tar.gz


clean:
	rm -rf lib release-1.8.0.tar.gz
