#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<string.h>
int main(int argc, char *argv[]) 
{ 
	int i;
	pid_t pid;
	printf("****Started Calling****\n");
	printf("Process PID: %d\n", getpid());
	printf("Process Parent PID: %d\n", getppid());
        //char *args[]={"./EXEC",NULL}; 
        if(argc == 1)
        {
        	printf("Number of Processes are not enough\n");
        }
        else{
        	for(i = 1; i < argc; i++)
		{	
			pid = fork();
			if(pid == 0)
			{
				if(strncmp("./",argv[i],2) == 0)
				{
					//char str[] = "./";
    					//strcat(str, argv[i]);
					execv(argv[i], argv);	
				}
				else
				{
					char s[] = "/bin/";
					strcat(s,argv[i]);
					execl(s, argv[i], NULL);
				} 
				//execv(argv[i], argv);
			}
			else{
				wait(NULL);
			}
		}
        }
	
	//execl("/bin/ls", "ls", NULL);
        printf("****Ending Calling****\n");     
    return 0; 
} 
