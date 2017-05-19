# C++ compile
CXX ?= g++
CXXFLAGS:= -Wall -Wextra -Werror -Wpedantic -Wold-style-cast -Weffc++ --std=c++14
CXXVERSION:= $(shell $(CXX) --version | grep version | sed -e 's/.*version //' -e 's/ .*//')

UNAME:= $(shell uname -o)

LDFLAGS= # -static
ifeq ($(DEBUG), 1)
	CXXFLAGS+= -O0 -g
else
	CXXFLAGS+= -O3
	LDFLAGS+= -flto
endif

ifeq ($(UNAME), Msys)
    CXXFLAGS+= -U__STRICT_ANSI__ -D__STDC_LIMIT_MACROS -D__LIBELF_INTERNAL__=1
    ifeq ($(CXX), clang++)
        CXXFLAGS+= --target=i386-w32-mingw32
        CXXFLAGS+= -D__float128=void
    endif
endif

# GoogleTest directories
GTEST_DIR:= ../libs/googletest
GTEST_INCL:=  $(GTEST_DIR)/lib/include
GTEST_LPATH:= $(GTEST_DIR)/lib
GTEST_LIB:= $(GTEST_LPATH)/libgtest.a

# Boost directories
BOOST_INCL?=  /usr/local/include/boost/
BOOST_LPATH?= /usr/local/lib

# LibELF directories
LIBELF_INCL?=./
LIBELF_LPATH?=/usr/local/lib

# option for C++ compiler specifying directories
# to search for headers and libraries
INCL+= -I $(pwd)/ -I $(BOOST_INCL) -I $(LIBELF_INCL)
LPATH:= -L $(BOOST_LPATH) -L $(GTEST_LPATH) -L $(LIBELF_LPATH)

TIDYFLAGS:=-header-filter=.* -checks=*,-google-readability-braces-around-statements,-readability-braces-around-statements,-cppcoreguidelines-pro-type-union-access,-cppcoreguidelines-pro-bounds-array-to-pointer-decay,-cppcoreguidelines-pro-type-vararg,-cppcoreguidelines-pro-bounds-constant-array-index,-cppcoreguidelines-pro-bounds-pointer-arithmetic,-llvm-header-guard,-llvm-include-order,-modernize-pass-by-value
