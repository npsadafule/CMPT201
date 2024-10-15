[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/xDNikGfe)
# Assignment 3: Basics of Compilation

One of the most essential tools for software development is a compiler. Without a compiler, we
cannot generate an executable from our source code. In this assignment, you will learn various
aspects of compilers and practice them.

Similar to the tutorials that you have done earlier, this assignment consists of reading and
practicing. For the reading part, you need to study it like reading a textbook. It is not a
description of the assignment but a task itself. Make sure you understand what is there before you
proceed.

## Task 0: Understanding the Compilation Steps

(This is a reading task. Make sure you read and understand this before you proceed.)

Conventionally, when we say we "compile" C code, we understand it to mean generating executables
from source files. However, strictly speaking, this is *not* correct because there are multiple
steps involved and compilation is one of the steps. The actual steps are, *preprocessing*,
*compiling*, and *linking*. Well-known compilers for C/C++, such as GCC and Clang, hide these steps
and generate executables directly from source files. However, internally they go through these
steps.

* Preprocessing: This is the first step of generating an executable and the *preprocessor* looks at
  the source code and transforms it for compilation. Generally, this involves two things. First, it
  removes all comments (`/* */`, `//`, etc.) as those are not going to be compiled. Second, it
  processes all *preprocessing directives*, which are the statements that start with `#` in C (e.g.,
  `#include`, `#define`, etc.). The preprocessor looks at these and transforms them appropriately
  according to their purposes. An example would be `#include <stdio.h>`, where the preprocessor
  replaces the statement with the content of a file named `<stdio.h>`. Another example would be
  `#define NUM 10`, where the preprocessor replaces all occurrences of `NUM` with `10` in the code.
* Compiling: Once preprocessing is done, the *compiler* transforms C code into machine code. This is
  done by reading each and every source file (a `.c` file) and transforming the code in it into
  machine-specific instructions. These machine-specific instructions are what a CPU can directly
  execute. The compiler stores these instructions in an *object* file that typically has the
  extension of `.o`. For each `.c` (source) file, the compiler generates a corresponding `.o`
  (object) file. Often times, there are multiple `.c` files in the source and the compiler generates
  multiple `.o` files. We note that there can be other intermediate steps involved such as the
  assembly step, which we do not discuss here. We also note that `.o` files contain more than just
  machine-specific instructions, which again we do not discuss here.
* Linking: This is the final step of generating an executable. In the most basic form, the *linker*
  takes all `.o` files and combines them to generate a single executable.

## Task 1: Using the Compiler

In this task, you will create some `.c` files and compile them. For grading, you need to use
`record` to record what you do.

One thing to note before we proceed is that we have installed a linter and a formatter for
C/C++---[Clang-Tidy](https://clang.llvm.org/extra/clang-tidy/index.html) and
[ClangFormat](https://clang.llvm.org/docs/ClangFormat.html). As you write your C code, you will
notice that our Neovim will provide error messages and suggestions, and also fix the code style. As
mentioned before, these are very common programming tools and we highly encourage you to use these
tools for all your programming tasks, regardless of the language and the editor you use. These tools
teach you how to write clean and reliable code with best practices.

Now, here is the list of things you need to do for this task.

* First, make sure you're recording (using `record`, which will record what you do to in a directory
  named `.record`). Make sure you start recording in this directory (the same directory that
  contains this README.md file). Make sure also that you see `[recording]` in your prompt.
* Create a file named `test.c` that prints out `Hello World!`.
* Enter `clang test.c` and see what it generates. [Clang](https://clang.llvm.org/) is a C/C++
  compiler that we use in this course (an alternative would be [GCC](https://gcc.gnu.org/), which we
  don't use). Execute the generated file and check if it prints out `Hello World!`. (If you are not
  sure about how to run an executable in the current directory, go back to the Linux Tutorial and
  look it up.) The file you see is the default name of the file that Clang or GCC generates.
* Enter `clang -o test test.c`. This generates `test` as the name of the executable, i.e., you can
  provide the name of the executable that you want to generate with the `-o` flag. Run the
  executable and check if it prints out `Hello World!`.
* Create two files, one named `main.c` and the other named `other.c`. In `other.c`, write a function
  that prints out `Other invoked`. In `main.c`, write a main function that calls the function in
  `other.c`. You should also create a header file, `other.h` that contains the function definition
  for `other.c` and use it in `main.c`.
* Enter `clang main.c other.c` which generates an executable with the default name. Run it and check
  if it prints out `Other invoked`.
* Enter a command that will generate an executable named `main`. Once generated, run it and check if
  it prints out `Other invoked`.
* Now, when you run `clang main.c other.c`, Clang conveniently combines all the compile steps
  described earlier and generates an executable. However, we can run each step separately.
* Enter `clang -E main.c`. This runs only the preprocessor on `main.c`. Check the output to see how
  it looks like. It should have expanded your source code significantly with the header files and
  macros all replaced and included. You can also try `clang -E other.c`.
* Enter `clang -c main.c`. This runs both the preprocessor and the compiler to generate an object
  file (`.o`) from `main.c`. List the files in the current directory to see what it generates.
* Enter a command that will generate an object file from `other.c`.
* Enter `clang main.o other.o`. This runs the linker that combines both object files to generate an
  executable with the default name. Run the executable and check the output.
* Enter a command that will run the linker that combines `main.o` and `other.o` to generate an
  executable named `main.1`. Run the executable and check the output.
* After you're done with the above, enter `exit` to stop recording. As you know already, you can
  later come back and enter `record` to start recording again.
* Once you're done, push all the files/directories you created, including `.record/` and `.nvim/`,
  for grading.

## Task 2: Understanding Shared and Static Libraries

If you enter `clang --help`, you will see that there are *pages* of compiler options available (try
it!). Each of these options typically corresponds to a feature, which means that there are *a lot*
of features in a compiler. You have tried some of the options in the previous task, e.g., `-E`,
`-c`, `-o`. In the next tasks, we will see some of the other most common options that you will
encounter when you use a compiler, along with the background necessary to understand what those
flags are for.

(This is a reading task. Make sure you understand this before you proceed.)

When you use `printf()` in your `main()`, where does the code for `printf()` come from? You should
know that the definition of `printf()` is in a header file named `stdio.h`, which is why you need to
have `#include <stdio.h>` when using `printf()`. But where is the code for `printf()` and how does
the compiler know where it is?

`printf()` is part of the *C standard library*, commonly known as *libc*, pronounced as "lib-see"
with "lib" pronounced like "liberty". There are many implementations of it, but the most popular
one and the default one for Linux is called [*glibc*](https://www.gnu.org/software/libc/), which is
the GNU C library. Just to make sure everyone's on the same page, a *library* typically means a
collection of definitions, data structures, and functions, developed for the purpose of making them
available for other programs to use. For example, the C standard library is available for every C
program to use, and there are many libraries available online such as GitHub that you can clone and
use.

Going back to glibc, many Linux distros have two files that contain the compiled code for glibc. One
is `libc.a` and the other is `libc.so`. You can find where these files are located by entering
`clang --print-file-name=libc.a` and `clang --print-file-name=libc.so`.

Why are there two files, you might ask? These are two types of libraries that exist, serving
different purposes. `libc.a` and (more generally) any other libraries with the `.a` extension are
called *static* libraries. `libc.so` and any other libraries with the `.so` extension are called
*shared* libraries. A static library is linked to your code directly during the linking step (which
we discussed in the previous task). This means that the executable file itself contains the
(compiled) library code, not just your code. This is also called *static linking*. A shared library
is *not* directly linked to your code, which means that the executable file itself *does not*
include the library code. Instead, just before executing your code, the operating system finds where
the library is, and links the library to your executable. This is also called *dynamic linking*. As
a side note, *static* in compiler jargon refers to something that occurs at compile time, while
*dynamic* refers to something that occurs at run time. This makes sense for static and dynamic
linking, as static linking performs linking at compile time and dynamic linking performs linking at
run time.

As you might have guessed, static linking generates bigger executables since the executables contain
library code as well. The benefit though is that you don't need to worry about whether or not the
platform on which you're going to run your executable has the libraries that the executable needs.
On the other hand, dynamic linking doesn't create bigger executables but you have to make sure that
you have the correct libraries on the platform on which you want to run your executable. In reality,
dynamic linking is the most common type of linking for popular libraries such as the C standard
library. Since every C program most likely uses the C standard library, it makes sense to have one
copy of the library on a platform and not include it in every single C executable. This is why these
libraries are called *shared* libraries since one copy is shared by many executables.

By convention, a library file name doesn't just use the library's name, but also uses `lib` as the
prefix, `.a` or `.so` as a suffix, and an optional version number as another suffix. For example, a
popular library named `pthread`, which provides threading as you will learn later, has two files
named `libpthread.so.0` and `libpthread.a`. Notice that the base library name `pthread` is just one
part of the library file names. The actual library file name includes `lib` as the prefix, and `.a`
or `.so` (and an optional version number) as the suffix. **Knowing that `libpthread.so.0` is for the
library named `pthread` is important for compilation as you will see later.** Keep this in mind for
the next task!

Now, think about the commands you used to compile a program in the previous assignment. Do you
recall telling Clang where the C standard library was? The answer should be no because you didn't.
You didn't use any commands or any compiler options to tell Clang where to find the C standard
library. But somehow Clang was able to compile the program and the program was able to use functions
like `printf()`. How is this possible? This is possible because the linker searches a few known
places to find the libraries necessary. This is controlled by a few things, such as system-wide
configuration files where you specify library directories. For example, `cat
/etc/ld.so.conf.d/*.conf` will show you some locations. Try it! You can also tell the compiler to
search for specific directories, which is useful when you want to use a third-party library
downloaded from the Internet. This is done by using some compiler options, which you will try in the
next task.

## Task 3: Compiling and Using Static and Shared Libraries

In this task, you will learn how to compile static and shared libraries and use them. Again for
grading, you need to use `record` to record what you do.

* First, make sure you're recording (`record`) if you're not already. Check that you see
  `[recording]` in your prompt.
* Create a directory named `hello`. We will refer to the current directory (where this `README.md`
  is in) as the *assignment root* directory, since it is the root of the directory tree for this
  assignment (that now contains a subdirectory named `hello`).
* Create three files, one named `main.c` in the assignment root directory, another named `hello.c`
  in `hello/` (the directory---notice the `/` at the end that indicates that we're referring to a
  directory), and the third named `hello.h` in `hello/` as well. In `hello.c`, write a function that
  prints out `Hello World!`. In `hello.h`, write the declaration for the function in `hello.c`. In
  `main.c`, write a main function that calls the function in `hello.c`. (We assume that you know
  this already, but you need to include `hello/hello.h` instead of just `hello.h` in your `main.c`,
  since `hello.h` is in `hello/`).
* You will compile `hello.c` as both a static library and a shared library. To generate a static
  library, you first need to generate an object file and then create an *archive* file, which is
  another name for a static library file. You should know how to generate an object file, so
  generate yourself `hello.o` from `hello.c` using Clang. Make sure `hello.o` is in the `hello`
  directory, not in the assignment root directory.
* To generate an archive, you need to use a tool called `ar`. The command is `ar -r <archive file
  name> <object files>`. For the archive file name, follow the naming convention above using `hello`
  as the base library name (if you're not sure, go back to above where we said "Keep this in
  mind!"). For object files, we have only a single object file, so you only need to provide
  `hello.o` as an object file. Try this and make sure it generates a `.a` file in the `hello`
  directory, following the library naming convention.
* One way to link a static library is to directly provide the library as if it were a regular object
  file. For this, first generate `main.o` in the assignment root directory. Then, enter `clang -o
  main main.o hello/<archive file name>`. This will generate `main` as the executable. Run `main`
  and make sure it prints out `Hello World!`.
* Another way to link a static library is to tell the compiler which library you want to use and
  where to find it. This is done by two options: `-l<base library name>` for which library you want
  to use and `-L<library directory path>` for where to find it. Thus, `clang -L./hello -o main
  main.c -lhello` will find the library file from the directory `./hello` and generate `main` by
  linking `main.o` with the static library. Try this yourself, and then run `main` to make sure it
  prints out `Hello World!`. (If this step doesn't work for you as it's suppose to, answer this
  question: if the base library name is `hello`, what would be the static library file name?)
* It is important to note that where you write `-l<library name>` makes difference. It *must* come
  after the files that use the library. For example, `clang -L./hello -o main -lhello main.c` will
  fail to compile. Try it and check the error message.
* To generate a shared library, you just need to provide two options, `-shared` and `-fpic`, to
  Clang. In other words, `clang -shared -fpic -o <shared library file name> <source files>` will
  generate a shared library. Do this yourself using the naming convention above (where `hello` is
  still the base library name) and create a shared library in `hello/`.
* You can compile with a shared library by the second method described above using `-l` and `-L`. Do
  this yourself and generate `main` with the shared library in the assignment root directory.
* When you run `main`, it's going to fail. Do this yourself and check the error message. The reason
  is that, as mentioned earlier, the OS links shared libraries at run time and needs to know where
  to find them. One way we discussed earlier was via system-wide configuration files (e.g.,
  `/etc/ld.so.conf.d/` files), but typically, non-root users cannot modify those. Luckily, there are
  other ways.
* `LD_LIBRARY_PATH` is an environment variable that the OS uses to find directories that contain
  shared libraries. You can set this variable to `./hello` by entering `LD_LIBRARY_PATH=./hello`.
  More conveniently, you can set the variable and run `main` together by entering
  `LD_LIBRARY_PATH=./hello ./main`. Try it yourself and see if you get any errors (you shouldn't).
* Another way is to use a compile option `-rpath <shared library directory path>`. If you add it,
  then the executable itself will contain the information regarding where to find the shared
  library. Thus, `clang -rpath <shared library directory path> -l<shared library name> -L<shared
  library directory path> -o <executable name> <source files>` will generate an executable that
  knows where to find the shared library it uses. Note that you need both `-L` and `-rpath` here.
  `-L` is used at compile time and `-rpath` is used at run time. Try this yourself and generate
  `main`. Run `main` to make sure it runs fine.
* For both `LD_LIBRARY_PATH` and `-rpath`, it is better to use a full, absolute path (e.g.,
  `/home/cmpt201/units/02-tools/a4/hello`), rather than a relative path (e.g., `./hello`). A
  relative path is always relative to the current directory, so if you run `main` from a different
  directory, the OS won't be able to find the shared library.
* Once these are all done, stop the recording.

## Submission

Make sure you use git to push all the files/directories you created, including `.nvim/` and
`.record/`, for grading.

As with previous assignments, make sure you run `a3-checker` and perform some minimal basic checks.
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
* It is entirely *your responsibility* to run `a3-checker` before submitting your work.

# Other Important Compiler Options

As you gain more experience with compilers, you will learn that there are many more options useful
for development. Here are a few more options that you definitely want to know about and use.

* `-I`: This option tells the compiler where to find header files. You want to remember this as it
  is one of the most frequently used options. For example, when you were doing Task 1 above, the
  header file was in `hello/` and your `main.c` included `hello/hello.h`. You can instead include
  `hello.h` (not `hello/hello.h`) and provide `-I./hello` to tell the compiler that there are header
  files in `./hello`.
* `-g`: This option produces debugging information. You don't want to use this for production code
  but you do want to use it for debugging. You also do want to use this for your development, even
  for this course.
* `-Wall`: This option turns on many warnings. It is a very good idea to use this.
* `-Wextra`: This option turns on some other warnings not covered by `-Wall`. It is also a very good
  idea to use this.
* `-Weverything`: This option turns on all warnings. This is useful to check how strictly your code
  follows the C standard.
* `-Werror`: This option turns warnings into errors. It is a very good idea to use this to make sure
  you address the warnings.
* Sanitizers: Clang provides a set of *sanitizers* to help you debug/avoid subtle bugs. It is a very
  good idea to use these sanitizers. Below are some of the popular ones.
    * `-fsanitize=address`: This enables
      [AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html), a fast memory error
      detector. You probably want to use this together with `-fno-omit-frame-pointer` to get more
      helpful error messages.
    * `-fsanitize=memory`: This enables
      [MemorySanitizer](https://clang.llvm.org/docs/MemorySanitizer.html), which detects
      uninitialized reads. You also want to use this together with `-fno-omit-frame-pointer`.
    * `-fsanitize=thread`: This enables
      [ThreadSanitizer](https://clang.llvm.org/docs/ThreadSanitizer.html), which detects *data
      races* that you will learn later. This may not be useful now, but when we get to threading, it
      will be very useful.
    * On our VM, you can use tab completion to get the list of available sanitizers. Type in `clang
      -fsanitize=` and press `<Tab>`.
* `-O1`, `-O2`, and `-O3`: These options controls the level of optimization (a higher number means
  more optimization). You don't want to use optimization when debugging, since it behaves
  differently.

# Next Steps

You need to accept the invite for the next assignment (A4).

* Get the URL for A4. Go to the URL on your browser.
* Accept the invite for Assignment 4 (A4).
* If you are not in `units/02-tools` directory, go to that directory.
* Clone the assignment repo.
