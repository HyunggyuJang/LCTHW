#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int print_string(char *source)
{
    char *temp = malloc(strlen(source) + 1);
    strcpy(temp, source); // Use bad functions on purpose

    for (int i = 0; temp[i]; i++)
        fputc(temp[i], stdout);

    free(temp);
    return 0;
}
