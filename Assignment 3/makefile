all: sic

# Tool invocations
sic: sic.o
	gcc -g -Wall -o sic sic.o 

# Depends on the source and header files
sic.o: sic.s
	nasm -g -f elf64 -w+all -o sic.o sic.s


#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o sic