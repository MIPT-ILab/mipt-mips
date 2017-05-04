# C++ compile
CXX = g++
CXXFLAGS= -std=c++14 -Wall -Wextra -Werror -Wpedantic -Wold-style-cast -Weffc++
ifeq ($(DEBUG), 1)
	CXXFLAGS+= -O0 -g
else
	CXXFLAGS+= -O3
	LDFLAGS = -flto
endif

# GoogleTest directories
GTEST_DIR= $(TRUNK)/../libs/googletest
GTEST_INCL= -I $(GTEST_DIR)/lib/include
GTEST_LIB= $(GTEST_DIR)/lib/libgtest.a

# option for C++ compiler specifying directories 
# to search for headers
INCL= -I ./
BOOST_INCL= -I /usr/local/include/boost/

