#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dbg.h"
#define MAX_DATA 100

int fGets(char *to, int max_data, FILE *src)
{
    int count = 0;
    char readChar = 0;
    int rc = 0;
    while(count < max_data - 1) {
        rc = fscanf(src, "%c", &readChar);
        check_debug(rc > 0, "Hit end of file.");

        if (readChar == '\n') break;

        to[count++] = readChar;
    }

    to[count] = '\0';

    return count;

error:
    if (ferror(src)) {
        log_err("Error occured during reading.");
        return -1;
    }

    return 0;
}

int isAllCheckedOff(char **wordsToCheck, int numberOfWords)
{
    for (int i = 0; i < numberOfWords; ++i) {
        if (wordsToCheck[i])
            return 0;
    }
    return 1;
}

typedef int (*find_cb)(char **wordsToCheck, int numberOfWords, char *sourceLine);


int findWithAnd(char **wordsToCheck, int numberOfWords, char *sourceLine)
{
    int i = 0;
    for (i = 0; i < numberOfWords; i++) {
        if (!wordsToCheck[i]) continue;
        if (strnstr(sourceLine, wordsToCheck[i], MAX_DATA))
            wordsToCheck[i] = NULL; /* Found */
    }
    if (isAllCheckedOff(wordsToCheck, numberOfWords))
        return 1;
    return 0;
}

int findWithOr(char **wordsToCheck, int numberOfWords, char *sourceLine)
{
    int i = 0;
    for (i = 0; i < numberOfWords; i++)
        if (strnstr(sourceLine, wordsToCheck[i], MAX_DATA))
            return 1;
    return 0;
}

static find_cb findMethod = NULL;

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

    while (fGets(currentLine, MAX_DATA - 1, sourceFile) > 0 &&
           !(allFound = findMethod(wordsToCheck, numberOfWords, currentLine)))
        ;


    rc = fclose(sourceFile);
    check(rc == 0,
          "Error occured during closing file %s", fileName);
    free(wordsToCheck);
    return allFound;

error:
    return -1;
}

int parseConfigurationFile(char logFiles[][MAX_DATA])
{
    int nlogs = 0;
    FILE *fileList = fopen(".logfind", "r");
    check(fileList, "Unable to open configuration file");

    for (nlogs = 0; fGets(logFiles[nlogs], MAX_DATA, fileList) > 0; ++nlogs) {
        check(!ferror(fileList),
              "Error occured during parsing configuration file.");
        debug("Currently parsed name is %s", logFiles[nlogs]);
    }

    fclose(fileList);
    return nlogs;
error:
    if (fileList) fclose(fileList);
    return -1;
}

int main(int argc, char *argv[])
{
    char logFiles[MAX_DATA][MAX_DATA] = { { '\0' } };
    int nlogs = parseConfigurationFile(logFiles);
    check(nlogs >= 0, "Unable to process configuration file.");

    int i = 0;
    int rc = 0;
    int isOr = 0;

    char **wordsToFind = calloc(argc - 1, sizeof (char *));
    int nWords = 0;

    check(argc > 1, "You need to specify the words to find with.");

    for (i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            wordsToFind[nWords++] = argv[i];
            continue;
        }
        check(argv[i][1] == 'o', "Illegal option %c", argv[i][1]);
        isOr = 1;
    }

    findMethod = isOr ? findWithOr : findWithAnd;

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
