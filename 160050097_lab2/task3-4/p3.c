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
		if(pid == wait(NULL))
		{
			printf("Parent : The child has terminated.\n");
			return 0;
		}
	}
	else
	{	
		char *args[]={"./mycat",NULL};
		execvp(args[0],args);
		return 0;
	}

}
