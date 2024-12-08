print: $(wildcard *.c)
	ls -la $?

thing_wrong := *.o
thing_right := $(wildcard *.o)

all: one two three four

one: $(thing_wrong)

two: *.o

three: $(thing_right)

four: $(wildcard *.o)
