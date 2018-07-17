#!/usr/bin/bash
# Copyright (C) 2018 Pavel Kryukov, MIPT-MIPS Project
#
# This script is intended to be used with CI systems, please do not run it manually.
# To get MIPS GNU Binutils, please follow this instruction: 
# https://github.com/MIPT-ILab/mipt-mips/wiki/MIPS-binutils
#
git clone git://sourceware.org/git/binutils-gdb.git --branch binutils-2_31 --depth=1
cd binutils-gdb
./configure --target=mips-linux-gnu --prefix=$1 --disable-gdb --disable-gprof > /dev/null
make all install MAKEINFO=true > /dev/null
cd ..
