#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: calc

# Tool invocations
calc: asm_calc.o calc.o
	gcc -g -Wall -o calc calc.o asm_calc.o 

# Depends on the source and header files
asm_calc.o: asm_calc.s
	nasm -g -f elf64 -w+all -o asm_calc.o asm_calc.s

calc.o: calc.c
	gcc -g -Wall -c -o calc.o calc.c 
 

#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o calc