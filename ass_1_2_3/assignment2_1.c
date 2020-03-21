/**
* For Concurrent Read and Write part
*/
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h>
#include<unistd.h>  
#include<string.h>
#include<stdlib.h>
int main()
{
	int fd=open("foo.txt", O_RDONLY);
	
	printf("fd = %d\n", fd); 
      
        if (fd ==-1) 
        { 
		perror("Error in Opening");
		exit(0);                  
        } 
	printf("File opened\n");
	
	pid_t pid;
	
	pid = fork();
	
	
	 
	printf("started\n");
	char *buf = "HELLO WORLD"; 
	/*int s = write(fd, buf, strlen(buf));
	if(s<0){
		printf("error on writing\n");
	}									//for write uncomment others
	
	if(close(fd) < 0){
		printf("Error while closing\n");
		exit(0);
	}*/
	
	//printf("File closed 1\n"); 
	
	//fd=open("foo.txt", O_RDONLY);
	char character[11]; 
	

	printf("Those bytes are as follows parent:\n");
	
	if(read(fd, character, 6) > 0)
	{
		character[6] = '\0';
		printf("%s\n",character);
	}
	
	if(close(fd) < 0){
		printf("Error while closing\n");
		exit(0);
	}
	
	printf("File closed 2\n");
	
	return 0;
}
