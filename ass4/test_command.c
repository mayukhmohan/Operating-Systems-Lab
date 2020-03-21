#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<string.h>
int main(int argc, char *argv[]) 
{ 
	int i;
	printf("****Started Calling****\n");
	execl("/bin/ls", "ls -la", NULL);
        printf("****Ending Calling****\n");     
    return 0; 
} 
