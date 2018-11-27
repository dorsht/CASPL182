#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: hexeditplus

hexeditplus: task1.o
	gcc -g -Wall -o hexeditplus task1.o 

# Depends on the source and header files

task1.o: task1.c
	gcc -g -Wall -c -o task1.o task1.c


#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o hexeditplus