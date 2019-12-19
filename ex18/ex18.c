#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define HACK_FP 1

/* Our old friend die from ex17. */
void die(const char *message)
{
    if (errno) {
        perror(message);
    } else {
        printf("ERROR: %s\n", message);
    }

    exit(1);
}

// a typedef creates a fake type, in this
// case for a function pointer
typedef int (*compare_cb)(int a, int b);
typedef int *(*sort_cb)(int *numbers, int count, compare_cb cmp);

/*
  A classic bubble sort function that uses the
  compare_cb to do the sorting.
*/
int *bubble_sort(int *numbers, int count, compare_cb cmp)
{
    int temp = 0;
    int i = 0;
    int j = 0;
    int *target = malloc(count * sizeof(int));

    if (!target)
        die("Memory error.");

    memcpy(target, numbers, count * sizeof(int));

    for (i = 0; i < count; i++) {
        for (j = 0; j < count -1; j++) {
            if (cmp(target[j], target[j+1]) > 0) {
                temp = target[j + 1];
                target[j + 1] = target[j];
                target[j] = temp;
            }
        }
    }
    return target;
}

void quick_sort_recur(int *numbers, int count, compare_cb cmp)
{
    int temp = 0;
    int i = 0;

    if (count < 2)
        return;

    int reference = count / 2;

    temp = numbers[reference];
    numbers[reference] = numbers[0];
    numbers[0] = temp;

    int last = 0;
    for (i = 1; i < count; ++i) {
        if (cmp(numbers[0], numbers[i]) > 0) {
            last++;
            temp = numbers[last];
            numbers[last] = numbers[i];
            numbers[i] = temp;
        }
    }

    temp = numbers[0];
    numbers[0] = numbers[last];
    numbers[last] = temp;
    quick_sort_recur(numbers, last, cmp);
    quick_sort_recur(numbers + last + 1, count - last - 1, cmp);
}

int *quick_sort(int *numbers, int count, compare_cb cmp)
{
    int *target = malloc(count * sizeof(int));

    if (!target)
        die("Memory error.");

    memcpy(target, numbers, count * sizeof(int));
    quick_sort_recur(target, count, cmp);
    return target;
}

int sorted_order(int a, int b)
{
    return a - b;
}

int reverse_order(int a, int b)
{
    return b - a;
}

int strange_order(int a, int b)
{
    if (a == 0 || b == 0) {
        return 0;
    } else {
        return a % b;
    }
}

/*
  Used to test that we are sorting things correctly
  by doing the sort and printing it out.
*/
void test_sorting(int *numbers, int count, sort_cb srt, compare_cb cmp)
{
    int i = 0;
    int *sorted = srt(numbers, count, cmp);

    if (!sorted)
        die("Failed to sort as requested.");

    for (i = 0; i < count; i++) {
        printf("%d ", sorted[i]);
    }
    printf("\n");

    free(sorted);

#if HACK_FP
    unsigned char *data = (unsigned char *) cmp;
    for (i = 0; i < 25; i++) {
        data[i] = i;
    }
    printf("\n");
#endif
}

int main(int argc, char *argv[])
{
    if (argc < 2) die("Usage: ex18 4 3 1 5 6");

    int count = argc - 1;
    int i = 0;
    char **inputs = argv + 1;

    int *numbers = malloc(count * sizeof(int));
    if (!numbers) die("Memory error.");

    for (i = 0; i < count; i++) {
        numbers[i] = atoi(inputs[i]);
    }

    test_sorting(numbers, count, bubble_sort ,sorted_order);
    test_sorting(numbers, count, quick_sort, sorted_order);
    test_sorting(numbers, count, bubble_sort ,reverse_order);
    test_sorting(numbers, count, quick_sort ,reverse_order);
    test_sorting(numbers, count, bubble_sort ,strange_order);
    test_sorting(numbers, count, quick_sort ,strange_order);

    free(numbers);

    return 0;
}
