#include "types.h"
#include "stat.h"
#include "user.h"



// the multi-threaded producer function
void producer(int producer_id, int count)
{
	// setup the producer
	//  .......
	//  .......
	// produce

	//synchronization done in main function

	set_var(0, get_var(0)+1);
	printf(1, "\t\t\t\t[%d]: P: %d %d %d \n", getpid(), producer_id, count, get_var(0));	
	//cleanup
	//  .......
	//  .......    
}

// the mult-threaded consumer function
void consumer(int consumer_id, int count)
{
	// setup the consumer
	//  .......
	//  .......

	//synchronization done in main function
	
	// consumer
	set_var(0, get_var(0)-1);
	printf(1, "\t\t\t\t[%d]: C: %d %d %d \n", getpid(), consumer_id, count, get_var(0));

	// cleanup
	//  .......
	//  .......
	

}

int main(int argc, char *argv[])
{
	
    int bsize;
	int np;
	int nc;
	int pitems;
	int citems;

	if(argc < 6)
	{
		printf(1, "usage: ./prodcon <buffer size> <#producers> <#items produced per producer> <#consumers> <#items consumed per consumer> \n");
		printf(1, "       buffer size denotes the maximum capacity of production.");
		exit();    
	}
	
	// get the arguments 

	bsize = atoi(argv[1]);
	np = atoi(argv[2]);
	pitems = atoi(argv[3]);
	nc = atoi(argv[4]);
	citems = atoi(argv[5]);

	// initialized counters etc.
	init_counters();
	// ...

	// use forks man!!!


	sem_init(0,bsize); // semaphore for producer
	sem_init(1,0); // consumer semaphore
	sem_init(2,1); // this semaphore is used as a lock;

	for(int i=0;i < np ;i++)
	{
		if(fork() > 0)
		{
			continue ;
		}
		else
		{
			for(int j=0; j< pitems;j++)
			{
				sem_down(0);

				sem_down(2);
				producer(i+1,j+1);
				sem_up(2);
				
				sem_up(1);
			}
			exit();
		}
	}

	for(int i=0;i < nc; i++)
	{
		if(fork() > 0)
		{
			continue;
		}
		else
		{
			for(int j=0;j < citems; j++)
			{
				sem_down(1);
				
				sem_down(2);
				consumer(i+1,j+1);
				sem_up(2);

				sem_up(0);
			}
			exit();
		}
	}

	for(int i=0;i< nc + np; i++)
	{
		wait();
	}

	exit();

    /* Todo

	   Create producer and consumer threads/processes and set them up
	   to produce pitems and consume citems each, respectively.

	   The processes need to be synchronized correctly for production
	   and consumption related updates.
       .......

	   .......

       .......

	   Cleanup
	 */
	
}
