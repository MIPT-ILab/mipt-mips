# C++ compile
CXX ?= g++
CXXFLAGS= -Wall -Wextra -Werror -Wpedantic -Wold-style-cast -Weffc++

ifeq ($(CXX), clang++)
	CXXVERSION:= $(shell clang++ --version | grep version | sed -e 's/.*version //' -e 's/ .*//')
	ifeq ($(CXXVERSION), 3.4)
		CXXFLAGS+= --std=c++1y
	else
		CXXFLAGS+= --std=c++14
	endif
else ifeq ($(CXX), g++)
	CXXVERSION:= $(shell g++ -dumpversion)
	CXXFLAGS+=  --std=c++14
endif

LDFLAGS= # -static
ifeq ($(DEBUG), 1)
	CXXFLAGS+= -O0 -g
else
	CXXFLAGS+= -O3
	LDFLAGS+= -flto
endif

# GoogleTest directories
GTEST_DIR= ../libs/googletest
GTEST_INCL=  $(GTEST_DIR)/lib/include
GTEST_LPATH= $(GTEST_DIR)/lib
GTEST_LIB= $(GTEST_LPATH)/libgtest.a

# Boost directories
BOOST_INCL=  /usr/local/include/boost/
BOOST_LPATH= /usr/local/lib

# option for C++ compiler specifying directories
# to search for headers and libraries
INCL= -I ./
INCL+= -I $(BOOST_INCL)
LPATH= -L $(BOOST_LPATH) -L $(GTEST_LPATH)
