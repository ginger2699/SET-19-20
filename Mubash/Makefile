PROGNAME = bash
LIBS = -std=c11

CFLAGS = -Wall -pedantic -Werror -ggdb -Og

all: bash

bash: main.o lib_bash.o
			gcc main.o lib_bash.o -o bash

main.o: main.c
				gcc -c $(LIBS) $(CFLAGS) main.c

lib_bash.o: lib_bash.c
						gcc -c $(LIBS) $(CFLAGS) lib_bash.c

clean:
			rm -rf *o bash
