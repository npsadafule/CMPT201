[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/WGQnE8HR)
# Assignment 5: Debugging Tools

Debugging is fundamental in software development. You probably know this already from your
experience in previous courses because you might have run into errors in your code and had to find
what the problems were and fixed them. In fact, debugging is a very deep and broad topic. There are
countless books published on this (and a good book to check out is [Why Programs
Fail](https://www.whyprogramsfail.com/) by [Andreas Zeller](https://andreas-zeller.info/)). It is
also an active research area where numerous researchers try to improve the status quo. Though the
science of debugging is not within the scope of this course, we still want to discuss it, especially
regarding the tools you can use for debugging.

Note that this assignment mixes reading and activities. Please make sure you read thoroughly since
you will learn important concepts.

## Task 0: Logging

The most basic form of debugging is *tracing* where you track your program's code *flow*, i.e., the
line-by-line execution of your code. There are many tools you can use for code flow tracing, and
logging is the most basic tool where you insert print statements in various places in your code.
Practically all software uses some form of logging. For example, if you look at `/var/log`, you will
see the many log files that Linux generates.

You might have used logging in your own debugging, e.g., using `printf()`. However, most developers
use a *logging library* since it provides many useful features. For example, it allows you to turn
on or off logging messages based on message severity, e.g., fatal, warning, info, etc. It also
allows you to include useful information without requiring you to manually do it, such as source
file name, line number, etc. `printf()` is perhaps sufficient for toy programs, but once you pass
that stage, it is much better to use a logging library.

In this task, you will use a simple C logging library [log.c](https://github.com/rxi/log.c) to debug
a program. Though this library does not have many features, it suffices our purpose of understanding
what a logging library is.

* First, start recording with `record`. Make sure you are in the same directory that this
  `README.md` file is in.
* Go to the repo for [log.c](https://github.com/rxi/log.c) and carefully read its `README.md` to
  understand how to use the logging functions.
* Create a directory named `logging` under this assignment's root directory, and do everything in
  that directory.
* Create a file named `logging.c` and write the following code.

  ```bash
  #include <stdio.h>

  int main() {
    int size = 5;
    int numbers[size];
    int sum = 0;

    for (int i = 0; i <= size; ++i) {
      numbers[i] = i;
      sum += numbers[i];
    }

    printf("The sum is: %d\n", sum);

    return 0;
  }
  ```

* Write a Makefile that compiles `logging.c` and generates an executable named `logging`.
* Go to [log.c](https://github.com/rxi/log.c) and clone the repo outside of `units/`. For example,
  you can clone the repo in your home root directory (`$HOME`).
* From the cloned repo, copy `log.c` and `log.h` to `logging/`.
* Insert some logging functions to `logging.c` to trace the program execution. Make sure you modify
  your Makefile to compile correctly.
* Debug the problem and fix it. You need to submit the version that does not have the problem and
  also prints out logging messages using `log.c`. Do *not* change the `printf()` format since it
  will be used for grading.
* Entering `make` should produce a working executable named `logging`. This will be used for
  grading.
* You can stop recording here or continue with the next task.

## Task 1: Assertions

An assertion is a function that checks if a condition is true and, if not, throws an error. You can
use assertions for anything, but the most basic use cases are checking conditions on input arguments
and return values. In C, `assert()` defined in `<assert.h>` provides the functionality. In this
task, you will first learn the background and do an activity.

### Checking Input Argument Conditions

When you write a function, you make certain assumptions about your input arguments. For example,
let's say you're writing a function that performs integer division as follows.

```c
#include <stdint.h>

int32_t unsafe_divide(int32_t dividend, int32_t divisor) {
  return dividend / divisor;
}
```

When writing this function, you're making an implicit assumption that `divisor` is not 0. This is
known as *division by zero*, and in C, this is *undefined behavior*, meaning it can do whatever. In
addition, you are making an assumption that you are *not* dividing the minimum possible value for an
integer by -1, i.e., it is *not* that `dividend` is `INT32_MIN` and that `divisor` is -1 at the same
time. The reason is because `INT32_MIN` is -2^31 (since it's a 32-bit signed integer) and if you
were to divide it by -1, you would have to get +2^31, which is 1 more than the maximum possible
value for a 32-bit integer (2^31 - 1). This is called *integer overflow* and it is also undefined
behavior.

In cases like this, you might want to check if these assumptions that you have actually hold. This
way, you don't run into undefined behavior, which will do something unexpected. You can use an
assertion to perform such a check.

```c
#include <assert.h>
#include <stdint.h>

int32_t divide(int32_t dividend, int32_t divisor) {
  assert(divisor != 0 && (!((dividend == INT32_MIN) && (divisor == -1))));
  return dividend / divisor;
}
```

If you call the above function with `INT32_MIN` as `dividend` and -1 as `divisor` or just 0 as
`divisor`, the program will abort. This is useful for testing since you can catch the use of your
function (from other parts of your program) that violates your assumptions. Generally, assertions do
not have to abort but the function we use (`assert()`) does abort and should be used when
termination is the right thing to do.

More broadly, a condition that must be met before executing a piece of code is called a
*precondition*. It is a general concept and can be applied to not just a function but any section of
code in a program. It is frequently used as a safety check before executing a piece of code, similar
to the way we used `assert()` in the example above.

To learn more about undefined behavior in C, we highly encourage you to read [A Guide to Undefined
Behavior in C and C++](https://blog.regehr.org/archives/213) by John Regehr. The above example of
division by zero comes from that article. Also, if you want to see the list of undefined behavior,
you can look at the end of [this article](https://blog.regehr.org/archives/1520) by the same author.
Undefined behavior is a very common source of bugs and vulnerabilities in a program. [What Every C
Programmer Should Know about Undefined
Behavior](http://blog.llvm.org/2011/05/what-every-c-programmer-should-know.html) by Chris Lattner,
the main creator of Clang, is also a good read on this topic.

### Checking Return Conditions

Another common use case for `assert()` is right before returning from a function to check if the
conditions for the return value are met. For example, let's say you are writing a function that
returns the absolute value of an integer as follows.

```c
#include <stdint.h>

int32_t absolute_value(int32_t num) {
  int32_t ret = 0;

  if (num < 0) {
    ret = -num;
  } else {
    ret = num;
  }

  return ret;
}
```

Since you want to return an absolute value, it is a good idea to add a check that verifies that the
return value is greater than or equal to 0.

```c
#include <assert.h>
#include <stdint.h>

int32_t absolute_value(int32_t num) {
  int32_t ret = 0;

  if (num < 0) {
    ret = -num;
  } else {
    ret = num;
  }

  assert(ret >= 0);
  return ret;
}
```

Along with the concept of preconditions discussed above, there is a concept known as
*postconditions*. A postcondition is a condition that must be met after executing a piece of code.
This is a general concept that goes beyond function returns, and the use of `assertion()` in the
above example is just one example.

### Assertion Activity

* If you're not already recording, start recording with `record`. Make sure that you start recording
  in the same directory where this `README.md` is located.
* Create a directory named `assertions` and do everything in that directory.
* Create a file named `abs.c` with the `absolute_value()` function above.
* Add a *precondition* that catches the value of `int num` that can cause undefined behavior. The
  program should abort if the value is passed as the argument. (Hint: read the discussion above on
  undefined behavior.)
* Create a file named `main.c` with a `main()` function that calls your `absolute_value()` function
  (that contains the precondition you added). When calling `absolute_value()`, pass the value that
  causes undefined behavior, so you can make your precondition fail.
* Write a Makefile that produces the executable named `assertions`.
* You can stop recording here or continue with the next task.

## Task 2: Static Checkers and Sanitizers

We mentioned linters and sanitizers before, which are useful tools for debugging. As you know
already, we have installed linters for bash, C, CMake, etc. for Neovim, which give you useful
suggestions and report errors as you write your code. In this sense, linters perform *static* checks
as opposed to *dynamic* checks, i.e., their checks occur when you write your code rather than when
you run your code. You might recall that we discussed the differences between the terms "static" and
"dynamic" when we discussed static and dynamic (shared) libraries. At that time, we said that
"static" referred to something done at compile time and "dynamic" referred to something done at run
time. However, the meaning of "static" is in fact broader---it refers to something done by reading
and analyzing code rather than by running it. For this reason, linters belong to the category of
tools called *static analysis* tools, since they do not run the code but read and analyze it.

In contrast, sanitizers are dynamic tools in the sense that they perform their checks at run time.
As mentioned before, Clang provides many sanitizers such as `AddressSanitizer`, `ThreadSanitizer`,
`MemorySanitizer`, `UndefinedBehaviorSanitizer`, etc. You can enable these sanitizers at compile
time by using Clang's compiler options (`-fsanitize=`). When you run the resulting executable, the
sanitizers that you have enabled will report errors if found. In this task, you will use one of the
sanitizers and see which error you get.

* Again, if you're not already recording, start recording with `record`. Make sure that you start
  recording in the same directory where this `README.md` is located.
* Create a directory named `sanitizers` and do everything in that directory.
* Create a file named `abs.c` with the `absolute_value()` function above.
* Remove all `assert()` calls from your `absolute_value()` function.
* Create a file named `main.c` with a `main()` function that calls your `absolute_value()` function.
  When calling `absolute_value()`, pass the value that causes undefined behavior.
* Write a Makefile that produces an executable named `sanitizers`.
* Run the executable and see what happens.
* Modify your Makefile so you can enable `UndefinedBehaviorSanitizer`. The option for `clang` you
  need to include is `-fsanitize=undefined`.
* Compile and run the executable. The executable should terminate with an error message about the
  undefined behavior. As you can see, a sanitizer automatically checks what you can manually check
  with an assertion, which is a benefit of using a sanitizer. A sanitizer inserts certain checks for
  you in order to detect or prevent the category of errors it targets, e.g., undefined behavior,
  memory errors, etc.
* You can stop recording here or continue with the next task.

## Task 3: Fuzzers

In the previous task, you passed a value that caused undefined behavior for `absolute_value()` so
that `UndefinedBehaviorSanitizer` could detect it and report it. This means that when you have a
problem in a function such as `absolute_value()`, you need to know which value to pass in order to
trigger the problem and fix it. But how do you know what values to pass? Generally, if you write a
function, you don't know if problems exist and so it is difficult for you to pass the right values
that trigger problems.

A *fuzzer* addresses that issue, not entirely but in a quite useful fashion. The most basic way to
understand a fuzzer is to think of it as an input generator. A fuzzer keeps running a function or a
program with a newly-generated input to see if the new input triggers any problem. It turns out that
this simple strategy of running a function/program over and over with new inputs is a very effective
way to find bugs and vulnerabilities. For example, [Google's OSS-Fuzz
project](https://github.com/google/oss-fuzz), which continuously runs various fuzzers on open-source
software, says that "[a]s of August 2023, OSS-Fuzz has helped identify and fix over 10,000
vulnerabilities and 36,000 bugs across 1,000 projects." Obviously, a fuzzer's main strength lies in
*how* to generate new inputs, and various fuzzers have their own strategies. A classic one is random
input generation, [an idea that goes back decades](https://dl.acm.org/doi/10.1145/96267.96279). But
modern fuzzers have many strategies they implement.

In this task, you will use a well-known fuzzer called
[libFuzzer](https://llvm.org/docs/LibFuzzer.html) and see how it finds a problem.

* Again, if you're not already recording, start recording with `record`. Make sure that you start
  recording in the same directory where this `README.md` is located.
* Create a directory named `fuzzers` and do everything in that directory.
* Create a file named `fuzzing.c` with the `absolute_value()` function above in it.
* Remove all `assert()` calls from your `absolute_value()` function.
* libFuzzer requires you to write a function called a *fuzz target* instead of the regular `main()`
  function. This is so you can receive a new input generated by libFuzzer and pass it to your own
  function. If you read the [Fuzz Target](https://llvm.org/docs/LibFuzzer.html#fuzz-target) section
  of the libFuzzer documentation page, it will tell you that you need to implement the following
  function:

  ```c
  extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    ...
  }
  ```

  This is the function that we need to implement instead of `main()`. If you look at the parameters,
  there are two, `const uint8_t *Data` and `size_t Size`. The first parameter is a pointer to a new
  input generated by libFuzzer. The second parameter is the size of the newly-generated input. One
  thing to note here is that since we are using plain C, not C++, we do not need `extern "C"`.
* Now, our `absolute_value()` expects to receive a 32-bit integer (`int32_t num`). Thus, we cannot
  just pass a libFuzzer-generated input to `absolute_value()`. We need to format it and make sure
  that we are passing correctly-sized data. We can do it with the following code:

  ```c
  #include <stddef.h>
  #include <stdint.h>

  int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    uint8_t input[sizeof(int32_t)] = {0}; // Making sure we initialize every byte to 0.

    // Making sure we're not passing anything larger than sizeof(int32_t) bytes
    size_t new_size = (size > sizeof(int32_t)) ? sizeof(int32_t) : size;
    for (size_t i = 0; i < new_size; ++i) {
      input[i] = data[i];
    }

    absolute_value(*(int32_t *)input);

    return 0;
  }
  ```

* Clang can compile this with libFuzzer with `-fsanitize=fuzzer` as an additional option passed to
  `clang`. It will produce an executable, which will repeatedly call `absolute_value()` with a new
  input. However, libFuzzer itself doesn't detect any problems and it just generates new inputs and
  calls `LLVMFuzzerTestOneInput()`. Thus, we also need a mechanism to detect problems. For example,
  let's say we enable `UndefinedBehaviorSanitizer` as a undefined behavior detection mechanism. If
  there is undefined behavior and libFuzzer generates a right input to trigger the problem,
  `UndefinedBehaviorSanitizer` will be able to detect the problem. This is an important point---a
  fuzzer's main feature is repeatedly generating new inputs and running code. We still need a way to
  detect a problem, which we can do either manually by inserting assertions ourselves or
  automatically by using tools like sanitizers. Though fuzzers *can* detect simple problems like
  crashes, generally you want to use it with other detection mechanisms like sanitizers.
* Let's use libFuzzer together with `UndefinedBehaviorSanitizer` by compiling `fuzzing.c` with
  `clang -fsanitize=undefined,fuzzer -o fuzzing fuzzing.c`
* If you run the executable, it will indefinitely run with new inputs. We can limit the duration of
  execution with `-max_total_time=<sec>`. For example, `./fuzzing -max_total_time=10` limits the
  duration of execution to 10 seconds.
* Run it and see what happens. The output format is explained
  [here](https://llvm.org/docs/LibFuzzer.html#output). In the middle of the output, you should be
  able to see an error message that shows `runtime error` that explains the detected undefined
  behavior. What this means is that we have discovered the same problem that we discovered in the
  previous task, but this time without providing a manually-crafted input. libFuzzer has generated
  an input that triggers the undefined behavior. At the end of the output, you should be able to see
  a message that says something similar to "Done 13316455 runs in 11 second(s)", which means that
  `absolute_value()` has been called X times (e.g., 13316455 times) with that many new inputs.
* If you create a directory and pass it as an argument, libFuzzer will save the inputs that have
  triggered problems. For example, if you create `corpus/` and run `./fuzzing -max_total_time=10
  corpus`, libFuzzer will save problematic inputs in `corpus/`. This directory can also contain
  [seed inputs](https://llvm.org/docs/LibFuzzer.html#corpus), which are sample inputs that libFuzzer
  can start with. It is typically considered a good idea to provide seed inputs, especially in cases
  where your function expects a particular data format, e.g., images.
* You can stop recording here or continue with the next task.

## Task 4: Debuggers

The last debugging tool we want to discuss is debuggers. A debugger gives you a powerful set of
features suited for all debugging tasks, e.g., step-wise execution, variable inspection, reverse
execution, code and memory manipulation, etc. In particular, [GDB](https://www.sourceware.org/gdb/)
is a popular command-line debugger available on Unix-like platforms. In this task, you will work
through some tutorials for GDB.

* Once again, if you are not recording, start recording with `record`. You should do this in the
  directory that this `README.md` file is in.
* Create a directory named `debugger` and do everything in that directory.
* Open [A GDB Tutorial with Examples](https://www.cprogramming.com/gdb.html) and work through the
  tutorial. However, when the tutorial asks you to run `gdb`, run `cgdb` instead.
  [cgdb](https://cgdb.github.io/) is a front-end to GDB that provides more features. Also, when the
  tutorial asks you to write a C++ factorial program, write the following C program instead.

  ```c
  #include <stdio.h>
  #include <stdlib.h>

  long factorial(int n);

  int main(void) {
    char input_buffer[10];
    int n = 0;

    if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
      printf("Error reading input.\n");
      return 1;
    }

    n = atoi(input_buffer);

    long val = factorial(n);
    printf("%ld\n", val);
    return 0;
  }

  long factorial(int n) {
    long result = 1;
    while (n--) {
      result *= n;
    }
    return result;
  }
  ```

  Since the code is different, the line numbers are different as well. When setting up breakpoints,
  make sure you match the line numbers. Lastly, instead of using `g++`, use `clang`.
* Next, read through two other articles to understand the basic features of GDB better. They are
  [The GDB developer's GNU Debugger tutorial, Part 1: Getting started with the
  debugger](https://developers.redhat.com/blog/2021/04/30/the-gdb-developers-gnu-debugger-tutorial-part-1-getting-started-with-the-debugger)
  and [An introduction to debug events: Learn how to use
  breakpoints](https://developers.redhat.com/articles/2022/11/08/introduction-debug-events-learn-how-use-breakpoints)
* Once everything is done, stop recording.

## Submission

Make sure you use git to push all the files/directories you created, including `.nvim/` and
`.record/`, for grading.

As with previous assignments, make sure you run `a5-checker` and perform some minimal basic checks.
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
* It is entirely *your responsibility* to run `a5-checker` before submitting your work.

# Next Steps

You need to accept the invite for the next assignment (A6). Note that you now need to clone the repo
under `units/03-memory`, **not** `units/02-tools`.

* Get the URL for A6. Go to the URL on your browser.
* Accept the invite for Assignment 6 (A6).
* If you are not in `units/03-memory` directory, go to that directory.
* Clone the assignment repo.
