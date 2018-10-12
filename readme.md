[![Build Status](https://travis-ci.com/MIPT-ILab/mipt-mips.svg?branch=master)](https://travis-ci.com/MIPT-ILab/mipt-mips)
[![Build status](https://ci.appveyor.com/api/projects/status/eungty6us329y8w1/branch/master?svg=true)](https://ci.appveyor.com/project/miptilab/mipt-mips/branch/master)
[![codecov](https://codecov.io/gh/MIPT-ILab/mipt-mips/branch/master/graph/badge.svg)](https://codecov.io/gh/MIPT-ILab/mipt-mips)
[![Github Releases](https://img.shields.io/github/release/MIPT-ILab/mipt-mips.svg)](https://github.com/MIPT-ILab/mipt-mips/releases)

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

## Requirements

MIPT-MIPS uses C++17 features and Boost 1.61. Thus, you have to use compilers of these versions or newer:
* GCC 7
* Clang 5.0
* Apple LLVM 7.3.0
* MS Visual Studio 2017 (Boost 1.66 and CMake 3.10.2 are required)

[Install Boost](https://github.com/MIPT-ILab/mipt-mips/wiki/Required-libraries) before building the project.

To work with MIPS traces, you need to install MIPS binutils. Please follow [our manual](https://github.com/MIPT-ILab/mipt-mips/wiki/MIPS-binutils) if you are using Linux, OS X, or Windows.

MIPT-MIPS build system is CMake. You should install CMake 3.9 or higher.
Check [our Wiki page](https://github.com/MIPT-ILab/mipt-mips/wiki/CMake) to get more details about CMake.
Users of IDE (Visual Studio, Eclipse, CodeBlocks etc.) may generate project files with CMake as well.

## Workflow example

#### Clone
  1. Check that your environment meets all the requirements above.
  1. Clone repository with submodules: `git clone --recursive https://github.com/MIPT-ILab/mipt-mips.git`
#### Build
  1. Go to `traces` subdirectory and build MIPS traces just by typing `make`
  1. Create a **new** build directory somewhere, then cd into it: `mkdir /path/to/your/build/directory`
  1. Go to the build directory: `cd /path/to/your/build/directory`
  1. Run `cmake /path/to/mipt-mips/simulator` to configure CMake
  1. Run `make` to get the `mipt-mips` binary file
  1. If you changed some source code files, just type `make` to rebuild project
#### Run
  1. Now you can run simulation: `./mipt-mips -b /path/to/mipt-mips/traces/<tracename>.out`
  1. See more command line options in the paragraph below
#### Test
  1. To run all unit tests, call `make unit-tests && ctest --verbose -C Release` from your build directory.

## Command line options

### Basic options

* `-I` — modeled ISA, default option is "mips32"
* `-b <filename>` — provide path to ELF binary file to execute.
* `-n <number>` — number of instructions to run. If omitted, simulation continues until halting system call or jump to `null` is executed.
* `-f` — enables functional simulation only
* `-d` — enables detailed output of each cycle

### Performance mode options

#### Branch prediction
* `--bp-mode` — prediction mode. Check supported modes in [manual](https://github.com/MIPT-ILab/mipt-mips/wiki/BPU-model).
* `--bp-size` — branch prediction cache size (amount of tracked branch instructions)
* `--bp-ways` — # of ways in branch prediction cache

#### Instruction cache
* `--icache-size` — instruction cache size in bytes
* `--icache-ways` — # of ways in instruction cache
* `--icache-line-size` — line size of instruction cache

#### Complex pipeline
* `--complex-alu-latency` - number of execution stages required for complex arithmetic instructions to be complete

## About MIPT-MIPS

[![](https://github.com/MIPT-ILab/ca-lectures/blob/master/images/mipt-eng.jpg?raw=true)](https://mipt.ru/english)
[![](https://github.com/MIPT-ILab/ca-lectures/blob/master/images/drec.gif?raw=true)](https://mipt.ru/drec/)
[![Logo](https://avatars2.githubusercontent.com/u/13999586?s=100)](https://mipt-ilab.github.io/)

This project is a part of [ILab](https://mipt-ilab.github.io/) activity at [Moscow Institute of Physics and Technology](http://phystech.edu/) (MIPT).

The main goal of the project is to teach the students the computer architecture through development of a microprocessor implementing the [MIPS](http://en.wikipedia.org/wiki/MIPS32) instruction set in both functional and performance simulators.

### May I contribute?

Yes, if you attend MIPT-MIPS lectures. See our [contributing.md](contributing.md) file for details.

