#!/usr/bin/bash
export BINUTILS_VER=2.31
wget http://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VER.tar.bz2
tar xjf binutils-$BINUTILS_VER.tar.bz2
cd binutils-$BINUTILS_VER
export BINUTILS=`pwd`/cross/mips
./configure --target=mips-linux-gnu --prefix=$BINUTILS --disable-gdb --disable-gprof --with-system-zlib > /dev/null
make all install MAKEINFO=true > /dev/null
export PATH=$PATH:$BINUTILS/bin
cd ..
