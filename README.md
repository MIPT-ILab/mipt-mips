[![Build Status](https://travis-ci.org/MIPT-ILab/mipt-mips.svg?branch=master)](https://travis-ci.org/MIPT-ILab/mipt-mips)[![Build status](https://ci.appveyor.com/api/projects/status/eungty6us329y8w1/branch/master?svg=true)](https://ci.appveyor.com/project/miptilab/mipt-mips/branch/master)

# MIPT-MIPS

MIPT-MIPS is a pre-silicon simulator of MIPS CPU. It measures _performance_ of program running on CPU, thus taking best features of RTL and common functional simulation:
* **Precision**. We provide cycle-accurate models of branch prediction unit and pipeline behavior.
* **Customization**. Cache size, branch prediction algorithms, and other parameters can be easily changed.
* **Simplicity**. Our source files are much more readable than RTL.
* **Speed**. Simulation frequency gets up to 0.7 MHz on i5-7300U.

MIPT-MIPS can be used for different purposes:
* Performance control of **software optimizations**: you may check IPC boosts of your programs
* Pathfinding of **hardware optimizations**: you may easily integrate some nice feature to CPU model
* **Education**: simulator is a nice experimental frog to study CPU internals

Features modeled:
* Configurable [branch prediction unit](https://github.com/MIPT-ILab/mipt-mips/wiki/BPU-model) with 5 prediction algorithms
* Configurable instruction cache with true-LRU policy
* Interstage data bypassing

More details about internals are available on [Wiki](https://github.com/MIPT-ILab/mipt-mips/wiki/Home/)


----
## Getting started
_Konstantin, please update this section_
1. Clone repository with submodules: `git clone --recursive https://github.com/MIPT-ILab/mipt-mips.git`
1. Install LibELF and Boost ([instruction](https://github.com/MIPT-ILab/mipt-mips/wiki/Required-libraries))
1. [Build MIPS binutils](https://github.com/MIPT-ILab/mipt-mips/wiki/MIPS-binutils) if you need to build MIPS ELF binaries.
1. In POSIX environment (Linux, OSX, Windows-Ubuntu, and Msys) you can build MIPT-MIPS just by `make mipt-mips` command.
1. WIP on MS Visual Studio support. Currently you can build MIPT-MIPS in command line using `build.cmd` batch file.

### Basic command line options

* `-b <filename>` — provide path to ELF binary file to execute
* `-n <number>` — number of instructions to run
* `-f` — enables functional simulation only
* `-d` — enables detailed output of each cycle

### C++ requirements

MIPT-MIPS uses C++17 features and Boost 1.61. Thus, you have to use compilers of these versions or newer:
* GCC 7
* Clang 5.0
* Apple LLVM 7.3.0
* MS Visual Studio 2017 (Boost 1.66 is required)

Additionally, you have to use CMake 3.8 or higher.

### Testing

_Konstantin, please update this section_
1. Get sure GoogleTest is fetched with other submodules.
1. To run all unit tests in POSIX environment, use `ctest` command.
1. To run all unit tests in MS Visual Studio command line, run `build.cmd` batch file.

----
## About MIPT-MIPS

This project is a part of [ILab](https://mipt-ilab.github.io/) activity at [Moscow Institute of Physics and Technology](http://phystech.edu/) (MIPT).

The main goal of the project is to teach the students the computer architecture through development of a microprocessor implementing the [MIPS](http://en.wikipedia.org/wiki/MIPS32) instruction set in both functional and performance simulators.

### May I contribute?

Yes, if you attend MIPT-MIPS lectures. See our [CONTRIBUTING.md](CONTRIBUTING.md) file for details.
