all: mysh

mysh: shell.o
	 gcc -o mysh shell.o

shell.o: shell.c
	 gcc -c shell.c

clean:
	 rm shell.o mysh
