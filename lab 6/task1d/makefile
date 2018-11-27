#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: myshell

myshell: myshell.o job_control.o line_parser.o
	gcc -g -Wall -o myshell myshell.o job_control.o line_parser.o

# Depends on the source and header files
job_control.o: job_control.c job_control.h
	gcc -g -Wall -c -o job_control.o job_control.c 

line_parser.o: line_parser.c line_parser.h
	gcc -g -Wall -c -o line_parser.o line_parser.c 

myshell.o: myshell.c
	gcc -g -Wall -c -o myshell.o myshell.c


#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o myshell