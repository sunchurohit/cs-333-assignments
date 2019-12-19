#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

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
    if(signum == SIGINT)
    {
    }
    // do nothing when ctrl+c is pressed
}

void main(void)
{
    char  line[MAX_INPUT_SIZE];            
    char  **tokens;              
    int i;

    while (1) {           
        
        signal(SIGINT,sig_handler) ;
        printf("turtle>");     
        bzero(line, MAX_INPUT_SIZE);
        gets(line);           
        line[strlen(line)] = '\n'; //terminate with new line
        tokens = tokenize(line);
        int n = 0;                                      // n used to get the no of arguements
        
        if(strcmp(tokens[0],"exit") == 0)
        {
            exit(0) ;
        }

        for(i=0;tokens[i]!=NULL;i++){
            n++;
        }

        int multi = 0;                                    // multi used to check whether ";;" is there or not (sequential operations)

        int pipe = 0; int n_pipe ;                        // used to check whether to use pipe or not
        for(i=0;tokens[i]!=NULL;i++)                      // for loop basically checks the above
        {
            if(strcmp(tokens[i],";;") == 0)
            {
                multi++ ;
            };

            if(strcmp(tokens[i],">") == 0)
            {
                pipe = 1; n_pipe = i ;
            }
        };

        if(multi)  // if multiple commands are present do this
        {
            //printf("%d\n", multi);
            char **temparr = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *)); int temp_counter = 0; //int temp_multi = 0;

            for(int j=0;j<n;j++)
            {
                if(strcmp(tokens[j],";;") == 0 )
                {
                
                    temparr[temp_counter] = NULL ;
                    if(temparr[0] == NULL)
                    {

                    }
                    else
                    {
                        int temp_pipe = 0; int n_temp_pipe = 0;
                        for(int j=0;temparr[j]!=NULL;j++)
                        {
                            if(strcmp(temparr[j],">") == 0)
                            {
                                temp_pipe = 1; n_temp_pipe = j ;
                            };
                        }

                        if(temp_pipe)
                        {
                            if(temparr[n_temp_pipe+1] == NULL)
                            {
                                printf("file name not given\n");
                            }
                            else
                            {
                                char **temparr11 = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
                                for(int j=0;j<n_temp_pipe;j++)
                                {
                                    temparr11[j] = temparr[j] ;
                                }
                                temparr11[n_pipe] = NULL ;

                                pid_t pid = fork() ;
                                if(!pid)
                                {   
                                    int file_d = open(temparr[n_temp_pipe+1] ,O_RDWR | O_CREAT, 0666) ;
                                    dup2(file_d,1);
                                    execvp(temparr11[0],temparr11);
                                }
                                else
                                {
                                    wait(NULL);
                                    for(int j=0;temparr11[j]!=NULL;j++)
                                    {
                                        free(temparr11[j]);
                                    }
                                    free(temparr11);    
                                }
                            }
                        }

                        else if(strcmp(temparr[0],"cd") == 0)
                        {
                            if(temp_counter!=2)
                            {
                                printf("Wrong number of arguements\n");
                            }
                            else
                            {
                                int succ = chdir(temparr[1]);
                                if(succ < 0)
                                {
                                    printf("Some error\n");
                                };

                                char cwd[1024] ; 
                                getcwd(cwd,sizeof(cwd));
                                printf("%s\n",cwd);
                            }
                        } 

                        else
                        {
                            pid_t pid = fork() ;

                            if(!pid)
                            {
                                if(execvp(temparr[0],temparr) < 0)
                                {
                                    printf("No command found\n");
                                };
                            }
                            else
                            {
                                wait(NULL) ;
                            } ;
                        }

                    }

                    temp_counter = 0;
                    temparr[0] = NULL ;
                    // for(int j=0;temparr[j]!=NULL;j++)
                    // {
                    //     free(temparr[j]) ;
                    // };
                    //free(temparr) ;
                }
                else
                {
                    temparr[temp_counter] = tokens[j] ; temp_counter++ ;
                }
            }

            if(temparr[0] != NULL)
            {
                int temp_pipe = 0; int n_temp_pipe = 0;
                        for(int j=0;temparr[j]!=NULL;j++)
                        {
                            if(strcmp(temparr[j],">") == 0)
                            {
                                temp_pipe = 1; n_temp_pipe = j ;
                            };
                        }

                        if(temp_pipe)
                        {
                            if(temparr[n_temp_pipe+1] == NULL)
                            {
                                printf("file name not given\n");
                            }
                            else
                            {
                                char **temparr11 = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
                                for(int j=0;j<n_temp_pipe;j++)
                                {
                                    temparr11[j] = temparr[j] ;
                                }
                                temparr11[n_pipe] = NULL ;

                                pid_t pid = fork() ;
                                if(!pid)
                                {   
                                    int file_d = open(temparr[n_temp_pipe+1] ,O_RDWR | O_CREAT, 0666) ;
                                    dup2(file_d,1);
                                    execvp(temparr11[0],temparr11);
                                }
                                else
                                {
                                    wait(NULL);
                                    for(int j=0;temparr11[j]!=NULL;j++)
                                    {
                                        free(temparr11[j]);
                                    }
                                    free(temparr11);    
                                }
                            }
                        }

                        else if(strcmp(temparr[0],"cd") == 0)
                        {
                            if(temp_counter!=2)
                            {
                                printf("Wrong number of arguements\n");
                            }
                            else
                            {
                                int succ = chdir(tokens[1]);
                                if(succ < 0)
                                {
                                    printf("Some error\n");
                                };

                                char cwd[1024] ; 
                                getcwd(cwd,sizeof(cwd));
                                printf("%s\n",cwd);
                            }
                        } 

                        else
                        {
                            pid_t pid = fork() ;

                            if(!pid)
                            {
                                if(execvp(temparr[0],temparr) < 0)
                                {
                                    printf("No command found\n");
                                };
                            }
                            else
                            {
                                wait(NULL) ;
                            } ;
                        }
            }

            

            
        }

        if(pipe && !multi) // if only piping and not multi 
        {
            if(tokens[n_pipe+1] == NULL)
            {
                printf("file name not given\n");
            }
            else
            {
                char **temparr = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
                for(int j=0;j<n_pipe;j++)
                {
                    temparr[j] = tokens[j] ;
                }
                temparr[n_pipe] = NULL ;

                pid_t pid = fork() ;
                if(!pid)
                {   
                    int file_d = open(tokens[n-1] ,O_RDWR | O_CREAT, 0666) ;
                    dup2(file_d,1);
                    execvp(temparr[0],temparr);
                }
                else
                {
                    wait(NULL);
                    for(int j=0;temparr[j]!=NULL;j++)
                    {
                        free(temparr[j]);
                    }
                    free(temparr);    
                }
            }
           
            continue ;
        }


        // handling the case when the command is 'cd'

        if(strcmp(tokens[0],"cd") == 0) // if it is a cd handle it separatly
        {
            //int n = (int) sizeof(tokens) / sizeof(tokens[0]) ;
            //printf("%d\n", n);
            if(n!=2)
            {
                printf("Wrong number of arguements\n");
            }
            else
            {
                int succ = chdir(tokens[1]);
                if(succ < 0)
                {
                    printf("Some error\n");
                };

                char cwd[1024] ; 
                getcwd(cwd,sizeof(cwd));
                printf("%s\n",cwd);
            }
            

            for(i=0;tokens[i]!=NULL;i++)
            {
                free(tokens[i]);
            }
                free(tokens);
            continue ;    
        } ;

        //  this handles all the commands which are dont come in the above categories (multi,pipe,cd)
        pid_t pid = fork() ;

        if(!pid)
        {
            if(execvp(tokens[0],tokens) < 0)
            {
                printf("No command found\n");
            }
        }
        else
        {
            wait(NULL);

        // Freeing the allocated memory 
        for(i=0;tokens[i]!=NULL;i++)
        {
            free(tokens[i]);
        }
            free(tokens);
        }
    }
    
}

                
