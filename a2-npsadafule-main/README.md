[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/8J1MHtNF)
# Assignment 2: Bash Scripting

A bash script is something that you will encounter quite frequently when you use Linux. You can
think of it as a task automation tool---it allows you to create a single command that executes
multiple commands. It's not just that you can execute a series of commands sequentially. You can do
much more complicated things, using variables, if-else conditionals, etc., similar to what you can
do with a general-purpose programming language. In this assignment, you will learn the basics of
bash scripting.

We will again use a tutorial from [Ryan's Tutorials](https://ryanstutorials.net). The URL of the
bash script tutorial is [this](https://ryanstutorials.net/bash-scripting-tutorial/). Your task is to
finish the tutorial, but there are extra instructions you need to follow.

## Extra Instructions

Similar to the command-line interface tutorial, there are activities described at the end of each
section and those are the basis of your grade. However, we have made slight modifications to these
activities for easier grading. As a result, you need to complete the activities that we describe
below, not the ones from the tutorial.

Of course, you need to complete the activities using `nvim` on our VM. As mentioned before, we have
configured our `nvim` so that it occasionally takes a snapshot of the file you are editing and saves
the snapshot to a directory named `.nvim`. For this and all future assignments, you need to push
this directory as part of your submission. We will check this directory, analyze the snapshots to
make sure that you are using `nvim`, and use the analysis results as part of our grading. So make
sure you always use `nvim` for this and all future assignments.

Also, make sure that you record your entire work session(s) using `record`. You need to first go to
this assignment's repo directory (where you have this `README.md` file) and then enter `record`.
Check that you see `[recording]` at your prompt. Don't forget to run `exit` when you want to
stop.

### Using Bash

It is important to note that the tutorial assumes that you are using bash. However, you're using zsh
on our VM, which is a different shell. Zsh behaves slightly differently from bash (albeit not much),
so when you test out commands before you include them in your bash script, it is important to do it
using bash, not zsh. You can simply enter `bash` and it will give you a bash shell. That way, you
can test out various commands using bash. After you're done, enter `exit` to return to the original
zsh shell. Note that we have configured zsh on our VM to enable many features, e.g., custom prompt,
autocompletion, git support, etc. When you use bash, you won't have access to those features.

### Neovim Linter for Bash

As you write bash scripts using our Neovim, you will notice that it shows various suggestions. This
is because we have installed a *linter* called [ShellCheck](https://www.shellcheck.net/). A linter
is a program that analyzes code and reports errors and bugs, and also shows suggestions. It is a
very common programming tool and we highly encourage you to use it for all programming tasks,
regardless of the language and the editor you use. Using a linter, you can learn a lot about writing
clean and reliable code and following best practices. In fact, we have installed Neovim linters for
C/C++ as well, as you will see in later assignments.

### Activities for `2. Variables`

* Create a script named `2.variables.1.sh` which accepts command line arguments and echoes out how
  many arguments there are and what the second argument is. The format of the output should be the
  number of arguments followed by a single space followed by the second argument. This *must* be
  your format. Otherwise, the grader will fail.
* Create a script named `2.variables.2.sh` which prints a random word. There is a file containing a
  list of words (usually /usr/share/dict/words or /usr/dict/words). Print a random word from that
  file. A few hints:
    * Look at the tutorial's activity description for a hint regarding piping that you learned from
      A0. If you look at piping examples again, you will get an idea.
    * Remember that you just saw `$RANDOM` in the tutorial. `$RANDOM` ranges from 0 to 32767 (`man
      bash` will tell you) and the list of words is a lot more than 32767 words. It is okay to print
      out a random word out of 32767 words.
    * Remember that you also just saw command substitution in the tutorial.
* Expand the previous activity so that if a number is supplied as the first command line argument
  then it selects from only words with that many characters. A few hints:
    * Look at the tutorial's activity description for a hint regarding `grep`. If you look at the
      grep page of the Linux tutorial, especially `Regular Expresssion Overview`, you will have a
      good set of tools to work with.
    * You need to come up with a regular expression that detects a word with the number of
      characters passed as the first command line argument.
    * Recall the meanings of `^` and `$`.
    * Think about what `[a-zA-Z]` matches.

### Activities for `3. Input`

* Create a script named `3.input.1.sh` which asks the user with three separate prompts, the first
  for a last name, the second for a first name, and the third for an address. The script should then
  combine these into a single message which is echoed to the screen. The message format should be
  `<first name> <last name>, <address>`. Note that there is a comma between the name and the address
  and make sure you have the exact same spacing.
* Create a script named `3.input.2.sh` which takes data from STDIN and prints the 3rd line only.

### Activities for `4. Arithmetic`

* Create a script named `4.arithmetic.1.sh` which takes two command line arguments and then
  multiplies them together using each of the methods detailed above.
* Remember when we looked at variables we discovered $RANDOM returns a random number. This number is
  between 0 and 32767 which is not always the most useful. Let's write a script named
  `4.arithmetic.2.sh` which uses this variable and some arithmetic (hint: play with modulus) to
  return a random number between 0 and 100.
* Now let's play with the previous script. Modify it so that you can specify as a command line
  argument the upper limit of the random number. Can you make it so that a lower limit can be
  specified also? E.g. if I ran `./random.sh 10 45` it would only return random numbers between 10
  and 45. Name this script `4.arithmetic.3.sh`.

### Activities for `5. If Statements`

* Create a script named `5.if.1.sh` which takes 2 numbers as command line arguments. It should print
  to the screen the larger of the two numbers.
* Create a script named `5.if.2.sh` which accepts a file as a command line argument and check if the
  file is executable or writable. You should print a message with true/false for each. For example,
  if the file is both executable and writable, the message should be `true, true`. If the file is
  executable but not writable, the message should be `true, false`. Make sure you have the exact
  same capitalization and spacing as the examples.

### Activities for `6. Loops`

* Create a script named `6.loops.1.sh` which prints the numbers 1 - 10 (each on a separate line) and
  whether they are even or odd. The format should be `number, even/odd` e.g., `1, odd`.
* Write a script named `6.loops.2.sh` which takes a single command line argument (a directory) and
  prints each entry in that directory, each on a separate line. If the entry is a file it should
  print its size (just the size, not the file name). If the entry is a directory it should print how
  many items there are in that directory (just how many items there are, not the directory name).
* Once everything is done, make sure you stop recording.

### Submission

Make sure you use git to push all the scripts you wrote and files/directories you created, including
`.nvim/` and `.record/`, for grading.

As with previous assignments, make sure you run `a2-checker` and perform some minimal basic checks.
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
* It is entirely *your responsibility* to run `a2-checker` before submitting your work.

# Next Steps

You need to accept the invite for the next assignment (A3).

* Get the URL for A3. Go to the URL on your browser.
* Accept the invite for Assignment 3 (A3).
* If you are not in `units/02-tools` directory, go to that directory.
* Clone the assignment repo.
