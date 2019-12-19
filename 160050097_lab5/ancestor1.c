#include "types.h"
#include "stat.h"
#include "user.h"

/*PIcture : Grandparent already there

	Hierarchy : Grandparent (already there) -> parent -> child -> grandchild
	
	Parent does the first fork() (ret = fork()) and sleeps for 100

	Child does the second fork() (ret1 = fork()) and sleeps for 150

	Granchild sleeps for 200

	initially, all the values are corresponding to the picture

	after parent dies child gets pid = grandparent pid

	and after child dies , grandchild gets pid = pid of the parent of its grandparent


*/

int main() 
{
	int ret = fork();
	int ret1;
	if (ret == 0) 
	{
		ret1 = fork();

		if(ret1 == 0)
		{
			printf(1, "\nGchild: pid %d\n", getpid());
			printf(1, "Gchild: parent pid %d\n", getppid());
			sleep(200);
			printf(1, "\nGchild: pid %d\n", getpid());
			printf(1, "Gchild: parent pid %d\n", getppid());
			
		}
		else
		{
			printf(1, "\nchild: pid %d\n", getpid());
			printf(1, "child: parent pid %d\n", getppid());
			sleep(150);
			printf(1, "\nchild: pid %d\n", getpid());
			printf(1, "child: parent pid %d\n", getppid());
		}
	}
	else 
	{
		sleep(100);

		printf(1, "\nparent: pid %d\n", getpid());
		printf(1, "parent: parent pid %d \n", getppid());
		printf(1, "parent: child pid %d\n", ret);
	}
	
	exit();
}