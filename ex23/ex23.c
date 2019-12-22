#include <stdio.h>
#include <string.h>
#include <time.h>
#include "dbg.h"

#define DATA_SIZE 1000000

double get_tick()
{
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}

int normal_copy(char *from, char *to, int count)
{
    int i = 0;

    for (i = 0; i < count; i++) {
        to[i] = from[i];
    }

    return i;
}

int duffs_device(char *from, char *to, int count)
{
    {
        int n = (count + 7) / 8;

        switch (count % 8) {
            case 0:
                do {
                    *to++ = *from++;
                    case 7:
                    *to++ = *from++;
                    case 6:
                    *to++ = *from++;
                    case 5:
                    *to++ = *from++;
                    case 4:
                    *to++ = *from++;
                    case 3:
                    *to++ = *from++;
                    case 2:
                    *to++ = *from++;
                    case 1:
                    *to++ = *from++;
                } while (--n > 0);
        }
    }

    return count;
}

int zeds_device(char *from, char *to, int count)
{
    {
        int n = (count + 7) / 8;
        debug("n starts: %d, count: %d, count%%8: %d", 
                n, count, count % 8);

        switch (count % 8) {
            case 0:
again:    *to++ = *from++;

            case 7:
          *to++ = *from++;
            case 6:
          *to++ = *from++;
            case 5:
          *to++ = *from++;
            case 4:
          *to++ = *from++;
            case 3:
          *to++ = *from++;
            case 2:
          *to++ = *from++;
            case 1:
          *to++ = *from++;
          debug("last case: n=%d", n);
          if (--n > 0) {
              debug("going again: n=%d", n);
              goto again;
          }
        }
    }

    return count;
}

int valid_copy(char *data, int count, char expects)
{
    int i = 0;
    for (i = 0; i < count; i++) {
        if (data[i] != expects) {
            log_err("[%d] %c != %c", i, data[i], expects);
            return 0;
        }
    }

    return 1;
}

int main(int argc, char *argv[])
{
    char from[DATA_SIZE] = { 'a' };
    char to[DATA_SIZE] = { 'c' };
    int rc = 0;
    double tick = 0.0;

    // setup the from to have some stuff
    // measure the execution time of memset
    tick = - get_tick();
    memset(from, 'x', DATA_SIZE);
    tick += get_tick();
    log_info("memset took %.6f time", tick);
    // set it to a failure mode
    memset(to, 'y', DATA_SIZE);
    check(valid_copy(to, DATA_SIZE, 'y'), "Not initialized right.");

    // use normal copy to
    // mesasure execution time
    tick = - get_tick();
    rc = normal_copy(from, to, DATA_SIZE);
    tick += get_tick();
    log_info("nomal copy took %.6f time", tick);
    check(rc == DATA_SIZE, "Normal copy failed: %d", rc);
    check(valid_copy(to, DATA_SIZE, 'x'), "Normal copy failed.");

    // measure execution time of memcpy
    tick = - get_tick();
    memcpy(to, from, DATA_SIZE);
    tick += get_tick();
    log_info("memcpy took %.6f time", tick);

    // reset
    memset(to, 'y', DATA_SIZE);

    // duffs version
    // measure execution time
    tick = - get_tick();
    rc = duffs_device(from, to, DATA_SIZE);
    tick += get_tick();
    log_info("Duff's device took %.6f time", tick);
    check(rc == DATA_SIZE, "Duff's device failed: %d", rc);
    check(valid_copy(to, DATA_SIZE, 'x'), "Duff's device failed copy.");

    // measure execution time of memmove
    tick = - get_tick();
    memmove(to, from, DATA_SIZE);
    tick += get_tick();
    log_info("memmove took %.6f time", tick);

    // reset
    memset(to, 'y', DATA_SIZE);

    // my version
    rc = zeds_device(from, to, DATA_SIZE);
    check(rc == DATA_SIZE, "Zed's device failed: %d", rc);
    check(valid_copy(to, DATA_SIZE, 'x'), "Zed's device failed copy.");

    // reset
    memset(to, 'y', DATA_SIZE);

    /* // Error version -- those newly created device do not handle count = 0 */
    /* // case properly. */
    /* rc = duffs_device(from, to, 0); */
    /* check(rc == 0, "Duff's device expected to write 0 but %d", rc); */
    /* check(valid_copy(to, DATA_SIZE, 'y'), "Duff's device failed to copy nothing."); */

    return 0;
error:
    return 1;
}
