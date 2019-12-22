CFLAGS=-Wall -g -fsanitize=address -DNDEBUG

all: logfind

logfind: logfind.c
	clang $(CFLAGS) logfind.c -o logfind

clean:
	rm -f logfind
