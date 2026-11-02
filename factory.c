#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* You can use these functions and data structures to transform strings to
 * numbers and use them in arrays
 */
enum place {
    NUZKY, VRTACKA, OHYBACKA, SVARECKA, LAKOVNA, SROUBOVAK, FREZA,
    _PLACE_COUNT
};

const char *place_str[_PLACE_COUNT] = {
    [NUZKY] = "nuzky",
    [VRTACKA] = "vrtacka",
    [OHYBACKA] = "ohybacka",
    [SVARECKA] = "svarecka",
    [LAKOVNA] = "lakovna",
    [SROUBOVAK] = "sroubovak",
    [FREZA] = "freza",
};

enum product {
    A, B, C,
    _PRODUCT_COUNT
};

const char *product_str[_PRODUCT_COUNT] = {
    [A] = "A",
    [B] = "B",
    [C] = "C",
};

int find_string_in_array(const char **array, int length, char *what)
{
    for (int i = 0; i < length; i++)
        if (strcmp(array[i], what) == 0)
            return i;
    return -1;
}

/* It is not necessary to represent each working place with a dynamically
 * allocated object.  You can store only numbers of ready places.
 *
 * int ready_places[_PLACE_COUNT];
 */

/* It is not necessary to represent each part as a dynamically allocated
 * object.  You can store only a number of parts for each working phase.
 *
 * #define _PHASE_COUNT 6
 * int parts[_PRODUCT_COUNT][_PHASE_COUNT]
 */

int main(int argc, char **argv)
{
    /* Initialize your internal structures, mutexes and condition variables.
     */
    char *line = NULL;
    size_t sz = 0;
    while (1) {
        char *cmd, *arg1, *arg2, *arg3, *saveptr;

        if (getline(&line, &sz, stdin) == -1)
            break; /* Error or EOF */

        cmd = strtok_r(line, " \r\n", &saveptr);
        arg1 = strtok_r(NULL, " \r\n", &saveptr);
        arg2 = strtok_r(NULL, " \r\n", &saveptr);
        arg3 = strtok_r(NULL, " \r\n", &saveptr);

        if (!cmd) {
            continue; /* Empty line */
        } else if (strcmp(cmd, "start") == 0 && arg1 && arg2 && !arg3) {
            /* - start new thread for new worker
             * - copy (e.g. strdup()) worker name from arg1, the
             *   arg1 will be overwritten by the next getline() call
             * - workers should have dynamic objects, you don't know
             *   total number of workers
             */
        } else if (strcmp(cmd, "make") == 0 && arg1 && !arg2) {
            int product = find_string_in_array(product_str, _PRODUCT_COUNT, arg1);

            if (product >= 0) {
                /* add the part to factory cycle */
                /* you need to wakeup a worker to start working if possible */
            } else {
                /* print error */
            }
        } else if (strcmp(cmd, "end") == 0 && arg1 && !arg2) {
            /* tell the worker to finish
             * the worker has to finish their work first
             * you should not wait here for the worker to finish
             *
             * if the worker is waiting for work
             * you need to wakeup the worker
             */
        } else if (strcmp(cmd, "add") == 0 && arg1 && !arg2) {
            /* add a new place
             *
             * if worker and part is ready, start working - wakeup the worker
             */
        } else if (strcmp(cmd, "remove") == 0 && arg1 && !arg2) {
            /* If no place is currently idle, queue removal for later.
             * You should not wait here until the place becomes idle.
             */
        } else {
            fprintf(stderr, "Invalid command: %s\n", line);
        }
    }
    free(line);
    /* Wait for every worker to finish their work. Nobody should be able to
     * continue.
     */
}

