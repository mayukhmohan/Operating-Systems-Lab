#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<errno.h>
#include<string.h>

struct data
{
	char msg[500];
	int pid[2];
	int ready,change,count;
};
struct data *p;

void signalHandler(void);

int main(int argc,char* argv[])
{
	key_t key;
	key=ftok("./",1234);
	int shm_id;
	signal(SIGUSR1,signalHandler);

	printf("Process ID: %d\n",getpid());
	
	
	shm_id=shmget(key,sizeof(struct data),IPC_CREAT | 0666);
	printf("SHMID -:> %d\n",shm_id);
	if(shm_id==-1)
		printf("%d\n",errno);
	p = (struct data *)shmat(shm_id, NULL, 0);
	if((int)p<0)
	{
		printf("attaching isn't possible for shmat()\n");
		exit(0);
	}

	p->count = 1;
	p->ready = 1;
	p->change = 1;
	while(p->ready);
	p->pid[0] = getpid();
	int i = 0;
	char ch;
	while(1)
	{
	
		//signal(SIGUSR1,signalHandler);
		if(p->change == 1){
			printf("CHAT1:");
			i=0;
			while((ch = getchar())!=EOF)
			{
				p->msg[i] = ch;
				i++;
			}
			p->change = 3;
			kill(p->pid[1],SIGUSR1);
		}
		else if(p->change == 0)
			break;
	}
	p->count--;
	while(p->count);
	shmdt((void *)p);
	shmctl(shm_id,IPC_RMID,NULL);
	return 0;
}

void signalHandler()
{
	int i = 0;
	printf("\nCHAT2:");
	while(p->msg[i]!='\0')
	{
		putchar(p->msg[i]);
		i++;
	}
	putchar('\n');	
	if(strcmp(p->msg,"bye")==0)
	{
		p->change = 0;
		return;
	}
	for(i=0;i<500;i++)
		p->msg[i] = '\0';
	p->change = 1;
}
