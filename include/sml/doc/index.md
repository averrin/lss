<a href="http://www.boost.org/LICENSE_1_0.txt" target="_blank">![Boost Licence](http://img.shields.io/badge/license-boost-blue.svg)</a>
<a href="https://github.com/boost-experimental/sml/releases" target="_blank">![Version](https://badge.fury.io/gh/boost-experimental%2Fsml.svg)</a>
<a href="https://travis-ci.org/boost-experimental/sml" target="_blank">![Build Status](https://img.shields.io/travis/boost-experimental/sml/master.svg?label=linux/osx)</a>
<a href="https://ci.appveyor.com/project/krzysztof-jusiak/sml" target="_blank">![Build Status](https://img.shields.io/appveyor/ci/krzysztof-jusiak/sml/master.svg?label=windows)</a>
<a href="https://codecov.io/gh/boost-experimental/sml" target="_blank">![Codecov](https://codecov.io/gh/boost-experimental/sml/branch/master/graph/badge.svg)</a>
<a href="http://github.com/boost-experimental/sml/issues" target="_blank">![Github Issues](https://img.shields.io/github/issues/boost-experimental/sml.svg)</a>

---------------------------------------

Introduction
============

| **[Boost].SML (State Machine Language/Lite/Library)** | |
| - | - |
| Your scalable C++14 one header only State Machine Library with no dependencies ([__Try it online!__](http://boost-experimental.github.io/sml/examples/index.html#hello-world)) | <a class="github-button" href="https://github.com/boost-experimental/sml" data-style="mega" data-count-href="/boost-experimental/sml/stargazers" data-count-api="/repos/boost-experimental/sml#stargazers_count" data-count-aria-label="# stargazers on GitHub" aria-label="Star boost-experimental/sml on GitHub">GitHub</a> |

<div class="important">
<h3 class="wy-text-neutral"><span class="fa fa-info-circle wy-text-info"></span>&nbsp; Latest Release: <b>v1.0.1</b> (May 6, 2017)</h3>
<a href="https://github.com/boost-experimental/sml/releases" class="btn btn-success" style="margin-bottom:8px;" role="button"><span class="fa fa-download"></span>&nbsp; <b>Download</b></a> &nbsp;&nbsp;&nbsp;&nbsp; <a href="CHANGELOG/index.html" class="btn btn-info" style="margin-bottom:8px;" role="button"><span class="fa fa-reorder"></span>&nbsp; <b>Changelog</b></a> &nbsp;&nbsp;&nbsp;&nbsp; <a href="tutorial/index.html" class="btn btn-warning" style="margin-bottom:8px;" role="button"><span class="fa fa-gear"></span>&nbsp; <b>Tutorial</b></a> &nbsp;&nbsp;&nbsp;&nbsp; <a href="examples/index.html" class="btn btn-danger" style="margin-bottom:8px;" role="button"><span class="fa fa-book"></span>&nbsp; <b>Examples</b></a>
</div>

###UML State Machine

* [State Machine](https://en.wikipedia.org/wiki/UML_state_machine)
* [UML2 Specification](http://www.omg.org/spec/UML/2.5)
* [Boost.MSM - eUML](http://www.boost.org/doc/libs/1_60_0/libs/msm/doc/HTML/ch03s04.html)

###Do I need a State Machine?
State Machine design pattern prevents you from creating and maintaining spaghetti code.

```cpp
void some_function() {
    ...
    if ((is_running && !is_jumping) || just_started) {
        ...
    } else if (is_boss_level && extra_feature_enabled && !ab_test) {
        ...
    } else {
        ...
    }
}
```

If above code looks somewhat similar to your code base or if you like
to avoid it `[Boost].SML` may suit you!

###Real Life examples?

![CPP(BTN)](Run_SDL2_Integration_Example|https://raw.githubusercontent.com/boost-experimental/sml/master/example/sdl2.cpp)
![CPP(BTN)](Run_Plant_UML_Example|https://raw.githubusercontent.com/boost-experimental/sml/master/example/plant_uml.cpp)
![CPP(BTN)](Run_Logging_Example|https://raw.githubusercontent.com/boost-experimental/sml/master/example/logging.cpp)
![CPP(BTN)](Run_Testing_Example|https://raw.githubusercontent.com/boost-experimental/sml/master/example/testing.cpp)

&nbsp;

###Why [Boost].SML?

* [Boost.MSM - eUML](http://www.boost.org/doc/libs/1_60_0/libs/msm/doc/HTML/ch03s04.html) is awesome, however it has a few huge limitations making it unusable on a large scale projects;
  [Boost].SML, therefore, is trying to address those issues.

###Problems with Boost.MSM - eUML

* Long compilation times (see [Performance](overview.md#performance))
* Huge resulting binaries (see [Performance](overview.md#performance))
* Based on too many macros
* Long error messages (see [Error Messages](overview.md#error-messages))
* Sometimes hard to follow as not all actions might be seen on transition table (ex. initial states, state entry/exit actions)
* A lot of boilerplate code with actions/guards (requires fsm, event, source state, target state)
* Data in states makes it harder to share/encapsulate (UML compliant though)
* Loosely coupled design is hard to achieve
* Functional programming emulation
* Huge complexity may overwhelm in the beginning
* A lot of Boost dependencies

###[Boost].SML design goals

* Keep the Boost.MSM - eUML 'goodies'
    * Performance (see [Performance](overview.md#performance))
    * Memory usage (see [Performance](overview.md#performance))
    * eUML DSL (src_state + event [ guard ] / action -> dst_state)
    * UML standard compliant (As much as possible)

>

* Eliminate Boost.MSM - eUML problems
    * Compilation times (see [Performance](overview.md#performance))
    * Binary size (see [Performance](overview.md#performance))
    * Reduce complexity by eliminating less used features
    * Short and informative error messages (see [Error Messages](overview.md#error-messages))
    * Less boilerplate / no macros (see [Hello World](examples.md#hello-world))
    * Improve visibility by having all actions on transition table (see [States](examples.md#states))
    * Allows loosely coupled design (see [Dependency Injection](examples.md#dependency-injection))
    * Functional programming support using lamda expressions (see [Action/Guards](examples.md#action-guards))
    * No dependencies / one header (2k LOC)

###What 'lite' implies?

* Guaranteed quick compilation times
* Maximized performance
* No dependencies
* Easy/Intuitive to use

###*Supported* UML features

* Transitions / Anonymous transitions / Internal transitions / Self transitions / No transition (see [Transitions](examples.md#transitions), [Events](examples.md#events))
* Actions / Guards (see [Action/Guards](examples.md#action-guards))
* State entry / exit actions (see [States](examples.md#states))
* Orthogonal regions (see [Orthogonal Regions](examples.md#orthogonal-regions))
* Sub / Composite state machines (see [Composite](examples.md#composite))
* History (see [History](examples.md#history))
* Defer/Process (see [Defer/Process](examples.md#deferprocess))

###*Additional* features

* Logging (see [Logging](examples.md#logging))
* Testing (see [Testing](examples.md#testing))
* Runtime Dispatcher (see [Runtime Dispatcher](examples.md#runtime-dispatcher))
* Dependency Injection integration (see [Dependency Injection](examples.md#dependency-injection))

###Related materials

* [CppNow-2016 - C++14 version of Boost.MSM-eUML](https://www.youtube.com/watch?v=j0J33BnIjnU)
* [CppNow-2017 - State Machine Language](https://www.youtube.com/watch?v=Lg3tIact5Fw)
* [Meeting C++ 2016 - Implementing a web game in C++14](https://www.youtube.com/watch?v=8gRHHIjx4oE)

###Acknowledgements
* Thanks to [Christophe Henry](https://github.com/henry-ch) for a great [Boost.MSM - eUML](http://www.boost.org/doc/libs/1_60_0/libs/msm/doc/HTML/ch03s04.html) library
* Thanks to [Deniz Bahadir](https://github.com/Bagira80) for all his constributions and support
* Thanks to [Takatoshi Kondo](https://github.com/redboltz) for testing, improving the library and for a great [Boost.MSM guide](http://redboltz.wikidot.com)!
* Thanks to [Ulenspiegel](https://github.com/Ulenspiegel) for insuring the quality of the library
* Thanks to [Jean Davy](https://github.com/splitsen) for the cmake support
* Thanks to [feltech](https://github.com/feltech) for improvements and bug fixes
* Thanks to [Vicente J. Botet Escriba](https://github.com/viboes) for useful suggestions how to improve the library
* Thanks to [AnthonyVH](https://github.com/AnthonyVH) for all contributions
* Thanks to [Oliver Daniell](https://github.com/oliverdaniell) for all contributions
* Thanks to [Julius Gelšvartas](https://github.com/JuliusGel) for bug fixes
* Thanks to [Christopher Motl](https://github.com/cmotl) for documentation fixes
