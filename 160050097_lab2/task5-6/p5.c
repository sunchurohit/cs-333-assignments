#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

int main()
{
	pid_t pid = fork() ;
	
	if(pid)
	{
		printf("Parent : My process ID is: %d\n", getpid());	  
		printf("Parent : The child process ID is: %d\n", pid);
		sleep(1);
		printf("Parent : The parent has terminated.\n") ;
		exit(0);
	};

	if(!pid)
	{
		printf("Child : My process ID is: %d\n", getpid());		
		printf("Child : The parent process ID is: %d\n", getppid());
		
		sleep(10) ;
		printf("\nChild : My process ID is: %d\n", getpid());		
		printf("Child : The parent process ID is: %d\n", getppid());
		exit(0);
	};
}
