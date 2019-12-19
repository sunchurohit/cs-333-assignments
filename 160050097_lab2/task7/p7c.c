#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

int main(int argc,char** argv)
{	
	int no_children = atoi(argv[1]);
	pid_t pid =1 ; int rand1; int status = 0; pid_t wpid ; 
	pid_t parent_pid = getpid();

	srand(time(0));

	int rand_arr[no_children];
	for(int i=0;i<no_children;i++)
	{	
		rand_arr[i] = rand() ;
	}

	pid_t pid_arr[no_children];

	//printf("%d %d %d\n",rand_arr[0],rand_arr[1],rand_arr[2]);

	printf("Parent is : %d\n", getpid());
	printf("Number of children: %d\n", no_children);

	for(int i=0;i<no_children;i++)
	{
		if(pid)
		{
			pid = fork();
			pid_arr[i] = pid;

			if(pid)
			{
				printf("Child %d is created\n",pid);	
			}
			else
			{
				sleep((rand_arr[i] % 2*no_children) + 2) ;
				exit(0);
			}		
		};// end only if
	}

	for(int i=no_children-1;i>=0;i--)
	{
		waitpid(pid_arr[i],NULL,0);
		printf("Child %d exited\n", pid_arr[i]);
	};

	printf("Parent exited\n");
	exit(0) ;
}