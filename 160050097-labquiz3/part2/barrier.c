#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "barrier.h"

void barrier_init(struct barrier_t * barrier, int nthreads)
{
	/*
		Todo
	*/
	barrier->currt = 0;
	barrier->nt = nthreads ;
	pthread_mutex_init(&barrier->mutex,NULL);
	pthread_cond_init(&barrier->cond,NULL) ;

}

void barrier_wait(struct barrier_t *barrier)
{

	pthread_mutex_lock(&barrier->mutex) ;	
	barrier->currt++;

	if(barrier->currt != barrier->nt)
	{
		pthread_cond_wait(&barrier->cond,&barrier->mutex) ;		
	}
	else
	{
		barrier->currt = 0;	
		pthread_cond_broadcast(&barrier->cond);	
	}

	pthread_mutex_unlock(&barrier->mutex);

	return ;
	/*
		Todo
	*/

}