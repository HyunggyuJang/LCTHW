CFLAGS=-Wall -g -fsanitize=address

all: logfind

logfind: logfind.c
	clang $(CFLAGS) logfind.c -o logfind

clean:
	rm -f logfind
