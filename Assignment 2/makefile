#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: root

# Tool invocations
root: root.o
	gcc -g -Wall -o root root.o 

# Depends on the source and header files
root.o: root.s
	nasm -g -f elf64 -w+all -o root.o root.s


#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o root