files := file1 file2
some_file: $(files)
	echo "Look at this variable: " $(files)
	touch some_file

file1:
	touch file1

file2:
	touch file2

clean:
	rm -f file1 file2 some_file

a := one two# a is set to the string "one two"
b := 'one two' # Not recommended. b is set to the string "'one two'"
all1:
	printf '$a'
	printf $b

x := dude

all2:
	echo $(x)
	echo ${x}
	echo $x
