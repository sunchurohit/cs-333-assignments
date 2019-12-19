#include <pthread.h>
#include <stdio.h>

#define NTHREADS 100

long int counter = 0;
pthread_mutex_t mut;


void *myThread()
{
	for(int i=0; i<1000; i++)
	{
		pthread_mutex_lock(&mut);
		counter++;
		pthread_mutex_unlock(&mut);
	}
}

int main()
{
	if (pthread_mutex_init(&mut, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }

	// Create space for pthread variables
	pthread_t tid[NTHREADS];

	for(int i=0; i<NTHREADS; i++)
	{
		//	Create a thread with default attributes and no arguments
		pthread_create(&tid[i], NULL, myThread, NULL);
	}

	for(int i=0; i<NTHREADS; i++)
	{
		//	Wait for all threads to finish
		pthread_join(tid[i], NULL);
	}

	pthread_mutex_destroy(&mut);

	printf("Counter: %ld \n", counter);

	return 0;
}