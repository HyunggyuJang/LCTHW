#include <stdio.h>
#include <ctype.h>
#include "dbg.h"

int print_a_message(const char *msg, int len)
{
    int rc = fputs("A STRING: ", stdout);
    check(rc != EOF, "Error occured before processing.");

    for (int i = 0; i < len; i++) {
        rc = fputc(msg[i], stdout);
        check(rc != EOF, "Error occured during processing.");
    }

    rc = fputc('\n', stdout);
    check(rc != EOF, "Error occured after processing.");
    return 0;

error:
    return 1;
}

int uppercase(const char *msg, int len)
{
    int i = 0;

    for(i = 0; i < len; i++) {
        fputc(toupper(msg[i]), stdout);
    }

    fputc('\n', stdout);

    check(!ferror(stdout), "Error occured during processing.");
    return 0;

error:
    return 1;
}

int lowercase(const char *msg, int len)
{
    int i = 0;

    for(i = 0; i < len; i++) {
        fputc(tolower(msg[i]), stdout);
    }

    fputc('\n', stdout);

    check(!ferror(stdout), "Error occured during processing.");
    return 0;

error:
    return 1;
}

int fail_on_purpose(const char *msg)
{
    return 1;
}
