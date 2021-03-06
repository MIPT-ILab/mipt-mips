MIPT-MIPS / MIPT-V is a pre-silicon simulator of MIPS and RISC-V CPUs It is able to measure _performance_ of program running on CPU, thus taking best features of RTL and common functional simulation.

This project is a part of [MIPT ILab](https://mipt-ilab.github.io/) activity at [Moscow Institute of Physics and Technology](http://phystech.edu/) (MIPT).
The project is being developed by a group of students under the guidance of Intel employees.
Additionally, it is a supporting project to MIPT lectures on Computer Architecture and Software Development courses.

## Computer Architecture 2020/2021

Lectures are given online each **Monday, 10:45-12:10**.
A link to join the meeting: [link](https://meet.google.com/fjn-ycma-ucb).

[Intro]:     https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2020/Course%20Introduction.pptx?raw=true
[L1]:        https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2020/Lecture%201%20-%20Layers%20of%20Computer%20Architecture.%20ISA%20and%20uArch.pptx?raw=true
[L2]:        https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2020/Lecture%202%20-%20ISA%20and%20uArch.pptx?raw=true
[L3]:        https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2020/Lecture%203%20-%20Integrated%20Circuits.pptx?raw=true
[L4]:        https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2020/Lecture%204%20-%20CMOS.pptx?raw=true
[L5]:        https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2020/Lecture%205%20-%20Combinational%20Circuits.pptx?raw=true
[L6]:        https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2020/Lecture%206%20-%20Sequential%20Circuits.pptx?raw=true
[L7]:        https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2020/Lecture%207%20-%20RISC-V%20ISA.pptx?raw=true
[L8]:        https://github.com/MIPT-ILab/ca-lectures/raw/master/mipt-mips/2020/Lecture%208%20-%20RISC-V%20Single-Cycle%20implementation.pptx?raw=true
[L9]:        https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2020/Lecture%209%20-%20Pipelining.pptx?raw=true
[L10]:       https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2020/Lecture%2010%20-%20Data%20Hazards.pptx?raw=true
[L11]:       https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2020/Lecture%2011%20-%20Control%20Hazards.%20Branch%20Prediction.pptx?raw=true
[L12]:       https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2020/Lecture%2012%20-%20Advanced%20Pipelining.pptx?raw=true
[L13]:       https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2020/Lecture%2013%20-%20Caches%2C%20part%201.pptx?raw=true
[L14]:       https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2020/Lecture%2014%20-%20Caches%2C%20part%202.pptx?raw=true
[L15]:       https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2020/Lecture%2015%20-%20Caches%2C%20part%203.pptx?raw=true
[L16]:       https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2020/Lecture%2016%20-%20Virtual%20Memory.pptx?raw=true
[L17]:       https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2020/Lecture%2017%20-%20ILP.%20Superscalar.%20OOO%20-%20part%201.pptx?raw=true
[L18]:       https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2020/Lecture%2018%20-%20OOO%20-%20part%202.pptx?raw=true
[L19]:       https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2020/Lecture%2019%20-%20OOO%20part%203,%20loads%20and%20stores%20(Load-Store%20Architecture).pptx?raw=true
[L20]:       https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2020/Lecture%2020%20-%20Introduction%20to%20Software%20Development.pptx?raw=true
[L21]:       https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2020/Lecture%2021%20-%20TLP%20-%20part%201.pptx?raw=true
[L22]:       https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2020/Lecture%2022%20-%20TLP%20-%20part%202%20(Multithreading%2C%20Synchronization).pptx?raw=true
[L7-VIDEO]:  https://drive.google.com/file/d/1ITi6EVFnVs2rUZxU5_IurgNNUN0WI6TC/view?usp=sharing
[L8-VIDEO]:  https://drive.google.com/file/d/1uk1dXSwToPq2yj0leAvXCVwpYBcntXjv/view?usp=sharing
[L10-VIDEO]: https://drive.google.com/file/d/1-Z7IkpZVhrrMSNu-OFcXcr-XpXUP9OmK/view?usp=sharing
[L11-VIDEO]: https://drive.google.com/file/d/1UCMwKX36BR9Jx1eJseWmI4ykDIjxJ7Kp/view?usp=sharing
[L12-VIDEO]: https://drive.google.com/file/d/1PlGlKrJ7FkMLR-GX5TfhQDc6W0ttjLv3/view?usp=sharing
[L13-VIDEO]: https://drive.google.com/file/d/1sOfYuI5sdzcnMC6TptBTJwPALNHFxaCZ/view?usp=sharing
[L14-VIDEO]: https://drive.google.com/file/d/1ZJm1iFeBG47pk65VMo9vFJlWBCDIEykx/view?usp=sharing
[L15-VIDEO]: https://drive.google.com/file/d/1AYhWqXkcvT_vN6yxiVsPSz62I1ysfhjE/view?usp=sharing
[L16-VIDEO]: https://drive.google.com/file/d/1qZDI3H3kwWf8CO8RebRHb4zh4zibmDgP/view?usp=sharing
[L17-VIDEO]: https://drive.google.com/file/d/1O1FR49W744vYbSI9h--JKvaaHnEH2lUQ/view?usp=sharing
[L18-VIDEO]: https://drive.google.com/file/d/14RmD57ppQLJBlE4e9Aujnu2lNr2j8SIE/view?usp=sharing
[L19-VIDEO]: https://drive.google.com/file/d/13MRpnt_MVITdCvsZEFH_zUc8uD6_6QIX/view?usp=sharing
[L20-VIDEO]: https://drive.google.com/file/d/1unBU5rmZhRLt4W9G1mIocH7NL4HQvQFu/view?usp=sharing
[L21-VIDEO]: https://drive.google.com/file/d/1Jogk5m2QnAp84LS9dOfSe-Q2cepckWWk/view?usp=sharing
[L22-VIDEO]: https://drive.google.com/file/d/1-_DC4k9u8g4Zt90rd89hTx4lT5iL65Oy/view?usp=sharing
[P1]:        https://github.com/MIPT-ILab/ca-lectures/blob/master/mipt-mips/2020/Practice%201%20-%20Combinational%20and%20Sequential%20Circuits.pptx?raw=true

Date   | Type     | Event
:----: | :------: | ------
 7 Sep | Org.     | **Course introduction**<br>                               • Slides: [download][Intro]
 7 Sep | Lecture  | **L1: Layers of Computer Architecture**<br>               • Slides: [download][L1]
14 Sep | Lecture  | **L2: ISA and uArch**<br>                                 • Slides: [download][L2]
14 Sep | Lecture  | **L3: Integrated Circuits**<br>                           • Slides: [download][L3]
21 Sep | Lecture  | **L4: CMOS**<br>                                          • Slides: [download][L4]
21 Sep | Lecture  | **L5: Combinational Circuits**<br>                        • Slides: [download][L5]
28 Sep | Lecture  | **L6: Sequential Circuits**<br>                           • Slides: [download][L6]
 5 Oct | Practice | **P1: Combinational & Sequential Circuits**<br>           • Slides: [download][P1]
12 Oct | Lecture  | **L7: RISC-V ISA**<br>                                    • Slides: [download][L7]<br> • Video: [watch][L7-VIDEO]
19 Oct | Lecture  | **L8: RISC-V Single-Cycle implementation**<br>            • Slides: [download][L8]<br> • Video: [watch][L8-VIDEO]
26 Oct | Lecture  | **L9: Pipelining**<br>                                    • Slides: [download][L9]
 2 Nov | Lecture  | **L10: Data hazards**<br>                                 • Slides: [download][L10]<br> • Video: [watch][L10-VIDEO]
 9 Nov | Lecture  | **L11: Control Hazards. Branch Prediction**<br>           • Slides: [download][L11]<br> • Video: [watch][L11-VIDEO]
23 Nov | Lecture  | **L12: Advanced pipelining**<br>                          • Slides: [download][L12]<br> • Video: [watch][L12-VIDEO]
 8 Feb | Lecture  | **L13: Caches, part 1/3**<br>                             • Slides: [download][L13]<br> • Video: [watch][L13-VIDEO]
15 Feb | Lecture  | **L14: Caches, part 2/3**<br>                             • Slides: [download][L14]<br> • Video: [watch][L14-VIDEO]
22 Feb | Lecture  | **L15: Caches, part 3/3**<br>                             • Slides: [download][L15]<br> • Video: [watch][L15-VIDEO]
 1 Mar | Lecture  | **L16: Virtual memory**<br>                               • Slides: [download][L16]<br> • Video: [watch][L16-VIDEO]
15 Mar | Lecture  | **L17: ILP. Superscalar. OOO - part 1**<br>               • Slides: [download][L17]<br> • Video: [watch][L17-VIDEO]
22 Mar | Lecture  | **L18: OOO - part 2**<br>                                 • Slides: [download][L18]<br> • Video: [watch][L18-VIDEO]
29 Mar | Lecture  | **L19: OOO - part 3, loads and stores**<br>               • Slides: [download][L19]<br> • Video: [watch][L19-VIDEO]
 5 Apr | Lecture  | **L20: Introduction to Software Development**<br>         • Slides: [download][L20]<br> • Video: [watch][L20-VIDEO]
12 Apr | Lecture  | **L21: TLP (part 1)**<br>                                 • Slides: [download][L21]<br> • Video: [watch][L21-VIDEO]
19 Apr | Lecture  | **L22: TLP (part 2)**<br>                                 • Slides: [download][L22]<br> • Video: [watch][L22-VIDEO]


You may find lectures from the previous year [in our archive](https://github.com/MIPT-ILab/mipt-mips/wiki/Lectures-on-Computer-Architecture-in-2019).

Instructions to use downloaded material can be found on [wiki page](https://github.com/MIPT-ILab/mipt-mips/wiki/Instructions-to-use-downloaded-lectures).

## Software Development _2018/2019_

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

Group | Name | Score | Comments
:----: |:---- | ------------------------------ | ------------------------------
MIPT-611 | Denis Los | 19 | [#98](https://github.com/MIPT-ILab/mipt-mips/issues/98), [#99](https://github.com/MIPT-ILab/mipt-mips/issues/99), [#77](https://github.com/MIPT-ILab/mipt-mips/issues/77), [#221](https://github.com/MIPT-ILab/mipt-mips/issues/221), [#225](https://github.com/MIPT-ILab/mipt-mips/issues/225), [#221](https://github.com/MIPT-ILab/mipt-mips/issues/221), [#70](https://github.com/MIPT-ILab/mipt-mips/issues/70), [#31](https://github.com/MIPT-ILab/mipt-mips/issues/31), [#20](https://github.com/MIPT-ILab/mipt-mips/issues/20) |
MIPT-718 | Andrey Agrachev | 13 | [#563](https://github.com/MIPT-ILab/mipt-mips/issues/563), [#465](https://github.com/MIPT-ILab/mipt-mips/issues/465), ¼[#626](https://github.com/MIPT-ILab/mipt-mips/issues/626), [#830](https://github.com/MIPT-ILab/mipt-mips/issues/830), [#248](https://github.com/MIPT-ILab/mipt-mips/issues/248), [#879](https://github.com/MIPT-ILab/mipt-mips/issues/879), [#821](https://github.com/MIPT-ILab/mipt-mips/issues/821) |
MIPT-714 | Yauheni Sharamed | 13 | [#599](https://github.com/MIPT-ILab/mipt-mips/issues/599), [#724](https://github.com/MIPT-ILab/mipt-mips/issues/724), [#649](https://github.com/MIPT-ILab/mipt-mips/issues/649), [#910](https://github.com/MIPT-ILab/mipt-mips/issues/910), [#925](https://github.com/MIPT-ILab/mipt-mips/issues/925) |
MIPT-618 | Alexander Misevich | 13 | [#37](https://github.com/MIPT-ILab/mipt-mips/issues/37), [#216](https://github.com/MIPT-ILab/mipt-mips/issues/216), [#201](https://github.com/MIPT-ILab/mipt-mips/issues/201), [#246](https://github.com/MIPT-ILab/mipt-mips/issues/246), [#124](https://github.com/MIPT-ILab/mipt-mips/issues/124), [#18](https://github.com/MIPT-ILab/mipt-mips/issues/18), [#283](https://github.com/MIPT-ILab/mipt-mips/issues/283), [#277](https://github.com/MIPT-ILab/mipt-mips/issues/277) |
MIPT-81? | Eric Konks | 12 | [#520](https://github.com/MIPT-ILab/mipt-mips/issues/520), [#1140](https://github.com/MIPT-ILab/mipt-mips/issues/1140), [#1257](https://github.com/MIPT-ILab/mipt-mips/issues/1257)
MIPT-512 | George Korepanov | 12 | [#28](https://github.com/MIPT-ILab/mipt-mips/issues/28), [#48](https://github.com/MIPT-ILab/mipt-mips/issues/48), [#49](https://github.com/MIPT-ILab/mipt-mips/issues/49), [#26](https://github.com/MIPT-ILab/mipt-mips/issues/26), [#60](https://github.com/MIPT-ILab/mipt-mips/issues/60), [#45](https://github.com/MIPT-ILab/mipt-mips/issues/45), [#73](https://github.com/MIPT-ILab/mipt-mips/issues/73) |
MIPT-711 | Vyacheslav Kompan | 11 | [#354](https://github.com/MIPT-ILab/mipt-mips/issues/354), [#662](https://github.com/MIPT-ILab/mipt-mips/issues/662), [#122](https://github.com/MIPT-ILab/mipt-mips/issues/122), ½[#290](https://github.com/MIPT-ILab/mipt-mips/issues/290)  |
MIPT-715 | Rustem Yunusov | 10 | [#901](https://github.com/MIPT-ILab/mipt-mips/issues/901), [#760](https://github.com/MIPT-ILab/mipt-mips/issues/760), [#614](https://github.com/MIPT-ILab/mipt-mips/issues/614) |
MIPT-712 | Egor Bova | 10 | ½[#530](https://github.com/MIPT-ILab/mipt-mips/issues/530), [#235](https://github.com/MIPT-ILab/mipt-mips/issues/235), [#461](https://github.com/MIPT-ILab/mipt-mips/issues/461) |
MIPT-717 | Vsevolod Pukhov | 9 | [#589](https://github.com/MIPT-ILab/mipt-mips/issues/589), ½[#530](https://github.com/MIPT-ILab/mipt-mips/issues/530), [#602](https://github.com/MIPT-ILab/mipt-mips/issues/602), [#130](https://github.com/MIPT-ILab/mipt-mips/issues/130) |
MIPT-711 | Yan Logovskiy | 9 | [#91](https://github.com/MIPT-ILab/mipt-mips/issues/91), [#92](https://github.com/MIPT-ILab/mipt-mips/issues/92), [⅓#383](https://github.com/MIPT-ILab/mipt-mips/issues/383), [#506](https://github.com/MIPT-ILab/mipt-mips/issues/506), ¼[#613](https://github.com/MIPT-ILab/mipt-mips/issues/613) |
MIPT-616 | Kirill Nedostoev | 9 | [#147](https://github.com/MIPT-ILab/mipt-mips/issues/147), [#87](https://github.com/MIPT-ILab/mipt-mips/issues/87), [#93](https://github.com/MIPT-ILab/mipt-mips/issues/93), [#215](https://github.com/MIPT-ILab/mipt-mips/issues/215), [#214](https://github.com/MIPT-ILab/mipt-mips/issues/214) |
**MIPT-91?** | **Vladimir Graudt** | **7** | **[#1410](https://github.com/MIPT-ILab/mipt-mips/issues/1410), [#1409](https://github.com/MIPT-ILab/mipt-mips/issues/1409), [#512](https://github.com/MIPT-ILab/mipt-mips/issues/512), [⅓#463](https://github.com/MIPT-ILab/mipt-mips/issues/1410)**
MIPT-616 | Alexander Timofeev | 5 | [#69](https://github.com/MIPT-ILab/mipt-mips/issues/69), [#132](https://github.com/MIPT-ILab/mipt-mips/issues/132), [#215](https://github.com/MIPT-ILab/mipt-mips/issues/215), [#214](https://github.com/MIPT-ILab/mipt-mips/issues/214) |
MIPT-616 | Konstantin Soshin | 5 | [#165](https://github.com/MIPT-ILab/mipt-mips/issues/165) |
**[BSUIR](https://www.bsuir.by/en/)** | **Alex Kulsha** | **5** | **[#356](https://github.com/MIPT-ILab/mipt-mips/issues/356)** |
**BSUIR** | **Vladislav Zhuravski** | **4** | **[1448](https://github.com/MIPT-ILab/mipt-mips/issues/1448)** |
**BSUIR** | **Andrei Karpyza** | **4** | **[#355](https://github.com/MIPT-ILab/mipt-mips/issues/355)** |
MIPT-616 | Andrew Sultan | 4 | [#282](https://github.com/MIPT-ILab/mipt-mips/issues/282), [#132](https://github.com/MIPT-ILab/mipt-mips/issues/132) |
MIPT-518 | Alexander Seppar | 4 | [fc526cb8](https://github.com/MIPT-ILab/ca-lectures/commit/fc526cb8f59bc6d9a399f453b417afc45c21012e), [#25](https://github.com/MIPT-ILab/mipt-mips/issues/25), [#52](https://github.com/MIPT-ILab/mipt-mips/issues/52) |
MIPT-614 | Alexey Steksov | 3 | [#152](https://github.com/MIPT-ILab/mipt-mips/issues/152), [T#3](https://github.com/MIPT-ILab/mips-traces/issues/3) |

Minor contributors: Ivan Startsev ([#525](https://github.com/MIPT-ILab/mipt-mips/issues/525)), Arsen Davtyan ([#660](https://github.com/MIPT-ILab/mipt-mips/issues/660), [#643](https://github.com/MIPT-ILab/mipt-mips/issues/643)), Danil Yarovoy ([T#4](https://github.com/MIPT-ILab/mips-traces/issues/4)), Maxim Davydov ([#1096](https://github.com/MIPT-ILab/mipt-mips/issues/1096)), Ivan Korostelev ([#32](https://github.com/MIPT-ILab/mipt-mips/issues/32))

RISC-V **B** instructions are delivered by Kirill Chemrov, Yaroslav Okatev, Alexandr Vinogradov, Airat Nazmiev, Roman Zlobin, Igor Bulatov, Daniel Kofanov, Nikolay Zernov, Nikita Gorbachev, Vladimir Prokhorov, Eric Konks, Maxim Davydov, Ilya Burtakov, Alexey Shcherbakov, Anton Okley, Egor Titov, Eugene Naydanov, Mihail Fedorov, Vasilii Zaitsev, Ravil Zakiryanov, Ivan Burtakov, and Vladimir Graudt.

List of [Upcoming contributions](https://github.com/MIPT-ILab/mipt-mips/issues/assigned/*)

## About us

The project is mentored by [Kirill Korolev](https://github.com/kkorolev), and [Oleg Ladin](https://github.com/olegladin) under [Pavel Kryukov](https://github.com/pavelkryukov)'s supervising. For more information, check [this wiki page](https://github.com/MIPT-ILab/mipt-mips/wiki/About-Us) and [MIPT ILab site](https://mipt.ru/drec/about/ilab/)

Additional repositories:
* [Interactive MIPS traces](https://github.com/MIPT-ILab/mips-traces)
* [Interactive RISC-V traces](https://github.com/MIPT-ILab/riscv-mars-examples)
* [Pipeline Visualizer](https://github.com/MIPT-ILab/PipelineVis)
* [Lectures storage](https://github.com/MIPT-ILab/ca-lectures)
* [MIPS-MIPS-CEN64](https://github.com/MIPT-ILab/cen64)
* _[Branches imported from GoogleCode SVN](https://github.com/MIPT-ILab/mipt-mips-old-branches)_
* _[MSVC-compatible LibELF](https://github.com/MIPT-ILab/libelf)_
