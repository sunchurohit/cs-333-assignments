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
	
	int fd = open(argv[1], O_RDWR | O_CREAT);
	printf("Parent : File opened. fd=%d.\n", fd);
	pid_t pid = fork() ;
	int file_size ;

	if(pid != 0)
	{
        file_size = write(fd, "hello world! I am the parent\n", strlen("hello world! I am the parent\n"));
        printf("Parent %d: writing to file %d\n", getpid(),fd );
        if(pid == wait(NULL))
        {
        	printf("Parent : The child has terminated.\n");
        	return 0;
        }
	}
	else
	{
		file_size = write(fd, "hello world! I am the child\n", strlen("hello world! I am the child\n"));
		printf("Child %d: writing to file %d\n", getpid(),fd );
		return 0;
	}
}