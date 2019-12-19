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

if(close(0) == 0)
{
	int fd = open(argv[1], O_RDWR | O_CREAT);
	printf("Parent: File opened: fd=%d.\n",fd) ;
	pid_t pid = fork() ;
	if(pid)
	{
		if(pid == wait(NULL))
		{ printf("Parent : The child has terminated.\n") ; return 0; }			
	}
	else
	{
		char *args[]={"./mycat",NULL};
		execvp(args[0],args);
	}
				
}
else
{
	printf("error closing the stdin file_descriptor") ;
}

}
