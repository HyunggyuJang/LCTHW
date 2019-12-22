#include <stdio.h>
#include <string.h>
#include "dbg.h"
#define MAX_DATA 100

int main(int argc, char *argv[])
{
    char *logFiles[] = { "1.log", "2.log", "3.log" };
    int nlogs = sizeof logFiles / sizeof logFiles[0];
    FILE *currentLog = NULL;
    char currentLine[MAX_DATA] = { '\0' };
    int i = 0;
    int rc = 0;

    check(argc == 2, "Currently logfind can not cope with multiple arguments.");
    char *wordToFind = argv[1];

    for (i = 0; i < nlogs; ++i) {
        currentLog = fopen(logFiles[i], "r");
        check(currentLog, "Unable to open %s", logFiles[i]);

        while (fgets(currentLine, MAX_DATA - 1, currentLog)) {
            if (strnstr(currentLine, wordToFind, MAX_DATA))
            {
                printf("%s\n", logFiles[i]);
                break;
            }
        }

        rc = fclose(currentLog);
        check(rc == 0,
              "Error occured during closing file %s", logFiles[i]);
    }
    return 0;

error:
    if (currentLog) fclose(currentLog);
    return -1;
}
