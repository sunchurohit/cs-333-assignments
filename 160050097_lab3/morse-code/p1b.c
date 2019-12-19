#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <signal.h>

void sig_handler(int signum)
{
	if(signum == SIGCHLD)
	{
		//int pid = waitpid(-1,NULL,WNOHANG) ;
		pid_t pid = wait(NULL) ;
		printf("Child %d is exited\n", pid);
	};
}

int main(int argc,char** argv)
{	
	signal(SIGCHLD,sig_handler) ;

	int no_children = atoi(argv[1]);
	pid_t pid =1 ; int rand1; int status = 0; pid_t wpid ; 
	pid_t parent_pid = getpid();
	int temp  ; unsigned int temp1 ; 

	srand(time(NULL));

	int rand_arr[no_children];
	for(int i=0;i<no_children;i++)
	{	
		rand_arr[i] = rand() ;
	}
	
	//printf("Parent is : %d\n", getpid());
	//printf("Number of children: %d\n", no_children);

	for(int i=0;i<no_children;i++)
	{
		if(pid)
		{
			pid = fork();

			if(pid)
			{				
				//printf("Child %d is created (sleeps for\n",pid);
			}
			else
			{
				temp1 = (unsigned int) getpid();
				temp = rand_r(&temp1) % (2*no_children) + 2 ;
				printf("Child %d is created (sleeps for %d seconds)\n",getpid(),temp);
				sleep(temp) ;				//printf("Child %d exited\n", getpid());
				exit(0);
			}		
		};// end only if
	}
	//while ((wpid = wait(&status)) > 0); 
	//printf("Parent exited\n");

	while(1) ;

	exit(0) ;
}