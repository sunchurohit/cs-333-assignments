#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

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

        int no_and = 0; int no_commands = 0;

        if(tokens[0] == NULL)
        {
        	continue ;
        }

        if(!strcmp(tokens[0],"&&"))
        {
        	printf("Syntax error: Unexpected &&\n"); continue ;
        }

        if(!strcmp(tokens[0],"exit"))
        {
        	exit(0);
        }

        for(i=0;tokens[i]!=NULL;i++){
        	if(!strcmp(tokens[i],"&&"))
        	{
        		no_and++ ;
        	}
        }

        for(i=0;tokens[i]!=NULL;i++)
        {
        	if(!strcmp(tokens[i],"&&"))
        	{}
        }

        if(!no_and)
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
        else
        {
        	// printf("%d\n",no_and );

        	pid_t pid ;

        	pid = 1;
        	
        	char **temp = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *)) ;

        	int temp_counter = 0;

        	for(i=0;tokens[i]!=NULL;i++)
        	{
        		if(!strcmp(tokens[i],"&&"))
        		{
        			temp_counter = 0;

        			if(pid)
        			{
        				if(temp[0] != NULL){
        				pid = fork();
        				if(!pid)
        				{
        					if(execvp(temp[0],temp) < 0)
        					{
        						printf("%d error: ", getpid());
        			for(int k=0;temp[k]!=NULL;k++)
        			{
        				printf("%s ", temp[k]);
        			}
        			printf("not found\n");
        					}
        					exit(0) ;
        				};
        			}
        			}
        		}
        		else
        		{
        			temp[temp_counter] = (char *)malloc(MAX_NUM_TOKENS * sizeof(char));
        			temp[temp_counter] = tokens[i];
        			temp_counter++;
        		}
        	}

        	if(temp[0] != NULL)
        	{
        		pid = fork();

        		if(!pid)
        		{
        			if(execvp(temp[0],temp) < 0)
        					{
        						printf("%d error: ", getpid());
        			for(int k=0;temp[k]!=NULL;k++)
        			{
        				printf("%s ", temp[k]);
        			}
        			printf("not found\n");
        					}

        			exit(0);
        		};
        	};

        }











       
        // Freeing the allocated memory	
        for(i=0;tokens[i]!=NULL;i++){
            free(tokens[i]);
        }
        free(tokens);
    }
     

}

                
