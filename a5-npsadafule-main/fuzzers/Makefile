CC = clang
CFLAG = -Wall -g -fsanitize=undefined,fuzzer

fuzzing: fuzzing.c
	$(CC) $(CFLAGS) -o fuzzing fuzzing.c
