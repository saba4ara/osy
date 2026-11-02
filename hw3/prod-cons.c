#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct Node {
	char* text;
	int x;
	struct Node* next;
} Node;

Node* alloc_node();

void free_linked_list(Node* head);

void* producer(void* arg);

void* consumer(void* arg);

int exit_code = 0;
pthread_mutex_t mut;
pthread_mutex_t print_mut;
sem_t sem;
bool finished = false;
Node *head = NULL, *tail = NULL;



int main(int argc, char* argv[]) {
	int N = (argc == 1) ? 1 : atoi(argv[1]);
	if (N < 1 || N > sysconf(_SC_NPROCESSORS_ONLN)) {
		perror("Invalid N");
		return 1;
	}

	pthread_mutex_init(&mut, NULL);
	pthread_mutex_init(&print_mut, NULL);
	sem_init(&sem, 0, 0);

	// create prod thread
	pthread_t prod_thr;
	int prod_crt_ret = pthread_create(&prod_thr, NULL, producer, &N); // arg = ???
	if (prod_crt_ret != 0) {
		perror("Thread creation");
		return 2;
	}

	// create N cons threads
	pthread_t cons_thrs[N];
	for (int i = 0; i < N; ++i) {
		int* id = malloc(sizeof(int));
		*id = i + 1;
		int cons_crt_ret = pthread_create(&cons_thrs[i], NULL, consumer, id); // arg = id	
		if (cons_crt_ret != 0) {
			perror("Thread creation");
			return 2;
		}
	}

	// join N cons threads	
	for (int i = 0; i < N; ++i) { 
		int cons_jo_ret = pthread_join(cons_thrs[i], NULL);
		if (cons_jo_ret != 0) {
			perror("Thread joining");
			return 3;
		}
	}

	// join prod thread
	int prod_jo_ret = pthread_join(prod_thr, NULL);
	if (prod_jo_ret != 0) {
		perror("Thread joining");
		return 3;
	}

	pthread_mutex_destroy(&mut);
	pthread_mutex_destroy(&print_mut);
	sem_destroy(&sem);

	return exit_code;
}



Node* alloc_node() {
	Node* node = (Node*)malloc(sizeof(Node));
	if (!node) return NULL;
	node->next = NULL;
	return node;
}

void free_linked_list(Node* head) {
	Node* tmp;
	
	while (head) {
		tmp = head;
		head = head->next;
		free(tmp);
	}	
}

void* producer(void* arg) {
	int N = *(int*)arg;
	int ret, x;
	char* text;

	while ((ret = scanf("%d %ms", &x, &text)) == 2) {
		if (x < 0) {
			free(text);
			exit_code = 1;
			break;
		}

		// alloc data struct, text, x
		Node* node = alloc_node();
		if (!node) {
			perror("Allocation");
			return NULL;
		}
		node->text = text;
		node->x = x;	
	
		// append to linked list
		pthread_mutex_lock(&mut);
		
		if (!head) {
			head = node; 
			tail = node;
		}
		else {
			tail->next = node; 
			tail = node;
		}

		pthread_mutex_unlock(&mut);	
		sem_post(&sem);
	}

	pthread_mutex_lock(&mut);	
	finished = true;
	pthread_mutex_unlock(&mut);
	
	// post N times
	for (int i = 0; i < N; ++i) sem_post(&sem);

	// check for mistakes && wait for empty linked list
	if (ret != EOF) {
		exit_code = 1;
	}

	return NULL;
	
}

void* consumer(void* arg) {
	int id = *(int*)arg;
	free(arg);	

	while (1) {
		sem_wait(&sem);
		pthread_mutex_lock(&mut);
		
		if (!head && finished) {
			pthread_mutex_unlock(&mut); 
			break;
		}

		Node* node = head;
		head = head->next;
		if (!head) tail = NULL;		
		pthread_mutex_unlock(&mut);

		pthread_mutex_lock(&print_mut);
		printf("Thread %d:", id);
		for (int i = 0; i < node->x; ++i) {
			printf(" %s", node->text);
		}
		printf("\n");
		pthread_mutex_unlock(&print_mut);
	
		free(node->text);
		free(node);
	}

	return NULL;
}

