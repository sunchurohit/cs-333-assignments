#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

//recursive implementation

int main(int argc,char** argv)
{
	int no_children = atoi(argv[1]);
	pid_t pid ;
	pid_t parent_pid = getpid();

	printf("Parent is : %d\n", getpid());
	printf("Number of children: %d\n", no_children);

	pid = 0;

	for(int i=0;i<no_children;i++)
	{
		if(!pid)
		{
			pid = fork();
			if(pid)
			{ printf("Child %d is created\n", pid); wait(NULL); }
		};
	}

	if(getpid() != parent_pid)
	{
		printf("Child %d exited\n", getpid());
	}
	else
	{
		printf("Parent exited\n");
	}
	exit(0);
}