#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dbg.h"
#define MAX_DATA 100

int isAllCheckedOff(char **wordsToCheck, int numberOfWords)
{
    for (int i = 0; i < numberOfWords; ++i) {
        if (wordsToCheck[i])
            return 0;
    }
    return 1;
}

int findWordsFromFile(char **wordsToFind, int numberOfWords, const char *fileName)
{
    FILE *sourceFile = fopen(fileName, "r");
    check(sourceFile, "Unable to open %s", fileName);

    char currentLine[MAX_DATA] = { '\0' };
    char **wordsToCheck = (char **) calloc(numberOfWords, sizeof (char *));
    check_mem(wordsToCheck);
    int i = 0;
    int rc = 0;
    int allFound = 0;

    // Setting up to word array to be checked off
    for (i = 0; i < numberOfWords; i++)
        wordsToCheck[i] = wordsToFind[i];

    while (fgets(currentLine, MAX_DATA - 1, sourceFile)) {
        for (i = 0; i < numberOfWords; i++) {
            if (!wordsToCheck[i]) continue;
            if (strnstr(currentLine, wordsToCheck[i], MAX_DATA))
                wordsToCheck[i] = NULL; /* Found */
        }
        if (isAllCheckedOff(wordsToCheck, numberOfWords)) {
            allFound = 1;
            goto cleanup;
        }
    }

cleanup:
    rc = fclose(sourceFile);
    check(rc == 0,
          "Error occured during closing file %s", fileName);
    free(wordsToCheck);
    return allFound;

error:
    return -1;
}

int main(int argc, char *argv[])
{
    char *logFiles[] = { "1.log", "2.log", "3.log" };
    int nlogs = sizeof logFiles / sizeof logFiles[0];
    int i = 0;
    int rc = 0;

    check(argc > 1, "You need to specify the words to find with.");
    int nWords = argc - 1;
    char **wordsToFind = argv + 1;

    for (i = 0; i < nlogs; ++i) {
        rc = findWordsFromFile(wordsToFind, nWords, logFiles[i]);
        check(rc != -1, "Error occured during searching %s", logFiles[i]);
        if (rc)
            printf("%s\n", logFiles[i]);
    }
    return 0;

error:
    return -1;
}
