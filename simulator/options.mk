# C++ compile
CXX ?= g++
CXXVERSION:= $(shell $(CXX) --version | grep version | sed -e 's/.*version //' -e 's/ .*//')
CXXFLAGS:= -Wall -Wextra -Werror -Wpedantic -Wold-style-cast -Weffc++
UNAME:= $(shell uname -o)
ifeq ($(shell uname), Darwin)	#OSX uses older version of clang which does not support std=c++17
        CXXFLAGS+= -std=c++1z
else
        CXXFLAGS+= -std=c++17
endif

OBJ_DIR:=obj-$(CXX)-$(UNAME)-D$(DEBUG)-P$(GPROF)

LDFLAGS= # -static
ifeq ($(DEBUG), 1)
	CXXFLAGS+= -O0 -g
else
	CXXFLAGS+= -O3
	LDFLAGS+= -flto
endif

ifeq ($(GPROF), 1)
	CXXFLAGS+= -pg -g
	LDFLAGS+= -pg
endif
	
ifeq ($(UNAME), Msys)
    CXXFLAGS+= -D__STDC_LIMIT_MACROS
    ifeq ($(CXX), clang++)
        CXXFLAGS+= --target=i386-w32-mingw32
        CXXFLAGS+= -D__float128=void
    endif
endif

# Boost directories
BOOST_INCL?=  /usr/local/include
BOOST_LPATH?= /usr/local/lib

# LibELF directories
LIBELF_INCL?= /usr/local/include
LIBELF_LPATH?=/usr/local/lib

# option for C++ compiler specifying directories
# to search for headers and libraries
INCL+= -I. -isystem $(BOOST_INCL) -isystem $(LIBELF_INCL)
LPATH:= -L $(BOOST_LPATH) -L $(LIBELF_LPATH)

TIDY?=clang-tidy
TIDYFLAGS:=-header-filter=.* -checks=*,-google-readability-braces-around-statements,-readability-braces-around-statements,-cppcoreguidelines-pro-type-union-access,-cppcoreguidelines-pro-bounds-array-to-pointer-decay,-llvm-header-guard,-llvm-include-order,-readability-redundant-declaration,-cert-err58-cpp,-cppcoreguidelines-pro-bounds-constant-array-index,-android-cloexec-fopen
