main_dump: main_dump.c
	clang -o main_dump main_dump.c

data_and_bss: data_and_bss.c
	clang -o data_and_bss data_and_bss.c

stack_basics: stack_basics.c
	clang -o stack_basics stack_basics.c

stack_frames: stack_frames.c
	clang -o stack_frames stack_frames.c

buffer_overflow: buffer_overflow.c
	clang -o buffer_overflow buffer_overflow.c

buffer_overflow_sanitizer: buffer_overflow.c
	clang -fsanitize=address -o buffer_overflow_sanitizer buffer_overflow.c

stack_overlow: stack_overflow.c
	clang -fsanitize=address -o stack_overflow stack_overflow.c
