CC = clang
CFLAGS = -Wall -g -fsanitize=undefined

sanitizers: abs.c main.c
	$(CC) $(CFLAGS) -o sanitizers main.c abs.c
