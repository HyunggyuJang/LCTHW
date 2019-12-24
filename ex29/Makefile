CC=clang						#to work around system defined cc, which linked to system clang
CFLAGS=-g -O2 -Wall -Isrc -Iinclude -DNDEBUG $(OPTFLAGS)
LIBS=-ldl $(OPTLIBS)
PREFIX?=/usr/local

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))


SO_TARGET=build/libex29.so

# The Target Build
all: $(SO_TARGET) tests

dev: CFLAGS=-g -Wall -Isrc -Iinclude -Wextra $(OPTFLAGS)
dev: all

# Add this if you got an error
$(SO_TARGET): build $(OBJECTS)
	$(CC) -shared -o $@ $(OBJECTS)

$(TESTS): $(SO_TARGET)

build:
	@mkdir -p build
	@mkdir -p bin

# The Unit Tests
.PHONY: tests
tests: CFLAGS += -fsanitize=address $(LIBS)
tests: $(TESTS)
	rm -f tests/tests.log
	sh ./tests/runtests.sh

# The Cleaner
clean:
	rm -rf build $(OBJECTS) $(TESTS)
	rm -f tests/tests.log
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*dSYM" -print`

# The Install
install: all
	install -d $(DESTDIR)/$(PREFIX)/lib/
	install $(SO_TARGET) $(DESTDIR)/$(PREFIX)/lib/

# The Checker
check:
	@echo Files with potentially dangerous functions.
	@egrep '[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)\
|stpn?cpy|a?sn?printf|byte_)' $(SOURCES) || true
