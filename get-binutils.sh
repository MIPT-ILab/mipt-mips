#!/usr/bin/bash
export BINUTILS_VER=2.31
wget http://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VER.tar.bz2
mv binutils-$BINUTILS_VER.tar.bz2 binutils.tar.bz2
tar xjf binutils-$BINUTILS_VER.tar.bz2
cd binutils
./configure --target=mips-linux-gnu --prefix=$1 --disable-gdb --disable-gprof --with-system-zlib > /dev/null
make all install MAKEINFO=true > /dev/null
cd ..
