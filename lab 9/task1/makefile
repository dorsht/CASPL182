all: clean ccompile

ccompile:
	echo compiling c
 
	gcc -g -Wall -c -o common.o common.c
	gcc -g -Wall -c -o line_parser.o line_parser.c

	gcc -g -Wall -c -o client.o client.c
	gcc -g -Wall -c -o server.o server.c

	gcc -g -Wall  -o client client.o common.o line_parser.o
	gcc -g -Wall  -o server server.o common.o line_parser.o

#Clean the build directory
clean:
	echo cleaning the directory
	rm -f bin/*
