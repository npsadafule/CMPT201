[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/e2xlcWtk)
# Assignment 4: Build Systems

Yet another important tool for development is a *build system*. A build system takes care of
compiling the source code of a program to generate executables, libraries, or sometimes entire
system images. If you have a single source file, you probably do not need a build system to compile
it. However, modern software often has countless source files with complex dependencies, and a build
system is necessary to manage them altogether.

The most basic problem of compiling many source files with dependencies is that of
*recompiling*---often times, you change a small portion of a single file and you want to recompile
your code so you can test the change. Ideally, you should be able to only recompile the source files
that are impacted by your change, i.e., you shouldn't have to recompile all the other files that
your change doesn't really impact at all. Build systems take care of this basic problem. Of course,
they do much more than that, as you will find out.

In this assignment, you will learn the basics of the two of the most popular build systems for C,
Make and CMake. [Make](https://www.gnu.org/software/make/) is a popular build system that has a
[long history](https://en.wikipedia.org/wiki/Make_(software)) from the early Unix days. Despite the
fact that it's been around for decades, it still remains a popular choice as a build system.
[CMake](https://cmake.org/) stands for *C*ross-platform *Make* and is another popular build system.
As the name suggests, it aims to be cross-platform, meaning that it allows you to use a single CMake
configuration to compile across different platforms. Both Make and CMake have many features but you
will learn just the basics so you can use them in future assignments.

## Task 0: Make

In this task, you will read through [Makefile Tutorial](https://makefiletutorial.com/), which is a
very well-written document that is designed for beginners. As always, you need to record what you do
as you follow the tutorial. But first, here are a few important points to note:

* Make sure you record what you do with `record`.
* Create a directory named `make` and do everything in that directory.
* The tutorial uses `cc` as the compiler, which defaults to `gcc`. We do not use `gcc` in this
  course. Instead, we use `clang`. The behavior is different, so make sure you use `clang` instead
  of `cc` or `gcc`.
* You need to work through the tutorial up to the end of the section named `Fancy Rules`.
* As the tutorial explains, Makefiles require tabs. However, we have replaced a tab with two spaces
  in our Neovim setup. In order to enter an actual tab, you need to enter `<Ctrl>-v` first, and then
  a tab.

### Make Activity

[Makefile Tutorial](https://makefiletutorial.com/) demonstrates various ways to write Makefiles.
Your task is to **write the Makefiles as the tutorial describes and run them to understand how they
behave**. Since you can create only one Makefile per directory, you must create a separate directory
for each Makefile. The directory should be named after the (sub)-section name in the tutorial.

Below are the directories you need to create. In each of these directories, you must have a Makefile
that contains the code the tutorial demonstrates in that section. If the tutorial shows you multiple
boxes (colored in black) within a section, please include them all in a single Makefile. There are
exceptions to this as we explain in parentheses below.

* Getting Started:
    * `running-the-examples`
    * `the-essence-of-make` (to make sure you understand the directives from above, please include
      both the `hello` and `blah` targets.)
    * `more-quick-examples`
    * `make-clean`
    * `variables` (there are two `all` targets here, so rename them as `all1` and `all2`.)
* Targets:
    * `the-all-target`
    * `multiple-targets`
* Automatic Variables and Wildcards:
    * `wildcard` (the actual section name is `* Wildcard`.)
    * `automatic-variables`
* Fancy Rules:
    * `implicit-rules`
    * `static-pattern-rules` (Makefile here should do the efficient way.)
    * `static-pattern-rules-and-filter`
    * `pattern-rules` (you just need to do the first example.)
    * `double-colon-rules`

## Task 1: CMake

CMake is a "meta" build system in the sense that it generates build files for another build system
(e.g., Makefiles for make) and relies on that build system for actual compilation. What you do with
CMake as a developer is to write configuration files (called CMake scripts) so that CMake can
generate build files. In this task, you will get a sense of how to use CMake. We adapt a few online
tutorials for this task such as [How to Use CMake](https://earthly.dev/blog/using-cmake/),
[Introduction to CMake by Example](http://derekmolloy.ie/hello-world-introductions-to-cmake/), and
[CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html).

### Part 0: Setup

* Make sure you record what you do with `record`.
* Create a directory named `cmake` and do everything below in that directory.
* Go to the `cmake` directory and create a directory named `src`. Go in there and create two files
  `main.c` and `random_range.c`.
    * Write the following code in `main.c`.

      ```c
      #include <stdio.h>

      int random_range(int min, int max);

      int main()
      {
        printf("Random number between 0 and 1000: %d\n" , random_range(0, 1000));
        return 0;
      }
      ```

    * Write the following code in `random_range.c`.

      ```c
      #include <stdio.h>
      #include <stdlib.h>
      #include <time.h>

      int random_range(int min, int max)
      {
        if (min >= max) {
          fprintf(stderr, "Error: Invalid range.\n");
          exit(1);
        }

        srand(time(NULL));
        return (rand() % (max - min + 1)) + min;
      }
      ```

* Compile the code with Clang and see if it works correctly.

### Part 1: CMake Basics

* Now, you will use CMake to compile this code. In order to do this, you need a file named
  `CMakeLists.txt`. This is the main configuration file for CMake, and without this, you can't use
  CMake. This file needs to exist in the root directory of your source. So in our example, don't
  create it in `src/`. Instead, create it in the parent directory of `src/` (i.e., `cmake/`).
* At the minimum, you need to have three commands in CMake, `cmake_minimum_required()`, `project()`,
  and `add_executable()`.
    * `cmake_minimum_required()` specifies the lowest version of CMake that you are supporting.
    * `project()` defines a few pieces of information regarding your program.
    * `add_executable()` tells CMake the executable name you want to generate and which source files
      CMake needs to use.
* The syntax for these three commands is as follows:
    * [The syntax for
      `cmake_minimum_required()`](https://cmake.org/cmake/help/latest/command/cmake_minimum_required.html#command:cmake_minimum_required)
      is `cmake_minimum_required(VERSION <min>)` where `<min>` is the minimum version number. For
      example, `cmake_minimum_required(VERSION 3.22)`.
    * [The syntax for
      `project()`](https://cmake.org/cmake/help/latest/command/project.html#command:project) is more
      complex, but at least you need to have `project(<project name>)` where `<project name>` is the
      name of your project. You typically want to have more information than that, such as a version
      number, a description, and the language you use. For example, `project(CMakeExercise VERSION
      1.0 DESCRIPTION "This is a CMake exercise." LANGUAGES C)` contains a project name
      (`CMakeExercise`), a version number (`1.0`), a description (`"This is a CMake exercise."`),
      and a supported language (`C`).
    * [The syntax for
      `add_executable()`](https://cmake.org/cmake/help/latest/command/add_executable.html#command:add_executable)
      is simple in that you first need to provide the executable name that you want to generate,
      followed by a list of source files. For example, `add_executable(random src/main.c
      src/random_range.c)` will generate `random` from `src/main.c` and `src/random_range.c`.
* Edit `CMakeLists.txt` and include the above three commands appropriately for compiling
  `src/main.c` and `src/random_range.c` and generating `random`.
* You also need to make sure that you use the correct C and C++ compilers with CMake. As mentioned
  earlier, we use `clang` in this course, and the command below sets the C and C++ compilers for
  CMake to `clang` and `clang++`. What it does is setting the environment variables `CC` and `CXX`,
  which various programs commonly use to find the C and C++ compilers.

  ```bash
  $ export CC=$(which clang)
  $ export CXX=$(which clang++)
  ```

  In fact, this needs to be done every time you log in. In order to automate this, we have
  configured this already by adding the above two commands at the end of `~/.zshenv`. Open
  `~/.zshenv` and check if the above two commands are in there.
* Once you have your `CMakeLists.txt` and the compilers all set up, you can compile your source with
  CMake. The most standard way of doing this is to create a directory called `build` and let CMake
  generate build files (e.g., Makefiles) under that directory. The reason why you want to do this is
  to cleanly separate your source from build files. To accomplish this, you can enter the following
  commands from your source's root directory. (`$` indicates a shell prompt, so you shouldn't type
  it in when you try the commands.)

  ```bash
  $ mkdir build
  $ cd build
  $ cmake .. # This tells CMake to use `CMakeLists.txt` in `..`, i.e., the parent directory.
  $ make
  ```

  As you can see, you still need to use `make` to compile your source. As mentioned earlier, this is
  because CMake is a "meta" build system.
* You can use CMake's options to do everything in the source's root directory as follows.

  ```bash
  $ cmake -S . -B build     # This generates build files in `build/`.
  $ cmake --build build     # This compiles the source and generates an executable.
  ```

  In the above commands, `-S <dir>` tells CMake that `<dir>` is the root of the source. `-B <dir>`
  tells CMake that we want to use `<dir>` for build files. `--build <dir>` tells CMake that we want
  to compile the source using `<dir>` as the build directory.
* Check `build/` and see if CMake has generated `random`, the executable. Run it and make sure it
  runs correctly.

### Part 2: A CMake Project

* Now, remove `build/` and everything in it for the next steps.
* Remove `int random_range(int min, int max);` in `main.c` and create a separate header file named
  `random_range.h` that contains the line.
* Create a new directory `include` (under the source root directory) and put `random_range.h` in the
  directory.
* Edit `main.c` appropriately to include the header.
* The source code structure should be as follows from the current directory (`.`), i.e., the source
  root directory.

  ```bash
  .
  |-- CMakeLists.txt
  |-- build
  |-- include
  |   \-- random_range.h
  \-- src
      |-- random_range.c
      \-- main.c
  ```

  In fact, this is how developers typically organize a C/C++ code base---a `CMakeLists.txt` (or a
  `Makefile`) in the source root directory, a `build/` directory for build files artifacts, an
  `include/` directory for header files, and a `src/` directory for source files.
* There are two important commands to introduce here to deal with such a source structure:
    * One is `include_directories(<dirs>)` that tells CMake where to find header files. For example,
      `include_directories(include)` tells CMake that `include` is a directory that contains header
      files. Include this in your `CMakeLists.txt`.
    * The other command is `file(GLOB <variable name> <wildcard expression>)` that allows us to use
      a wildcard expression to get a list of files and store it in a variable. For example,
      `file(GLOB SOURCES "src/*.c")` will make a list of all `.c` files under `src/` and store the
      list to a variable named `SOURCES`. You can use this variable instead of manually listing out
      every `.c` file under `src/` in `add_executable()`. The only catch here is that when you
      reference a variable in `CMakeLists.txt`, you need to use the format of `${<variable name>}`,
      e.g., `add_executable(add ${SOURCES})`.
* Modify your `CMakeLists.txt` to include this new `add_executable()` and the above `file(GLOB ...)`
  command.
* Check `build/` and see if CMake has generated `random`. Run it and make sure it runs correctly.

### Part 3: Final Notes

Once everything is done, make sure you stop recording.

For all future assignments, we will ask you to write your own `Makefile` or `CMakeLists.txt`. So
make sure you understand how to write those files.

## Submission

Make sure you use git to push all the files/directories you created, including `.nvim/` and
`.record/`, for grading.

As with previous assignments, make sure you run `a4-checker` and perform some minimal basic checks.
A few things to note about the checker:
* It warns you if you do not have any recording files. Generally, if you do not record your work
  sessions, you will receive a 0.
* It warns you if it detects that you have not used `nvim`. Generally, if you do not use `nvim` for
  your work, you will receive a 0.
* It warns you if it detects any copy-and-paste actions. Generally, if you copy and paste anything,
  you will receive a 0.
* It performs some basic checks for your progress and tells you about it.
* It is important to understand that the checker is not a grader and only performs minimal basic
  checks for you. We only provide it to make sure that you do not make any silly mistakes that could
  lead to an unexpected grade, e.g., a 0 or a very low grade. However, if it says that everything is
  good, and you have faithfully done the tasks above, you can be assured that you will get the full
  credit.
* Note that if a check fails, it does not always tell you why. You need to debug it by yourself and
  find out.
* It is entirely *your responsibility* to run `a4-checker` before submitting your work.

# Next Steps

You need to accept the invite for the next assignment (A5).

* Get the URL for A5. Go to the URL on your browser.
* Accept the invite for Assignment 5 (A5).
* If you are not in `units/02-tools` directory, go to that directory.
* Clone the assignment repo.

