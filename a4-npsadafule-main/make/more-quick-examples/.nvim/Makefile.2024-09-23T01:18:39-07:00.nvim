blah: blah.o
	clang blah.o -o blah

blah.o: blah.c
	clang -c blah.c -o blah.o

blah.c:
	echo "int main() { return 0; }" > blah.c

some_file: other_file
	echo "This will always run, and runs second"
	touch some_file

other_file:
	echo "This will always run, and runs first"
