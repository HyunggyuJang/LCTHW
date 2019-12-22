#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "dbg.h"

#define MAX_DATA 10

int read_string(char **out_string, int max_buffer)
{
    *out_string = calloc(1, max_buffer + 1);
    check_mem(*out_string);

    char *result = fgets(*out_string, max_buffer, stdin);
    check(result != NULL, "Input error.");

    return 0;

error:
    if (*out_string) free(*out_string);
    *out_string = NULL;
    return -1;
}

int read_int(int *out_int)
{
    char *input = NULL;
    int rc = read_string(&input, MAX_DATA);
    check(rc == 0, "Failed to read number.");

    *out_int = atoi(input);

    free(input);
    return 0;

error:
    if (input) free(input);
    return -1;
}

int read_scan(const char *fmt, ...)
{
    int i = 0;
    int rc = 0;
    int *out_int = NULL;
    char *out_char = NULL;
    char **out_string = NULL;
    int max_buffer = 0;

    va_list argp;
    va_start(argp, fmt);

    for (i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%') {
            i++;
            switch (fmt[i]) {
                case '\0':
                    sentinel("Invalid format, you ended with %%.");
                    break;

                case 'd':
                    out_int = va_arg(argp, int *);
                    rc = read_int(out_int);
                    check(rc == 0, "Failed to read int.");
                    break;

                case 'c':
                    out_char = va_arg(argp, char *);
                    *out_char = fgetc(stdin);
                    break;

                case 's':
                    max_buffer = va_arg(argp, int);
                    out_string = va_arg(argp, char **);
                    rc = read_string(out_string, max_buffer);
                    check(rc == 0, "Failed to read string.");
                    break;

                default:
                    sentinel("Invalid format.");
            }
        } else {
            fgetc(stdin);
        }

        check(!feof(stdin) && !ferror(stdin), "Input error.");
    }

    va_end(argp);
    return 0;

error:
    va_end(argp);
    return -1;
}

int write_string(char *in_string)
{
    int i = 0;
    for (i = 0; in_string[i] != '\0'; ++i) {
        fputc(in_string[i], stdout);
        check(!ferror(stdout), "Output error.");
    }
    return i;

error:
    return -1;
}

int _write_int(char *intInString, int cur, int max_buffer, int src)
{
    check_debug(src, "Source integer hit 0");

    cur = _write_int(intInString, cur, max_buffer, src / 10);

    check_debug(cur <= max_buffer, "Unable to convert %d", src);

    intInString[cur++] = src % 10 + '0';

    return cur;

error:
    return cur;
}

int write_int(int in_int)
{
    char intInString[100] = { '\0' };
    int i = 0;

    if (in_int < 0)
    {
        intInString[i++] = '-';
        in_int = - in_int;
    }

    i = _write_int(intInString, i, 99, in_int / 10);
    check(i < 99, "Given integer to big to print.");

    intInString[i++] = in_int % 10 + '0';
    intInString[i] = '\0';

    return write_string(intInString);

error:
    return -1;
}

int write_print(const char *fmt, ...)
{
    int i = 0;
    int rc = 0;
    int in_int = 0;
    char in_char = ' ';
    char *in_string = NULL;

    va_list argp;
    va_start(argp, fmt);

    for (i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%') {
            i++;
            switch (fmt[i]) {
                case '\0':
                    sentinel("Invalid format, you ended with %%.");
                    break;

                case 'd':
                    in_int = va_arg(argp, int);
                    rc = write_int(in_int);
                    check(rc >= 0, "Failed to write int.");
                    break;

                case 'c':
                    in_char = va_arg(argp, int);
                    fputc(in_char, stdout);
                    break;

                case 's':
                    in_string = va_arg(argp, char *);
                    rc = write_string(in_string);
                    check(rc >= 0, "Failed to write string.");
                    break;

                default:
                    sentinel("Invalid format.");

            }
        } else {
            fputc(fmt[i], stdout);
        }

        check(!ferror(stdout), "Output error.");
    }

    va_end(argp);
    return 0;

error:
    va_end(argp);
    return -1;
}

int main(int argc, char *argv[])
{
    char *first_name = NULL;
    char initial = ' ';
    char *last_name = NULL;
    int age = 0;

    write_print("What's your first name? ");
    int rc = read_scan("%s", MAX_DATA, &first_name);
    check(rc == 0, "Failed first name.");

    write_print("What's your initial? ");
    rc = read_scan("%c\n", &initial);
    check(rc == 0, "Failed initial.");

    write_print("What's your last name? ");
    rc = read_scan("%s", MAX_DATA, &last_name);
    check(rc == 0, "Failed last name.");

    write_print("How old are you? ");
    rc = read_scan("%d", &age);

    write_print("---- RESULTS ----\n");
    write_print("First Name: %s", first_name);
    write_print("Initial: %c\n", initial);
    write_print("Last Name: %s", last_name);
    write_print("Age: %d\n", age);

    free(first_name);
    free(last_name);
    return 0;
error:
    return -1;
}
