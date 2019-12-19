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
		int pid = waitpid(-1,NULL,WNOHANG) ; 
		printf("Parent %d: Child %d is exited\n",getpid(),pid);
	};
}

int main()
{
	signal(SIGCHLD,sig_handler) ;
	unsigned int temp1 ; int temp ; 
	pid_t parentpid , childpid, grandpid , pid1 , pid2  ;
	parentpid = getpid() ;
	
	pid1= fork() ;
	if(pid1 > 0)
	{
		while(1)
		{

		};
		//childpid = pid1 ;
	}
	else
	{
		pid2 = fork();
		if(pid2)
		{
			temp1 = (unsigned int) getpid();
			temp = rand_r(&temp1) % 10 ;
			printf("Child %d is created by parent %d (sleeps for %d seconds)\n", getpid(),getppid(),temp);
			sleep(temp) ;
		}
		else
		{
			temp1 = (unsigned int) getpid();
			temp = rand_r(&temp1) % 10 ;
			printf("Child %d is created by parent %d (sleeps for %d seconds)\n", getpid(),getppid(),temp);
			sleep(temp) ;
		}
	}
	exit(0) ;

}