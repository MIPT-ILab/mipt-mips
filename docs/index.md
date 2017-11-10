MIPT-MIPS is a pre-silicon simulator of MIPS CPU. It is able to measure _performance_ of program running on CPU, thus taking best features of RTL and common functional simulation.

This project is a part of [MIPT ILab](https://mipt-ilab.github.io/) activity at [Moscow Institute of Physics and Technology](http://phystech.edu/) (MIPT). The project is being developed by a group of students under the guidance of Intel employees.

## Lectures on Computer Architecture

MIPT-MIPS is a supporting project to MIPT lectures on Computer Architecture.

Lectures are delivered every Friday at 18:30 in **304 RT building** of MIPT.

### Term 1: CS Basics and Components

 Date | Event | Hometask
 :-: |:---- |:----------------
 15<br/>Sep | **Promotion event**<br/>• [Rus text, no audio, 8 MB](https://github.com/MIPT-ILab/ca-lectures/blob/master/2017/Promotion__15_Sep.pptx?raw=true)<br/>• [Eng text, no audio, 8 MB](https://github.com/MIPT-ILab/ca-lectures/blob/master/2017/Promotion__15_Sep_Eng.pptx?raw=true)  | 
 22<br/>Sep | **L1: Project intro.<br/>Basics of Simulator Development**<br/>• [Rus/Eng text, no audio, 0.7 MB](https://github.com/MIPT-ILab/ca-lectures/blob/master/2017/Lecture_01__22_Sep__Project_Introduction__Rus_text__No_audio.pptx?raw=true) | • Add your name to members.txt. Follow [this instruction](https://github.com/MIPT-ILab/mipt-mips/wiki/Git-&-GitHub-cheat-sheet).<br/>• Start reading of 1st and 2nd chapters of [„Teach Yourself C++“](http://lib.mipt.ru/book/27518/).
 29<br/>Sep | **L2: Layers of Computer Science.<br/>ISA and uArch**<br/>• [Eng text, no audio, 9 MB](https://github.com/MIPT-ILab/ca-lectures/blob/master/2017/Lecture_02__29_Sep__Layers_of_Computer_Science__ISA_and_uArch___Eng_text__No_audio.pptx?raw=true) | • Read our [README.md](https://github.com/MIPT-ILab/mipt-mips/blob/master/README.md), build MIPT-MIPS, and run some traces.<br/>• Take a look at [MIPS ISA](https://github.com/MIPT-ILab/mipt-mips/wiki/MIPS-Instruction-Set)
 06<br/>Oct | **L3: Combinational Circuits**<br/>• [Eng text, no audio, 0.9 MB](https://github.com/MIPT-ILab/ca-lectures/blob/master/2017/Lecture_03__06_Oct__Combinational_Circuits__Eng_text__No_audio.pptx?raw=true) | • Install [Logisim](http://www.cburch.com/logisim/index.html), reproduce some schemes we discussed, look inside [ALU](https://github.com/MIPT-ILab/ca-lectures/blob/master/logisim/alu.circ) example
 13<br/>Oct | **L4: Integrated Circuits Basics**<br/>• [Eng text, no audio, 1.8 MB](https://github.com/MIPT-ILab/ca-lectures/blob/master/2017/Lecture_04__13_Oct__Integrated_Circuits_Basics__Eng_text__No_audio.pptx?raw=true) | • Watch YouTube videos about CPU production: [1](http://www.youtube.com/watch?v=qm67wbB5GmI), [2](https://www.youtube.com/watch?v=d9SWNLZvA8g)<br/>• Look inside [CMOS](https://github.com/MIPT-ILab/ca-lectures/blob/master/logisim/cmos.circ) Logisim example  |
 20<br/>Oct | **L5: Sequential Circuits**<br/>• [Eng text, no audio, 0.4 MB](https://github.com/MIPT-ILab/ca-lectures/blob/master/2017/Lecture_05__20_Oct__Sequential_Circuits__Eng_text__No_audio.pptx?raw=true) | • Look inside [RS trigger](https://github.com/MIPT-ILab/ca-lectures/blob/master/logisim/rs.circ), [ALU + flip-flop](https://github.com/MIPT-ILab/ca-lectures/blob/master/logisim/alu-filp-flop.circ), [counter](https://github.com/MIPT-ILab/ca-lectures/blob/master/logisim/counter.circ), and [2x4 array](https://github.com/MIPT-ILab/ca-lectures/blob/master/logisim/array2x4.circ) Logisim examples<br/>• Get ready for the upcoming test! |
 27<br/>Oct | **Test 1**<br/> • Review **L2−L5** material<br/>(excluding semiconductors, n/p-doping and transistor structure)<br/> • Review **1st** and **2nd** chapters of Schildt C++ manual<br/> • Review main Git commands<br/> | • Continue reading [„Teach Yourself C++“](http://lib.mipt.ru/book/27518/), chapters 3-6|
 
### Term 2: MIPS as pipeline-friendly ISA

 Date | Event | Hometask
 :-: |:---- |:----------------
 03<br/>Nov | **L6: Single-Cycle Implementation of MIPS CPU**<br/>• [Eng text, no audio, 0.5 MB](https://github.com/MIPT-ILab/ca-lectures/blob/master/2017/Lecture_06__03_Nov__Single-Cycle_Implementation_of_MIPS_CPU__Eng_text__No_audio.pptx?raw=true) | • Take a look at [MIPS ISA](https://github.com/MIPT-ILab/mipt-mips/wiki/MIPS-Instruction-Set)<br/>• Check simulator internals: [MIPS disasm](https://github.com/MIPT-ILab/mipt-mips/wiki/MIPS-disassembler) and [Functional Simulator](https://github.com/MIPT-ILab/mipt-mips/wiki/Functional-simulator)<br/>• Assign yourself to [MIPS ISA tasks](https://github.com/MIPT-ILab/mipt-mips/milestone/1) and [MIPS traces tasks](https://github.com/MIPT-ILab/mips-traces/issues)|
 10<br/>Nov | **L7: Pipeline**<br/>• [Eng text, no audio, 0.8 MB](https://github.com/MIPT-ILab/ca-lectures/blob/master/2017/Lecture_07__10_Nov__Pipelining__No_audio.pptx?raw=true) | • Check simulator internals: [Ports](https://github.com/MIPT-ILab/mipt-mips/wiki/https://github.com/MIPT-ILab/mipt-mips/wiki/Communication-between-modules-through-ports) and [Performance Simulator](https://github.com/MIPT-ILab/mipt-mips/wiki/Performance-simulator)<br/>• Assign yourself to [PerfSim tasks] (https://github.com/MIPT-ILab/mipt-mips/milestone/3)|
 17<br/>Nov | **L8: Branch Prediction** | |
 24<br/>Nov | **_No lecture_** | |

You may find previous lectures with audio narrations: the most complete set from [2015](https://github.com/MIPT-ILab/mipt-mips/wiki/Lectures-on-the-computer-architecture-in-2015) and others: [2016](https://github.com/MIPT-ILab/mipt-mips/wiki/Lectures-on-the-computer-architecture-in-2016), [2014](https://github.com/MIPT-ILab/mipt-mips/wiki/Lectures-on-the-computer-architecture-in-2014), [2013](https://github.com/MIPT-ILab/mipt-mips/wiki/Lectures-on-the-computer-architecture-in-2013), [2012](https://github.com/MIPT-ILab/mipt-mips/wiki/Lectures-on-the-computer-architecture-in-2012).

Instructions to use downloaded material can be found on [wiki page](https://github.com/MIPT-ILab/mipt-mips/wiki/Instructions-to-use-downloaded-lectures).

In addition, repository contains a lot of useful [Wiki manuals](https://github.com/MIPT-ILab/mipt-mips/wiki) about Git, MIPS, C++, Make utilites and so on.

## Students Contribution Score

N | Year | Name | Score | Comments
:-: |:----: |:----: | ------------------------------ | ------------------------------
1 | 2016/2017 | Georgiy Korepanov | 12 | [#28](https://github.com/MIPT-ILab/mipt-mips/issues/28), [#48](https://github.com/MIPT-ILab/mipt-mips/issues/48), [#49](https://github.com/MIPT-ILab/mipt-mips/issues/49), [#26](https://github.com/MIPT-ILab/mipt-mips/issues/26), [#60](https://github.com/MIPT-ILab/mipt-mips/issues/60), [#45](https://github.com/MIPT-ILab/mipt-mips/issues/45), [#73](https://github.com/MIPT-ILab/mipt-mips/issues/73) |
2 | 2016/2017 | Alexandr Seppar | 4 | [fc526cb8](https://github.com/MIPT-ILab/ca-lectures/commit/fc526cb8f59bc6d9a399f453b417afc45c21012e), [#25](https://github.com/MIPT-ILab/mipt-mips/issues/25), [#52](https://github.com/MIPT-ILab/mipt-mips/issues/52) |
3 | 2017/2018 | Kirill Nedostoev | 2 | [#147](https://github.com/MIPT-ILab/mipt-mips/issues/147) |
4 | 2017/2018 | Danil Yarovoy | 2 |[T#4](https://github.com/MIPT-ILab/mips-traces/issues/4) |
5 | 2017/2018 | Denis Los | 1 | [#98](https://github.com/MIPT-ILab/mipt-mips/issues/98) |
6 | 2017/2018 | Aleksandr Misevich | 1 | [#37](https://github.com/MIPT-ILab/mipt-mips/issues/37) |
7 | 2017/2018 | Alexey Steksov | 1 | [#152](https://github.com/MIPT-ILab/mipt-mips/issues/152) |
8 | 2016/2017 | Ivan Korostelev | 1 | [#32](https://github.com/MIPT-ILab/mipt-mips/issues/32) |



## About us

The project is mentored by [Pavel Kryukov](https://github.com/pavelkryukov) and [Igor Smirnov](https://github.com/igorsmir-ilab). For more information, check [this wiki page](https://github.com/MIPT-ILab/mipt-mips/wiki/About-Us) and [MIPT ILab site](https://mipt.ru/drec/about/ilab/)

Additional repositories:
* [MIPS traces](https://github.com/MIPT-ILab/mips-traces)
* [Branches imported from GoogleCode SVN](https://github.com/MIPT-ILab/mipt-mips-old-branches)
* [Lectures storage](https://github.com/MIPT-ILab/ca-lectures)
* [MSVC-compatible LibELF](https://github.com/MIPT-ILab/libelf)
