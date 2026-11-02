#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

#define _PHASE_COUNT 6

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

const int place_duration_ms[_PLACE_COUNT] = {
	[NUZKY] = 100,
	[VRTACKA] = 200,
	[OHYBACKA] = 150,
	[SVARECKA] = 300,
	[LAKOVNA] = 400,
	[SROUBOVAK] = 250,
	[FREZA] = 500,
};

const enum place product_pipeline[_PRODUCT_COUNT][_PHASE_COUNT] = {
	[A] = {NUZKY, VRTACKA, OHYBACKA, SVARECKA, VRTACKA, LAKOVNA},
	[B] = {VRTACKA, NUZKY, FREZA, VRTACKA, LAKOVNA, SROUBOVAK},
	[C] = {FREZA, VRTACKA, SROUBOVAK, VRTACKA, FREZA, LAKOVNA},
};

typedef struct worker {
	char* name;
	enum place profession;
	pthread_t thread;
	bool should_end;
	struct worker *next;
} worker_t;

typedef struct {
	pthread_mutex_t mtx;
	pthread_cond_t cond;

	int ready_places[_PLACE_COUNT];	
	int busy_places[_PLACE_COUNT];
	int remove_pending[_PLACE_COUNT];

	int parts[_PRODUCT_COUNT][_PHASE_COUNT];
	int in_progress[_PRODUCT_COUNT][_PHASE_COUNT];

	int active_workers[_PLACE_COUNT];
	bool eof_reached;

	worker_t *workers_head;
} factory_t;

static factory_t factory;



int find_string_in_array(const char **array, int length, char *what)
{
    for (int i = 0; i < length; i++)
        if (strcmp(array[i], what) == 0)
            return i;
    return -1;
}



int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

    /* Initialize your internal structures, mutexes and condition variables.
     */
	memset(&factory, 0, sizeof(factory));
	pthread_mutex_init(&factory.mtx, NULL);
	pthread_cond_init(&factory.cond, NULL);    


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
	pthread_mutex_destroy(&factory.mtx);
	pthread_cond_destroy(&factory.cond);
	return 0;
}

