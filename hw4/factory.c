#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

#define _PHASE_COUNT 6



// ENUMS ------------------------------------------------------------

enum place {
    NUZKY, VRTACKA, OHYBACKA, SVARECKA, LAKOVNA, SROUBOVAK, FREZA,
    _PLACE_COUNT
};

enum product {
    A, B, C,
    _PRODUCT_COUNT
};



// LOOKUP TABLES ----------------------------------------------------

const enum place product_pipeline[_PRODUCT_COUNT][_PHASE_COUNT] = {
	[A] = {NUZKY, VRTACKA, OHYBACKA, SVARECKA, VRTACKA, LAKOVNA},
	[B] = {VRTACKA, NUZKY, FREZA, VRTACKA, LAKOVNA, SROUBOVAK},
	[C] = {FREZA, VRTACKA, SROUBOVAK, VRTACKA, FREZA, LAKOVNA},
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



// TODO: REASSIGN STRUCTS RESPECTING THE MEMORY PADDING
// STRUCTS ----------------------------------------------------------

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


	// 2D counters
	int parts[_PRODUCT_COUNT][_PHASE_COUNT];
	int in_progress[_PRODUCT_COUNT][_PHASE_COUNT];	
	

	// 1D counters
	int ready_places[_PLACE_COUNT];	
	int busy_places[_PLACE_COUNT];
	int remove_pending[_PLACE_COUNT];

	int active_workers[_PLACE_COUNT];
	

	bool eof_reached;
	worker_t *workers_head;
} factory_t;

static factory_t factory;



// HELPER FUNCS -----------------------------------------------------

int find_string_in_array(const char **array, int length, char *what)
{
    for (int i = 0; i < length; ++i)
        if (strcmp(array[i], what) == 0)
            return i;
    return -1;
}

worker_t* find_worker(const char* name) {
	for (worker_t* w = factory.workers_head; w != NULL; w = w->next) {
		if (!w->should_end && strcmp(w->name, name) == 0) return w;
	}
	return NULL;
}

bool find_best_work(enum place prof, int* out_product, int* out_step) {
	if (factory.ready_places[prof] <= 0) return false;

	for (int step = _PHASE_COUNT-1; step >= 0; --step) {
		for (int prod = 0; prod < _PRODUCT_COUNT; ++prod) {
			if (product_pipeline[prod][step] == prof && 
			    factory.parts[prod][step] > 0) {
				*out_product = prod;
				*out_step = step;
				return true;
			}
		}
	}

	return false;
}

static bool has_usable_machine(enum place p) {
	int total_usable = factory.ready_places[p] + 
			   factory.busy_places[p] - 
			   factory.remove_pending[p];
	return total_usable > 0;
}

bool is_profession_needed(enum place prof) {
	if (!has_usable_machine(prof)) return false;

	for (int prod = 0; prod < _PRODUCT_COUNT; ++prod) {
		for (int target_step = 0; target_step < _PHASE_COUNT; ++target_step) {
			if (product_pipeline[prod][target_step] != prof) continue;

			if (factory.parts[prod][target_step] > 0) return true;

			for (int s = 0; s < target_step; ++s) {
				bool waiting_at_s = (factory.parts[prod][s] > 0);
				bool in_prog_at_s = (factory.in_progress[prod][s] > 0);

				if (!waiting_at_s && !in_prog_at_s) continue;

				int first_check_step = in_prog_at_s ? (s+1) : s;
				bool path_clear = true;

				for (int k = first_check_step; k < target_step; ++k) {
					enum place req = product_pipeline[prod][k];
					if (factory.active_workers[req] <= 0 ||
					    !has_usable_machine(req)) {
						path_clear = false;
						break;
					}
				}

				if (path_clear) return true;
			}
		}
	}

	return false;

}

void* worker_routine(void* arg) {
	(void)arg;
	return NULL;
}



// MAIN FUNC --------------------------------------------------------

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

		int place = find_string_in_array(place_str, _PLACE_COUNT, arg2);
		if (place < 0) {
			fprintf(stderr, "Error: Unknown workplace '%s' in 'start' command.\n", arg2);
			continue;
		}

		worker_t* w = (worker_t*)malloc(sizeof(worker_t));
		if (!w) {
			perror("malloc");
			continue;
		}

		w->name = strdup(arg1);
		if (!w->name) {
			perror("strdup");
			free(w);
			continue;
		}

		w->profession = (enum place)place;
		w->should_end = false;

		pthread_mutex_lock(&factory.mtx);
		w->next = factory.workers_head;
		factory.workers_head = w;
		factory.active_workers[place]++;

		if (pthread_create(&w->thread, NULL, worker_routine, w) != 0) {
			perror("pthread_create");
			factory.workers_head = w->next;
			factory.active_workers[place]--;
			pthread_mutex_unlock(&factory.mtx);
			free(w->name);
			free(w);
			continue;
		}

		pthread_cond_broadcast(&factory.cond);
		pthread_mutex_unlock(&factory.mtx);
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

