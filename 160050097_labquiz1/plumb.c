#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define _GNU_SOURCE 
#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

char **tokenize(char *line)
{
    char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
    char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
    int i, tokenIndex = 0, tokenNo = 0;

    for(i =0; i < strlen(line); i++){

        char readChar = line[i];

        if (readChar == ' ' || readChar == '\n' || readChar == '\t') {
            token[tokenIndex] = '\0';
            if (tokenIndex != 0){
                tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
                strcpy(tokens[tokenNo++], token);
                tokenIndex = 0; 
            }
        } 
        else {
            token[tokenIndex++] = readChar;
        }
    }
 
    free(token);
    tokens[tokenNo] = NULL ;
    return tokens;
}


void sig_handler(int signum)
{
	//printf("\nturtle>");
	
	return ;

}

void main(void)
{

	signal(SIGINT,sig_handler);
    char  line[MAX_INPUT_SIZE];            
    char  **tokens;              
    int i;

    while (1) {           
       
        printf("turtle>");     
        bzero(line, MAX_INPUT_SIZE);
        fgets(line, 1024, stdin);
        // gets(line);           
        //printf("Got command %s\n", line);
        line[strlen(line)] = '\n'; //terminate with new line
        tokens = tokenize(line);
   
        //do whatever you want with the commands, here we just print them

        if(tokens[0] == NULL)
        {
        	continue;
        }

        if(!strcmp(tokens[0],"exit"))
        {
        	exit(0);
        }

        int no_pipes = 0; int a1 , b1 ;// a1 and b1 used to store pipe locations

        for(i=0;tokens[i]!=NULL;i++)
        {
        	if(!strcmp(tokens[i],"|"))
        	{
        		no_pipes++ ;
        		if(no_pipes == 1)
        		{
        			a1 = i;
        		}
        		else
        		{
        			b1 = i;
        		}
        	};
        }

        if(no_pipes == 0)
        {
        	pid_t pid = fork();
        	if(pid)
        	{
        		wait(NULL);
        	}
        	else
        	{
        		if(execvp(tokens[0],tokens) < 0)
        		{
        			printf("%d error: ", getpid());
        			for(int k=0;tokens[k]!=NULL;k++)
        			{
        				printf("%s ", tokens[k]);
        			}
        			printf("not found\n");
        			//printf("%s not found\n",tokens ); // just make sure to print the command before leaving
        		}
        		exit(0);
        	}

        }
        else if(no_pipes == 1)
        {
        	//printf("hello\n");
        	char **cmd1 = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
        	char **cmd2 = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
        	//printf("%d\n", a1);
        	for(i=0;i<a1;i++)
        	{
        		cmd1[i] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
                strcpy(cmd1[i],tokens[i]);
        	}
        	cmd1[a1] = NULL;
        	// printf("%s\n", cmd1[0]);
        	// printf("%d\n",a1 );
        	for(i=a1+1;tokens[i]!=NULL;i++)
        	{
        		cmd2[i-a1-1] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
        		strcpy(cmd2[i-a1-1],tokens[i]);
        	}
        	cmd2[i-a1-1] = NULL ;
        	//printf("hello2\n");
        	// printing hello2

        	// printf("%s\n", cmd2[0]);
        	// printf("%s\n", cmd2[1]);
        	int fd[2];
        	if(pipe(fd) < 0)
        	{
        		printf("error in pipe\n");
        	}

        	pid_t pid1,pid2 ;

        	pid1 = fork();

        	if(pid1)
        	{
        		pid2 = fork();
        		if(pid2)
        		{

        		}
        		else
        		{
        			dup2(fd[0],0);
        			close(fd[1]);
        			if(execvp(cmd2[0],cmd2) < 0)
        			{
        				printf("%d error: ",getpid());
        				for(int k=0;cmd2[k]!=NULL;k++)
        			{
        				printf("%s ", cmd2[k]);
        			}
        			printf("not found\n");// make sure to print the other ecommadns before leaving
        			}
        			
        			close(fd[0]);
        			exit(0);
        		}
        	}
        	else
        	{
        		dup2(fd[1],1);
        		close(fd[0]);
        		if(execvp(cmd1[0],cmd1) < 0)
        		{
        			printf("%d error: ",getpid());
        			for(int k=0;cmd1[k]!=NULL;k++)
        			{
        				printf("%s ", cmd1[k]);
        			}
        			printf("not found\n"); // print the commands also
        		}
        		close(fd[1]);
        		exit(0);
        	}

        	close(fd[0]);
        	close(fd[1]);

        	waitpid(pid1,NULL,0);
        	waitpid(pid2,NULL,0);

        }
        else if(no_pipes == 2)
        {
        	char **cmd1 = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
        	char **cmd2 = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
        	char **cmd3 = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));

        	for(i=0;i<a1;i++)
        	{
        		cmd1[i] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
                strcpy(cmd1[i],tokens[i]);
        	}
        	cmd1[i] = NULL;

        	for(i=a1+1;i<b1;i++)
        	{
        		cmd2[i-a1-1] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
        		strcpy(cmd2[i-a1-1],tokens[i]);
        	}
        	// printf("hwllo\n" );
        	// printf("%s\n", cmd2[i-a1-1-1]);
        	cmd2[i-a1-1] = NULL;

        	for(i=b1+1;tokens[i]!=NULL;i++)
        	{
        		cmd3[i-b1-1] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
        		strcpy(cmd3[i-b1-1],tokens[i]);
        	}
        	cmd2[i-b1-1] = NULL;

        	int fd1[2]; int fd2[2]; // two pipes
        	pid_t pid1,pid2,pid3 ;

        	if(pipe(fd1) < 0 || pipe(fd2) < 0)
        	{
        		printf("some error in pipe\n");
        	}

        	pid1 = fork();

        	if(pid1)
        	{
        		pid2 = fork() ;

        		if(pid2)
        		{
        			pid3 = fork();

        			if(pid3)
        			{

        			}
        			else
        			{

        				//waitpid(pid2,NULL,0) ;

        				// code for 3 command
        				dup2(fd2[0],0);
        				close(fd1[0]) ; close(fd1[1]) ;
        				close(fd2[1]) ;
        				if(execvp(cmd3[0],cmd3) < 0)
	        			{
	        				printf("%d error: ",getpid());
	        				// make sure to print the other ecommadns before leaving
	        				for(int k=0;cmd3[k]!=NULL;k++)
        			{
        				printf("%s ", cmd3[k]);
        			}
        			printf("not found\n");

	        			}
	        			//printf("hello\n");
	        			close(fd2[0]);
	        			exit(0);
        			}
        		}
        		else
        		{	
        			// code for 2 command
        			dup2(fd1[0],0);
        			//close(fd1[1]);
        			dup2(fd2[1],1);
        			close(fd2[0]);

        			if(execvp(cmd2[0],cmd2) < 0)
        			{
        				printf("%d error: ",getpid());
        				for(int k=0;cmd2[k]!=NULL;k++)
        			{
        				printf("%s ", cmd2[k]);
        			}
        			printf("not found\n");// make sure to print the other ecommadns before leaving
        			}
        			
        			close(fd1[0]);
        			close(fd2[1]);
        			exit(0);

        		}
        	}
        	else
        	{
        		// code for 1 commnad
        		dup2(fd1[1],1);
        		close(fd1[0]);
        		close(fd2[0]) ; close(fd2[1]) ; // this does not need fd2
        		if(execvp(cmd1[0],cmd1) < 0)
        		{
        			printf("%d error: ",getpid()); 
        			for(int k=0;cmd1[k]!=NULL;k++)
        			{
        				printf("%s ", cmd1[k]);
        			}
        			printf("not found\n");// print the commands also
        		}
        		//printf("hello\n");
        		close(fd1[1]);
        		exit(0);
        	}

        	close(fd1[0]);
        	close(fd1[1]);
        	close(fd2[0]);
        	close(fd2[1]);
        	
        	waitpid(pid1,NULL,0);
        	waitpid(pid2,NULL,0);
        	waitpid(pid3,NULL,0);
        	

        }

        // for(i=0;tokens[i]!=NULL;i++){
        //     printf("found token %s\n", tokens[i]);
        // }
       
        // Freeing the allocated memory	
        for(i=0;tokens[i]!=NULL;i++){
            free(tokens[i]);
        }
        free(tokens);
    }
     

}

                
