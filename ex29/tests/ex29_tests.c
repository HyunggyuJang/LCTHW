#include <stdio.h>
#include "dbg.h"
#include <dlfcn.h>

typedef int (*lib_function)(const char *data, int data_length);

int test_main(int argc, char *argv[])
{
    int rc = 0;
    check(argc == 4, "USAGE: ex29 libex29.so funciton data");

    char *lib_file = argv[1];
    char *func_to_run = argv[2];
    char *data = argv[3];

    void *lib = dlopen(lib_file, RTLD_LAZY | RTLD_LOCAL);
    check(lib != NULL, "Failed to open the library %s: %s", lib_file,
          dlerror());

    lib_function func = dlsym(lib, func_to_run);
    check(func != NULL,
          "Did not find %s function in the library %s: %s", func_to_run,
          lib_file, dlerror());

    rc = func(data, strlen(data));
    check(rc == 0, "Function %s return %d for data: %s with length of %d",
          func_to_run, rc, data, (int) strlen(data));

    rc = dlclose(lib);
    check(rc == 0, "Failed to close %s", lib_file);

    return 0;

error:
    return 1;
}

int main(int argc, char *argv[])
{
    char *fake_argv[] = { "ex29", "../build/libex29.so",
                          "print_a_message", "hello there"};
    char fake_argc = sizeof fake_argv / sizeof fake_argv[0];

    int rc = test_main(fake_argc, fake_argv);
    check(rc == 0, "Error occured during first test clause.");

    fake_argv[2] = "uppercase";
    rc = test_main(fake_argc, fake_argv);
    check(rc == 0, "Error occured during second test clause.");

    fake_argv[2] = "lowercase";
    fake_argv[3] = "HELLO THERE";
    rc = test_main(fake_argc, fake_argv);
    check(rc == 0, "Error occured during third test clause.");

    fake_argv[2] = "fail_on_purpose";
    fake_argv[3] = "I fail";
    rc = test_main(fake_argc, fake_argv);
    check(rc == 1, "Fourth test clause supposed to fail but not.");

    return 0;
error:
    return 1;
}
