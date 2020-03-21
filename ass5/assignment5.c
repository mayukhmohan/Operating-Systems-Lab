#include<stdio.h>
#include<stdlib.h>
#include<unistd.h> 
#include<fcntl.h>
int main(int argc, char *argv[]) 
{
	pid_t pid1,pid2;
	int pfd[2];
	
	pid1 = fork();
	

			if(pid1 == 0)
			{
				if(pipe(pfd) < 0)
				{
					printf("Error during pipe creation\n");
					exit(0);
				}
				pid2 = fork();
				if(pid2 == 0)
				{	
					close(pfd[0]);
					//pfd[1] = open("test.txt",O_RDWR); 
					dup2(pfd[1],1);
//					close(pfd[1]);
					char *args[] = {"./write",NULL};
					execvp(args[0],argv);
				}
				else
				{
					close(pfd[1]);
					wait(NULL);
					dup2(pfd[0],0);
					char *args[] = {"./wc",NULL};				        
					execvp(args[0], argv);
				}
			}
			else{
				wait(NULL);
			}
    
	
	return 0;
}
