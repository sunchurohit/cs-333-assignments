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
	if(pid != 0)
	{
		printf("Parent : My process ID is : %d\n", getpid());
		printf("Parent : The child process ID is : %d\n", pid);
	}
	else
	{
		printf("Child : My process ID is : %d\n", getpid());
		printf("Child : The parent process ID is : %d\n", getppid());
	}

	exit(0);
}
