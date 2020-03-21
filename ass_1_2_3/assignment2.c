#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h>
#include<unistd.h>  
#include<string.h>
#include<stdlib.h>
int main()
{
	/*int fd=open("foo.txt", O_RDWR);
	
	printf("fd = %d\n", fd); 
      
        if (fd ==-1) 
        { 
		perror("Error in Opening");
		exit(0);                  			//for write uncomment comment others
        } 
	printf("File opened\n");
	*/
	int fd=open("foo.txt", O_RDONLY);
	char character[11]; 
	
	pid_t pid;
	
	pid = fork();
	
	if(pid > 0){
		wait(NULL);
		/*printf("Parent started\n");							//for write uncomment comment others
		char *buf = "HELLO WORLD"; 
		int s = write(fd, buf, strlen(buf));
		if(s<0){
			printf("error on writing parent\n");
		}
		
		if(close(fd) < 0){
			printf("Error while closing parent1");
			exit(0);
		}
		
		printf("File closed parent1\n"); */
		
		/*fd=open("foo.txt", O_RDONLY);
		char character[11]; 
		*/

		printf("Those bytes are as follows parent:\n");
		
		if(read(fd, character, 6) > 0)
		{
			character[6] = '\0';
			printf("%s parent\n",character);
		}
		
		if(close(fd) < 0){
			printf("Error while closing parent2");
			exit(0);
		}
		
		printf("File closed parent2\n");
		printf("parent closed\n");
	}
	else if(pid == 0){
		/*printf("child started\n");
		char *buf = "hello world"; 
		int s = write(fd, buf, strlen(buf));
		if(s<0){
			printf("error on writing child\n");
		}
		
		if(close(fd) < 0){								//for write uncomment others
			printf("Error while closing child1");
			exit(0);
		}
		
		printf("File closed child1\n"); */
		
		/*fd=open("foo.txt", O_RDONLY);
		char character[11]; 
		*/
		printf("Those bytes are as follows child:\n");
		
		if(read(fd, character, 5) > 0)
		{
			character[5] = '\0';
			printf("%s child\n",character);
		}
		
		if(close(fd) < 0){
			printf("Error while closing child2");
			exit(0);
		}
		
		printf("File closed child2\n");
		printf("Child closed\n");
	}
	 	
	return 0;
}
