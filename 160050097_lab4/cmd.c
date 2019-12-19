#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc,char* argv[])
{
	if(argc <= 1)
	{
		printf(1,"not enough arguements\n");
	}

	

	int pid ;
	pid = fork();

	if(pid)
	{
		wait();
	}
	else
	{
		exec(argv[1],argv + 1);
		exit();
	}

	exit();
}