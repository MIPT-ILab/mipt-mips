MIPT-MIPS / MIPT-V is a pre-silicon simulator of MIPS and RISC-V CPUs It is able to measure _performance_ of program running on CPU, thus taking best features of RTL and common functional simulation.

This project is a part of [MIPT ILab](https://mipt-ilab.github.io/) activity at [Moscow Institute of Physics and Technology](http://phystech.edu/) (MIPT).
The project is being developed by a group of students under the guidance of Intel employees.
Additionally, it is a supporting project to MIPT lectures on Computer Architecture and Software Development courses.

Lectures and seminars are delivered each **Friday 18:30, room 108 of RT building**.

## Computer Architecture
YouTube playlist: [MIPT-MIPS 2018/2019](https://www.youtube.com/playlist?list=PLhJz09Rm90Dowrv0kKhryUSSXUsI1N7Kq)

Date | Event name
:----: | ----------------------------
13 Sep | **Promotion event**<br/> • Slides: [russian version](https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2018/Promotion.pptx?raw=true)
19 Sep | **Layers of Computer Science, ISA and uArch**<br/> • Slides: [L1](https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2018/Lecture%201%20-%20Layers%20of%20Computer%20Science.pptx?raw=true)
03 Oct | **CMOS and Combinational Circuits**<br/> • Slides: [L2 - CMOS](https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2018/Lecture%202%20-%20Integrated%20Circuits%20Basics.pptx?raw=true), [L3 - Combinational Circuits](https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2018/Lecture%203%20-%20Combinational%20Circuits.pptx?raw=true), [YouTube](https://youtu.be/zDldV01U5MQ) <br/> • Homework: Install [Logisim](http://www.cburch.com/logisim/index.html) and look inside [ALU](https://github.com/MIPT-ILab/ca-lectures/blob/master/logisim/alu.circ) and [CMOS](https://github.com/MIPT-ILab/ca-lectures/blob/master/logisim/cmos.circ) examples.
17 Oct | **Sequential Circuits**<br/> • Slides: [L4](https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2018/Lecture%204%20-%20Sequential%20Circuits.pptx?raw=true), [YouTube](https://youtu.be/SlFY1iJ-63Q) <br/> • Homework: Study implementation of [SR-latch](https://github.com/MIPT-ILab/ca-lectures/blob/master/logisim/rs.circ), [T-latch counter](https://github.com/MIPT-ILab/ca-lectures/blob/master/logisim/counter.circ) and [Mem array](https://github.com/MIPT-ILab/ca-lectures/blob/master/logisim/array2x4.circ) with [Logisim](http://www.cburch.com/logisim/index.html). Refresh scheme of [ALU](https://github.com/MIPT-ILab/ca-lectures/blob/master/logisim/alu.circ) and look inside [Flip-flop ALU](https://github.com/MIPT-ILab/ca-lectures/blob/master/logisim/alu-filp-flop.circ).
31 Oct | **Single-cycle implementation of MIPS/RISC-V CPU**<br/> • Slides: [L5](https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2018/Lecture%205%20-%20Single-cycle%20implementation%20of%20MIPS%20CPU.pptx?raw=true), [YouTube](https://youtu.be/_okzQg7OxP0) <br/> • Homework: Read wiki's [MIPS section](https://github.com/MIPT-ILab/mipt-mips/wiki#mips). Recap single-cycle data path (foils 20-26). Get familiar with single-cycle model: [simulator](https://github.com/MIPT-ILab/mipt-mips/tree/master/simulator) & [wiki](https://github.com/MIPT-ILab/mipt-mips/wiki/Functional-Simulator). Study RISC-V [opened issues](https://github.com/MIPT-ILab/mipt-mips/issues?q=is:issue+is:open+sort:updated-asc+label:%22S1+%E2%80%94+ISA%22+no:assignee).
14 Nov | **Pipeline**<br/> • Slides: [L6](https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2018/Lecture%206%20-%20Pipelining.pptx?raw=true), [YouTube](https://youtu.be/QHj2fBxSpUU) <br/> • Homework: Walk through documentation on [performance simulator](https://github.com/MIPT-ILab/mipt-mips/wiki/Performance-simulator) and [ports concepts](https://github.com/MIPT-ILab/mipt-mips/wiki/Communication-between-modules-through-ports). Read original paper on [ASIM Perf Model Framework](http://www.ckluk.org/ck/papers/asim_ieeecomputer.pdf).
28 Nov | **Branch prediction**<br/> • Slides: [L7](https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2018/Lecture%207%20-%20Branch%20prediction.pptx?raw=true), [YouTube](https://youtu.be/XzfcQ8VgnU8)
12 Dec | **Advanced pipelining**<br/> • Slides: [L8](https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2018/Lecture%208%20-%20Advanced%20Pipelining.pptx?raw=true), [YouTube](https://youtu.be/pUw2sU9RZSY)
19 Dec | **First Test**
08 Feb | **Superscalar CPU and multicore systems**<br/> • Slides: [L9](https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2018/Lecture%209%20-%20Superscalar%20CPU%20and%20multicore%20systems.pptx?raw=true), no video was recorded.
15/22<br/>Feb | **Caches, pts I and II**<br/> • Slides: [L10-11](https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2018/Lecture%2011%20-%20Caches%2C%20part%202.pptx), [YouTube L10](https://youtu.be/S8FopsF86VQ), [YouTube L11](https://youtu.be/twQdcDt94DM)
15 Mar | **Out-of-order execution**<br/> • Slides: [L12](https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2018/Lecture%2012%20-%20Out-of-order%20execution.pptx?raw=true), [YouTube](https://youtu.be/rmnHFtVwjBY)
22 Mar | **Out-of-order execution: load-store architecture**<br/> • Slides: [L13](https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2018/Lecture%2013%20-%20Load-Store%20Architecture.pptx?raw=true), [YouTube](https://youtu.be/zMAlhLq5O3I)
29 Mar | **Virtual Memory**<br/> • Slides: [L14](https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2018/Lecture%2014%20-%20Virtual%20Memory.pptx?raw=true), [YouTube](https://youtu.be/WiobWMd4tuo)
05 Apr | **Multiprocessor Systems**<br/> • Slides: [L15](https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2018/Lecture%2015%20-%20Multiprocessor%20Systems.pptx?raw=true), [YouTube](https://youtu.be/_QSnEvSbyc4)
12 Apr | **Simultaneous Multithreading**<br/> • Slides: [L16](https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2018/Lecture%2016%20-%20Simultaneous%20Multithreading.pptx?raw=true), no video was recorded.
19 Apr |
26 Apr | **Final Test**

Instructions to use downloaded material can be found on [wiki page](https://github.com/MIPT-ILab/mipt-mips/wiki/Instructions-to-use-downloaded-lectures).
You may find lectures of previous year [in our archive](https://github.com/MIPT-ILab/mipt-mips/wiki/Lectures-on-Computer-Architecture-in-2017).

## Software Development

Date | Event name
:----: | ----------------------------
13 Sep | **Promotion event**<br/> • Slides: [russian version](https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2018/Promotion.pptx?raw=true)
26 Sep | **Version control system**<br/> • [Slides](https://github.com/MIPT-ILab/sd-lectures/blob/master/mipt-mips/2018/Lecture%201%20-%20Introduction.%20Version%20Control%20System.pptx?raw=true), [YouTube](https://www.youtube.com/watch?v=HOeMi9dRD58)<br/> • Homework: read our [our cheat sheet](https://github.com/MIPT-ILab/mipt-mips/wiki/Git-&-GitHub-cheat-sheet) and contribute to members.md<br/> • Further reading: *Pro Git* ([Rus](https://git-scm.com/book/ru/v2), [Eng](https://git-scm.com/book/en/v2)), *Git Flight Rules* ([Rus](https://github.com/k88hudson/git-flight-rules/blob/master/README_ru.md), [Eng](https://github.com/k88hudson/git-flight-rules))
10 Oct | **Build, Makefiles, and CMake**<br/> • [Slides](https://github.com/MIPT-ILab/sd-lectures/blob/master/mipt-mips/2018/Lecture%202%20-%20Build,%20Makefiles,%20CMake.pptx?raw=true), [YouTube](https://youtu.be/2t_qkJ67nAE)<br/> • Homework: use Make or CMake to build your project, build MIPT-MIPS.
24 Oct | **Continuous Integration**<br/> • [Slides](https://github.com/MIPT-ILab/sd-lectures/blob/master/mipt-mips/2018/Lecture%203%20-%20Continious%20Integration.pptx?raw=true), [YouTube](https://youtu.be/H8xRux-Tmm4)<br/> • Homework: install Travis CI for your project to run your Make or CMake builds.
07 Nov | **Code form**<br/> • [Slides](https://github.com/MIPT-ILab/sd-lectures/blob/master/mipt-mips/2018/Lecture%204%20-%20Code%20Form.pptx?raw=true), [YouTube](https://youtu.be/kFZL-WdxN5Q)
21 Nov | **Unit testing**<br/> • [Slides](https://github.com/MIPT-ILab/sd-lectures/blob/master/mipt-mips/2018/Lecture%205%20-%20Unit%20Tests.pptx?raw=true), [YouTube](https://youtu.be/yP6rUwjBEqI)
05 Dec | **Test-driven development**<br/> • [Slides](https://github.com/MIPT-ILab/sd-lectures/blob/master/mipt-mips/2018/Lecture%206%20-%20Test%20Driven%20Development.pptx?raw=true), [YouTube](https://youtu.be/hMmPlqUbeQM)

In addition, repository contains a lot of useful [Wiki manuals](https://github.com/MIPT-ILab/mipt-mips/wiki) about Git, MIPS, C++, Make utilites and so on.

## Students Contribution Score

MIPT | Name | Score | Comments
:----: |:---- | ------------------------------ | ------------------------------
611 | Denis Los | 19 | [#98](https://github.com/MIPT-ILab/mipt-mips/issues/98), [#99](https://github.com/MIPT-ILab/mipt-mips/issues/99), [#77](https://github.com/MIPT-ILab/mipt-mips/issues/77), [#221](https://github.com/MIPT-ILab/mipt-mips/issues/221), [#225](https://github.com/MIPT-ILab/mipt-mips/issues/225), [#221](https://github.com/MIPT-ILab/mipt-mips/issues/221), [#70](https://github.com/MIPT-ILab/mipt-mips/issues/70), [#31](https://github.com/MIPT-ILab/mipt-mips/issues/31), [#20](https://github.com/MIPT-ILab/mipt-mips/issues/20) |
**718** | **Andrey Agrachev** | **4 + 9** | **[#563](https://github.com/MIPT-ILab/mipt-mips/issues/563), [#465](https://github.com/MIPT-ILab/mipt-mips/issues/465), ¼[#626](https://github.com/MIPT-ILab/mipt-mips/issues/626), [#830](https://github.com/MIPT-ILab/mipt-mips/issues/830), [#248](https://github.com/MIPT-ILab/mipt-mips/issues/248), [#879](https://github.com/MIPT-ILab/mipt-mips/issues/879), [#821](https://github.com/MIPT-ILab/mipt-mips/issues/821)** |
**714** | **Yauheni Sharamed** | **6 + 7** | **[#599](https://github.com/MIPT-ILab/mipt-mips/issues/599), [#724](https://github.com/MIPT-ILab/mipt-mips/issues/724), [#649](https://github.com/MIPT-ILab/mipt-mips/issues/649), [#910](https://github.com/MIPT-ILab/mipt-mips/issues/910), [#925](https://github.com/MIPT-ILab/mipt-mips/issues/925)** |
618 | Alexander Misevich | 13 | [#37](https://github.com/MIPT-ILab/mipt-mips/issues/37), [#216](https://github.com/MIPT-ILab/mipt-mips/issues/216), [#201](https://github.com/MIPT-ILab/mipt-mips/issues/201), [#246](https://github.com/MIPT-ILab/mipt-mips/issues/246), [#124](https://github.com/MIPT-ILab/mipt-mips/issues/124), [#18](https://github.com/MIPT-ILab/mipt-mips/issues/18), [#283](https://github.com/MIPT-ILab/mipt-mips/issues/283), [#277](https://github.com/MIPT-ILab/mipt-mips/issues/277) |
512 | George Korepanov | 12 | [#28](https://github.com/MIPT-ILab/mipt-mips/issues/28), [#48](https://github.com/MIPT-ILab/mipt-mips/issues/48), [#49](https://github.com/MIPT-ILab/mipt-mips/issues/49), [#26](https://github.com/MIPT-ILab/mipt-mips/issues/26), [#60](https://github.com/MIPT-ILab/mipt-mips/issues/60), [#45](https://github.com/MIPT-ILab/mipt-mips/issues/45), [#73](https://github.com/MIPT-ILab/mipt-mips/issues/73) |
**711** | **Vyacheslav Kompan** | **10 + 1** | **[#354](https://github.com/MIPT-ILab/mipt-mips/issues/354), [#662](https://github.com/MIPT-ILab/mipt-mips/issues/662), [#122](https://github.com/MIPT-ILab/mipt-mips/issues/122), ½[#290](https://github.com/MIPT-ILab/mipt-mips/issues/290)**  |
**712** | **Egor Bova** | **+ 10** | **½[#530](https://github.com/MIPT-ILab/mipt-mips/issues/530), [#235](https://github.com/MIPT-ILab/mipt-mips/issues/235), [#461](https://github.com/MIPT-ILab/mipt-mips/issues/461)** |
**711** | **Yan Logovskiy** | **4 + 5** | **[#91](https://github.com/MIPT-ILab/mipt-mips/issues/91), [#92](https://github.com/MIPT-ILab/mipt-mips/issues/92), [⅓#383](https://github.com/MIPT-ILab/mipt-mips/issues/383), [#506](https://github.com/MIPT-ILab/mipt-mips/issues/506), ¼[#613](https://github.com/MIPT-ILab/mipt-mips/issues/613)** |
616 | Kirill Nedostoev | 9 | [#147](https://github.com/MIPT-ILab/mipt-mips/issues/147), [#87](https://github.com/MIPT-ILab/mipt-mips/issues/87), [#93](https://github.com/MIPT-ILab/mipt-mips/issues/93), [#215](https://github.com/MIPT-ILab/mipt-mips/issues/215), [#214](https://github.com/MIPT-ILab/mipt-mips/issues/214) |
**717** | **Vsevolod Pukhov** | **4 + 2** | **[#589](https://github.com/MIPT-ILab/mipt-mips/issues/589), ½[#530](https://github.com/MIPT-ILab/mipt-mips/issues/530), [#602](https://github.com/MIPT-ILab/mipt-mips/issues/602)** |
616 | Alexander Timofeev | 5 | [#69](https://github.com/MIPT-ILab/mipt-mips/issues/69), [#132](https://github.com/MIPT-ILab/mipt-mips/issues/132), [#215](https://github.com/MIPT-ILab/mipt-mips/issues/215), [#214](https://github.com/MIPT-ILab/mipt-mips/issues/214) |
616 | Konstantin Soshin | 5 | [#165](https://github.com/MIPT-ILab/mipt-mips/issues/165) |
616 | Andrew Sultan | 4 | [#282](https://github.com/MIPT-ILab/mipt-mips/issues/282), [#132](https://github.com/MIPT-ILab/mipt-mips/issues/132) |
518 | Alexander Seppar | 4 | [fc526cb8](https://github.com/MIPT-ILab/ca-lectures/commit/fc526cb8f59bc6d9a399f453b417afc45c21012e), [#25](https://github.com/MIPT-ILab/mipt-mips/issues/25), [#52](https://github.com/MIPT-ILab/mipt-mips/issues/52) |
614 | Alexey Steksov | 3 | [#152](https://github.com/MIPT-ILab/mipt-mips/issues/152), [T#3](https://github.com/MIPT-ILab/mips-traces/issues/3) |
**717** | **Ivan Startsev** | **2** | **[#525](https://github.com/MIPT-ILab/mipt-mips/issues/525)** |
**717** | **Arsen Davtyan** | **2** | **[#660](https://github.com/MIPT-ILab/mipt-mips/issues/660), [#643](https://github.com/MIPT-ILab/mipt-mips/issues/643)** |
612 | Danil Yarovoy | 2 | [T#4](https://github.com/MIPT-ILab/mips-traces/issues/4) |
516 | Ivan Korostelev | 1 | [#32](https://github.com/MIPT-ILab/mipt-mips/issues/32) |

List of [Upcoming contributions](https://github.com/MIPT-ILab/mipt-mips/issues/assigned/*)

## About us

The project is mentored by [Igor Smirnov](https://github.com/igorsmir-ilab), [Kirill Korolev](https://github.com/kkorolev), and [Oleg Ladin](https://github.com/olegladin) under [Pavel Kryukov](https://github.com/pavelkryukov)'s supervising. For more information, check [this wiki page](https://github.com/MIPT-ILab/mipt-mips/wiki/About-Us) and [MIPT ILab site](https://mipt.ru/drec/about/ilab/)

Additional repositories:
* [MIPS traces](https://github.com/MIPT-ILab/mips-traces)
* [Lectures storage](https://github.com/MIPT-ILab/ca-lectures)
* [MIPS-MIPS-CEN64](https://github.com/MIPT-ILab/cen64)
* _[Branches imported from GoogleCode SVN](https://github.com/MIPT-ILab/mipt-mips-old-branches)_
* _[MSVC-compatible LibELF](https://github.com/MIPT-ILab/libelf)_
