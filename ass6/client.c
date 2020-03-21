#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<errno.h>
#include<string.h>

struct data
{
	long long int data;
	int cdone,sdone,final;//ready;
	int term;
};
struct data *p;

int main(int argc,char* argv[])
{
	key_t key;
	key=ftok("./",1234);
	int shm_id;

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
	//p->ready = 0;
	p->term = 1;
	p->cdone = 0;
	int i = 0;
	while(i<20)
	{
		//if(p->ready)
		//{
			if(p->sdone==1)
				p->cdone = 0;
			sleep(1);
			//printf("Enter Data:");
			//scanf("%d",&p->data);
			printf("SENDING: %d\n",i+1);
			p->data = i+1;
			i++;	
			p->cdone = 1;		
		//}
	}
	while(p->final == 0)
		sleep(1);
	shmdt((void *)p);
	shmctl(shm_id,IPC_RMID,NULL);
	return 0;
}
