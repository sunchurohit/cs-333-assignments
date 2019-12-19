#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sig_handler(int signum)
{
	if(signum == 2)
	{
		char input ;
		printf("Interrupt SIGINT received.\n");
	    printf("Do you really want to quit (y|n)?\n");
	    scanf("%c",&input) ;
	    if(input == 'y')
	    {
	    	exit(0) ;
	    }
	    else
	    {

	    }
	}
	else if(signum == 15)
	{
		char input ;
		printf("Interrupt SIGTERM received.\n");
	    printf("Do you really want to quit (y|n)?\n");
	    scanf("%c",&input) ;
	    if(input == 'y')
	    {
	    	exit(0) ;
	    }
	    else
	    {

	    }
	}
}
	 
int main()
	{
	    signal(SIGINT, sig_handler);
	    signal(SIGTERM,sig_handler) ;
	    while(1)
	    {
	    	//printf("abc\n");
	    } // This is your chance to press CTRL-C
	    return 0;
	}