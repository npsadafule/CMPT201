memory_mapping: memory_mapping.c
	clang -o memory_mapping memory_mapping.c

memory_leak: memory_leak.c
	clang -fsanitize=address -o memory_leak memory_leak.c

use_after_free: use_after_free.c
	clang -fsanitize=address -o use_after_free use_after_free.c

types: types.c
	clang -o types types.c

alignment: alignment.c
	clang -o alignment alignment.c
