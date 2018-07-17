#!/usr/bin/bash
wget http://ftp.gnu.org/gnu/binutils/binutils-2.30.tar.bz2
tar xjf binutils-2.31.tar.bz2
cd binutils-2.31
./configure --target=mips-linux-gnu --prefix=$1 --disable-gdb --disable-gprof --with-system-zlib > /dev/null
make all install MAKEINFO=true > /dev/null
cd ..
