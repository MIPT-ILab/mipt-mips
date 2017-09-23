[![Build Status](https://travis-ci.org/MIPT-ILab/mipt-mips.svg?branch=master)](https://travis-ci.org/MIPT-ILab/mipt-mips)[![Build status](https://ci.appveyor.com/api/projects/status/eungty6us329y8w1/branch/master?svg=true)](https://ci.appveyor.com/project/miptilab/mipt-mips/branch/master)

# MIPT-MIPS

This project is a part of [ILab](https://mipt.ru/drec/about/ilab/) activity at [Moscow Institute of Physics and Technology](http://phystech.edu/) (MIPT).

The aim of the project is to teach the students the computer architecture through development of a microprocessor implementing the [MIPS](http://en.wikipedia.org/wiki/MIPS32) instruction set in both functional and performance simulators.

This repository contains source files of cycle-accurate simulator (aka performance or pre-silicon simulator) of MIPS CPU.

More details about the project are available on [Wiki](https://github.com/MIPT-ILab/mipt-mips/wiki/Home/)

## Getting started

1. [Install MIPS binutils](https://github.com/MIPT-ILab/mipt-mips/wiki/MIPS-binutils) if you need to build MIPS ELF binaries.
1. MIPT-MIPS requires LibELF and Boost. We have [an instruction how to get them](https://github.com/MIPT-ILab/mipt-mips/wiki/Required-libraries)
1. In POSIX environment (Linux, OSX, Windows-Ubuntu, and Msys) you can build MIPT-MIPS just by `make mipt-mips` command
1. WIP on MS Visual Studio support. Currently you can build MIPT-MIPS in command line using `build.cmd` batch file.

### C++ requirements

MIPT-MIPS is written in C++17. Thus, you have to use one of these compilers:
1. GCC 7 or higher
1. MS Visual Studio 2017 or higher
1. Clang 5.0 or higher

Required version of Boost is 1.55.

## Basic command line options

* `-b <filename>` — provide path to ELF binary file to execute
* `-n <number>` — number of instructions to run
* `-f` — enables functional simulation only
* `-d` — enables detailed output of each cycle

## Known issues
* Reduced subset of MIPS instructions is supported at the moment.

## May I contribute?

Yes, if you attend MIPT-MIPS lectures. See our [CONTRIBUTING.md](CONTRIBUTING.md) file for details.
