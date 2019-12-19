#include <pthread.h>

struct barrier_t
{
	/*
		Todo
		Barrier related variables
	*/
	int currt ;
	int nt;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
};

void barrier_init(struct barrier_t *, int);
void barrier_wait(struct barrier_t *);