#include "types.h"
#include "stat.h"
#include "user.h"

#define MAX_SZ 1000


/*
	
	pausing the own process, The code just after the pause (maybe the first or second lines after pause), gets executed due to small delays.

*/

void do_some_job(int starts)
{
	int pid;
	int ends;

	pid = getpid();

	printf(1, "\nchild %d: starts executiion at %d \n", pid, starts );

	sleep(10);
	
	ends = uptime();
	printf(1, "child %d: ends at %d \n", pid, ends);
	printf(1, "child %d: time taken = %d ticks \n", pid, (ends - starts) );

}

int main(int argc, const char **argv) 
{
	int pid = getpid();

	printf(1,"I am about to get paused%d\n",uptime());

	pause(pid,500);

	printf(1,"I am out of pause!!!%d\n",uptime());

	printf(1,"I am out of pause!!!%d\n",uptime());
	printf(1,"I am out of pause!!!%d\n",uptime());

	printf(1,"I am out of pause!!!%d\n",uptime());


	exit();

}
