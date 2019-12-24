CFLAGS=-Wall -g -fsanitize=address

all: logfind
	./logfind || true
	./logfind -n Invalid || true
	./logfind TEST
	./logfind check char
	./logfind -o check char

logfind: logfind.c
	clang $(CFLAGS) logfind.c -o logfind

clean:
	rm -f logfind
