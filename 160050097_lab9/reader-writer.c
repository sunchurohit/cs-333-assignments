#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

struct thread_data {
	int index ;
	int delay ;
};

struct read_write_lock
{
	pthread_mutex_t mut;
	pthread_cond_t cond_r;
	pthread_cond_t cond_w;
	int r; //number of readers
	int w; // number of writers
	int inw; // writers inside the lock

	// If required, use this strucure to create
	// reader-writer lock related variables


}rwlock;

long int data = 0;			//	Shared data variable

void InitalizeReadWriteLock(struct read_write_lock * rw)
{
	pthread_mutex_init(&(rw->mut), NULL);
	pthread_cond_init(&(rw->cond_r), NULL);
	pthread_cond_init(&(rw->cond_w), NULL);
	// rw->mut =  PTHREAD_MUTEX_INITIALIZER; 
	// rw->cond_r = PTHREAD_COND_INITIALIZER;
	// rw->cond_w = PTHREAD_COND_INITIALIZER;
	rw->r = 0;
	rw->w = 0;
	rw->inw = 0;
}


// The pthread based reader lock
void ReaderLock(struct read_write_lock * rw)
{
	pthread_mutex_lock(&(rw->mut));
	// printf("Again Stuck! %d %d",rw->w,rw->inw);
	while(rw->w > 0)
	{
		pthread_cond_wait(&(rw->cond_r), &(rw->mut));
	}
	rw->r ++;
	pthread_mutex_unlock(&(rw->mut));
}	

// The pthread based reader unlock
void ReaderUnlock(struct read_write_lock * rw)
{
	pthread_mutex_lock(&(rw->mut)) ;
	rw->r --;
	if(rw->r == 0)
	{
		pthread_cond_broadcast(&(rw->cond_w)) ;
	}
	pthread_mutex_unlock(&(rw->mut)) ;
}

// The pthread based writer lock
void WriterLock(struct read_write_lock * rw)
{
	pthread_mutex_lock(&(rw->mut));
	rw->w ++;
	//printf("I'm stuck! \n");
	while(rw->r > 0 || rw->inw > 0)
	{
		pthread_cond_wait(&(rw->cond_w), &(rw->mut));
	}

	rw->inw++;
	pthread_mutex_unlock(&(rw->mut)) ;
}

// The pthread based writer unlock
void WriterUnlock(struct read_write_lock * rw)
{
	pthread_mutex_lock(&(rw->mut)) ;
	rw->w--;
	rw->inw--;
	//printf("I'm leaving %d\n", rw->w);
	pthread_cond_broadcast(&(rw->cond_w)) ;
	pthread_cond_broadcast(&(rw->cond_r)) ;

	pthread_mutex_unlock(&(rw->mut)) ;
}

//	Call this function to delay the execution by 'delay' ms
void delay(int delay)
{
	struct timespec wait;

	if(delay <= 0)
		return;

	wait.tv_sec = delay / 1000;
	wait.tv_nsec = (delay % 1000) * 1000 * 1000;
	nanosleep(&wait, NULL);
}

// The pthread reader start function
void *ReaderFunction(void *args)
{
	struct thread_data *m = (struct thread_data *) args;

	//	Delay the execution by arrival time specified in the input
	delay(m->delay);
	//	....
	//  Get appropriate lock
	ReaderLock(&rwlock) ;
	//	Display  thread ID and value of the shared data variable
	//
	printf("	Reader %d, data: %ld\n",m->index,data);
    //  Add a dummy delay of 1 ms before lock release  
	//	....
	delay(1) ;
	ReaderUnlock(&rwlock);
}

// The pthread writer start function
void *WriterFunction(void *args)
{
	//	Delay the execution by arrival time specified in the input
	struct thread_data *m = (struct thread_data *) args;

	delay(m->delay);

	WriterLock(&rwlock) ;
	//	Display  thread ID and value of the shared data variable
	//
	data++;

	printf("Writer %d, data: %ld\n",m->index,data);
    //  Add a dummy delay of 1 ms before lock release  
	//	....
	delay(1) ;
	WriterUnlock(&rwlock);
	//	....
	//
	//  Get appropriate lock
	//	Increment the shared data variable
	//	Display  thread ID and value of the shared data variable
	//
    //  Add a dummy delay of 1 ms before lock release  
	//	....
	
}



int main(int argc, char *argv[])
{
	pthread_t *threads;
	struct argument_t *arg;
	
	long int reader_number = 0;
	long int writer_number = 0;
	long int thread_number = 0;
	long int total_threads = 0;	
	
	int count = 0;			// Number of 3 tuples in the inputs.	Assume maximum 10 tuples 
	int rws[10];			
	int nthread[10];
	int delay[10];

	InitalizeReadWriteLock(&rwlock) ;

	//	Verifying number of arguments
	if(argc<4 || (argc-1)%3!=0)
	{
		printf("reader-writer <r/w> <no-of-threads> <thread-arrival-delay in ms> ...\n");		
		printf("Any number of readers/writers can be added with repetitions of above mentioned 3 tuple \n");
		exit(1);
	}

	//	Reading inputs
	for(int i=0; i<(argc-1)/3; i++)
	{
		char rw[2];
		strcpy(rw, argv[(i*3)+1]);

		if(strcmp(rw, "r") == 0 || strcmp(rw, "w") == 0)
		{
			if(strcmp(rw, "r") == 0)
				rws[i] = 1;					// rws[i] = 1, for reader
			else
				rws[i] = 2;					// rws[i] = 2, for writer
			
			nthread[i] = atol(argv[(i*3)+2]);		
			delay[i] = atol(argv[(i*3)+3]);

			count ++;						//	Number of tuples
			total_threads += nthread[i];	//	Total number of threads
			
			if(rws[i] == 1)
			{
				reader_number += nthread[i] ;
			}
			else
			{
				writer_number += nthread[i] ;
			}
		}
		else
		{
			printf("reader-writer <r/w> <no-of-threads> <thread-arrival-delay in ms> ...\n");
			printf("Any number of readers/writers can be added with repetitions of above mentioned 3 tuple \n");
			exit(1);
		}
	}

	//	Create reader/writer threads based on the input and read and write.

	pthread_t reads[reader_number] ;
	pthread_t writes[writer_number] ;

	int read_p = 1 ;
	int write_p = 1;

	struct thread_data reader_threads[reader_number];
	struct thread_data writer_threads[writer_number];

	for(int i=0; i < count ; i++)
	{
		if(rws[i] == 1)
		{
			for(int k=0;k < nthread[i]; k++)
			{
				reader_threads[read_p-1].index = read_p;
				reader_threads[read_p-1].delay = delay[i] ;

				pthread_create(&reads[read_p-1],NULL,ReaderFunction,&reader_threads[read_p-1]);

				read_p++;
			}
			// read
		}
		else
		{
			for(int k=0;k < nthread[i]; k++)
			{
				writer_threads[write_p-1].index = write_p;
				writer_threads[write_p-1].delay = delay[i];
				pthread_create(&writes[write_p-1],NULL,WriterFunction,&writer_threads[write_p-1]);
				write_p++;
			}
			
		}
	}

	read_p = 0;
	write_p = 0;

	//  Clean up threads on exit

	for(int i= 0 ; i < count ; i++)
	{
		if(rws[i] == 1)
		{
			for(int k=0;k < nthread[i];k++)
			{
				pthread_join(reads[read_p], NULL);
				read_p++;
			}
		}	
		else
		{
			for(int k=0;k < nthread[i];k++)
			{
				pthread_join(writes[write_p],NULL);
				write_p++;
			}
		}
	}

	exit(0);
}
