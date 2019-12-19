#include "types.h"
#include "stat.h"
#include "user.h"

#define MAX_SZ 1000000

/*
	Overriding the kill signal with the pause signal.
	As the latest signal is pause , the child will foreever be in pause state (will not terminate)
	and the parent waits forever.
*/


int main(int argc, const char **argv) 
{
	int ret = fork();
	if(ret == 0) 
	{ 
		for (int i = 0; i < MAX_SZ; ++i)
		{
			sleep(5e1);
			printf(1, "child: Not_paused\n");
		}

		exit();
	}

	sleep(5e2);
	printf(1, "parent: sending pause signal to child \n");
	signal_process(ret, SIGNAL_PAUSE);
	
	sleep(5e2);
	printf(1, "parent: sending continue signal to child \n");
	signal_process(ret, SIGNAL_CONTINUE);
	
	sleep(5e2);
	printf(1, "parent: sending kill signal to child \n");
	signal_process(ret, SIGNAL_KILL);
	signal_process(ret, SIGNAL_PAUSE);

	wait();
	printf(1, "parent: child has terminated \n");
	exit();
}