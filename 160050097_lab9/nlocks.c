#include <pthread.h>
#include <stdio.h>

#define NTHREADS 10

int Counter[10];
pthread_mutex_t mut[10];

void* myThread(void *ii)
{
	int i = *( (int *) ii);

	for(int j=0;j<1000;j++)
	{
		pthread_mutex_lock(&mut[i]);
		Counter[i]++;
		pthread_mutex_unlock(&mut[i]);
	}
}

int numbers[10];

int main()
{
	for(int i=0;i<10;i++)
	{
		Counter[i] = 0;
	}

	for(int i=0;i<10;i++)
	{
		numbers[i] = i;
	}

	pthread_t tid[NTHREADS];

	for(int i=0;i<10;i++)
	{
		if (pthread_mutex_init(&mut[i], NULL) != 0)
    	{
        	printf("\n mutex %d init failed\n",i);
        	return 1;
    	}
	}

	for(int i=0;i<NTHREADS;i++)
	{
		pthread_create(&tid[i],NULL,myThread,&numbers[i]);
	}

	for(int i=0;i<1000;i++)
	{
		for(int j=0;j<10;j++)
		{
			pthread_mutex_lock(&mut[j]);
			Counter[j]++;
			pthread_mutex_unlock(&mut[j]);
		}
	}

	for(int i=0; i<NTHREADS; i++)
	{
		//	Wait for all threads to finish
		pthread_join(tid[i], NULL);
	}

	for(int i=0; i<NTHREADS; i++)
	{
		//	destroying the locks
		pthread_mutex_destroy(&mut[i]);
	}

	for(int i=0;i<10;i++)
	{
		printf("Counter[%d]: %d\n",i,Counter[i]);
	}

	return 0;
}