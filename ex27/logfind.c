#define NDEBUG
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glob.h>
#include "dbg.h"
#define MAX_DATA 100

char *fGets(char *to, int max_data, FILE *src)
{
    int count = 0;
    int rc = 0;
    while(count < max_data - 1) {
        rc = getc(src);
        check_debug(rc != EOF, "Hit end of file.");

        if (rc == '\n') break;

        to[count++] = rc;
    }

    to[count] = '\0';

    return to;

error:
    if (ferror(src)) {
        log_err("Error occured during reading.");
        return NULL;
    }

    return NULL;
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

    while (fGets(currentLine, MAX_DATA - 1, sourceFile) != NULL &&
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

int parseConfigurationFile(glob_t *pglob)
{
    char line[MAX_DATA] = { '\0' };
    FILE *fileList = fopen(".logfind", "r");
    int glob_flags = GLOB_TILDE;
    int rc = -1;

    check(fileList, "Unable to open configuration file");

    while (fGets(line, MAX_DATA - 1, fileList) != NULL) {
        rc = glob(line, glob_flags, NULL, pglob);
        check(rc == 0 || rc == GLOB_NOMATCH, "Failed to glob.");

        glob_flags |= GLOB_APPEND;
        debug("Currently parsed name is %s", line);
    }

    check(!ferror(fileList),
          "Error occured during parsing configuration file.");
    fclose(fileList);

    for (int i = 0; i < pglob->gl_pathc; ++i) {
        debug("Glob matched file name: %s", pglob->gl_pathv[i]);
    }

    return 0;
error:
    if (fileList) fclose(fileList);
    return -1;
}

int parseCommandline(char ***wordsToFind, int *isOr, int argc, char *argv[])
{
    *wordsToFind = calloc(argc - 1, sizeof (char *));
    check_mem(*wordsToFind);
    int i = 0;
    int nWords = 0;

    for (i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            (*wordsToFind)[nWords++] = argv[i];
            continue;
        }
        check(argv[i][1] == 'o', "Illegal option %c", argv[i][1]);
        *isOr = 1;
    }

    return nWords;

error:
    if (*wordsToFind) free(*wordsToFind);
    *wordsToFind = NULL;
    return -1;
}

int printFoundFiles(glob_t *logFiles, char *wordsToFind[], int nWords)
{
    int rc = -1;
    for (int i = 0; i < logFiles->gl_pathc; ++i) {
        rc = findWordsFromFile(wordsToFind, nWords, logFiles->gl_pathv[i]);
        check(rc != -1, "Error occured during searching %s", logFiles->gl_pathv[i]);
        if (rc)
            printf("%s\n", logFiles->gl_pathv[i]);
    }

    return 0;

error:
    return -1;
}

int main(int argc, char *argv[])
{

    int i = 0;
    int rc = -1;
    int isOr = 0;
    char **wordsToFind = NULL;
    glob_t logFiles;

    check(argc > 1, "You need to specify the words to find with.");

    int nWords = parseCommandline(&wordsToFind, &isOr, argc, argv);
    check(nWords != -1, "Error occured while parsing commandline.");

    rc = parseConfigurationFile(&logFiles);
    check(rc == 0, "Failed to parse configuration file.");


    findMethod = isOr ? findWithOr : findWithAnd;

    rc = printFoundFiles(&logFiles, wordsToFind, nWords);
    check(rc == 0, "Failed to processing globbed files.");

    free(wordsToFind);
    globfree(&logFiles);
    return 0;

error:
    if (wordsToFind) free(wordsToFind);
    globfree(&logFiles);
    return -1;
}
