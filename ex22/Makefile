CFLAGS=-Wall -g

all: ex22_main

ex22_main: ex22.o
	cc $(CFLAGS) ex22_main.c ex22.o -o ex22_main

ex22.o: ex22.c
	cc $(CFLAGS) -DNDEBUG -c -o ex22.o ex22.c

clean:
	rm ex22_main
	rm ex22.o
