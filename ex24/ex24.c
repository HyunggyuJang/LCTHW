#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "dbg.h"

#define MAX_DATA 100

typedef enum EyeColor {
    BLUE_EYES, GREEN_EYES, BROWN_EYES,
    BLACK_EYES, OTHER_EYES
} EyeColor;

const char *EYE_COLOR_NAMES[] = {
    "Blue", "Green", "Brown", "Black", "Other"
};

typedef struct Person {
    int age;
    char first_name[MAX_DATA];
    char last_name[MAX_DATA];
    EyeColor eyes;
    float income;
} Person;

int fgetInt(int *dst, int max_data, FILE *src)
{
    char *intInString = malloc(max_data + 1);

    check_mem(intInString);
    check(fgets(intInString, MAX_DATA - 1, src), "Failed to read integer.");
    check(isdigit(intInString[0]), "Non-integer value is entered.");
    *dst = atoi(intInString);

    free(intInString);
    return 0;
error:
    if (intInString)
        free(intInString);
    return -1;
}

int fgetFloat(float *dst, int max_data, FILE *src)
{
    char *floatInString = malloc(max_data + 1);

    check_mem(floatInString);
    check(fgets(floatInString, MAX_DATA - 1, src), "Failed to read float.");
    check(isdigit(floatInString[0]) || floatInString[0] == '.',
          "Non-floating number value is entered.");
    *dst = atof(floatInString);

    free(floatInString);
    return 0;
error:
    if (floatInString)
        free(floatInString);
    return -1;
}

void stripTrailingWhites(char *src)
{
    int len = strlen(src);
    int i = 0;

    for (i = len - 1; i >= 0 && isspace(src[i]); --i)
        src[i] = '\0';
}

int fGets(char *to, int max_data, FILE *src)
{
    int count = 0;
    char readChar = 0;
    int rc = 0;
    while(count < max_data - 1) {
        rc = fscanf(src, "%c", &readChar);
        check(rc > 0, "Failed to read character.");

        if (readChar == '\n') break;

        to[count++] = readChar;
    }

    to[count] = '\0';

    return count;

error:
    return -1;
}

int main(int argc, char *argv[])
{
    Person you = {.age = 0 };
    int i = 0;
    //char *in = NULL;
    int rc = -1;

    printf("What's your First Name? ");
    rc = fGets(you.first_name, MAX_DATA - 1, stdin);
    check(rc > 0, "Failed to read first name.");
    stripTrailingWhites(you.first_name);

    printf("What's your Last Name? ");
    rc = fGets(you.last_name, MAX_DATA - 1, stdin);
    check(rc > 0, "Failed to read last name.");
    stripTrailingWhites(you.last_name);

    printf("How old are you? ");
    rc = fgetInt(&you.age, MAX_DATA - 1, stdin);
    check(!rc, "You have to enter a number.");

    printf("What color are your eyes:\n");
    for (i = 0; i <= OTHER_EYES; i++) {
        printf("%d) %s\n", i + 1, EYE_COLOR_NAMES[i]);
    }
    printf("> ");

    int eyes = -1;
    rc = fgetInt(&eyes, MAX_DATA - 1, stdin);
    check(!rc, "You have to enter a number.");

    you.eyes = eyes - 1;
    check(you.eyes <= OTHER_EYES
            && you.eyes >= 0, "Do it right, that's not an option.");

    printf("How much do you make an hour? ");
    rc = fgetFloat(&you.income, MAX_DATA - 1, stdin);
    check(!rc, "Enter a floating point number.");

    printf("----- RESULTS -----\n");

    printf("First Name: %s\n", you.first_name);
    printf("Last Name: %s\n", you.last_name);
    printf("Age: %d\n", you.age);
    printf("Eyes: %s\n", EYE_COLOR_NAMES[you.eyes]);
    printf("Income: %f\n", you.income);

    return 0;
error:

    return -1;
}
