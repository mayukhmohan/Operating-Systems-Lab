#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include<errno.h>
#include<string.h>

#define n 10

union semun
{
	int val;
	struct semid_ds *buf;
	ushort* array;
}arg;

struct data
{
	int circularQueue[10];
	int in,out,cflag,sflag;
};

struct data *p;
void wait(int semid,int semnum)
{
	struct sembuf a;
	a.sem_num=semnum;
	a.sem_op=-1;
	a.sem_flg=0;
	semop(semid,&a,1);
}

void signal(int semid,int semnum)
{
	struct sembuf a;
	a.sem_num=semnum;
	a.sem_op=1;
	a.sem_flg=0;
	semop(semid,&a,1);
}

/*
struct semid_ds {
. . .
struct sem *sem_base; /*pointer to first semaphore in set
ushort_t sem_nsems; /* number of semaphores in set 
};


struct sem {
ushort semval; /*semaphore value
pid_t sempid; /*pid of last operation 
ushort semcnt; /* # waiting for increase in semval 
ushort semzcnt; /* # waiting for semval == 0 
};


struct sembuf {
ushort sem_num; /* semaphore # 
short sem_op; /*semaphore operation
short sem_flg; /*operation flags
};


*/

int main(int argc,char* argv[])
{
	pid_t pid;
	key_t key1,key2;
	key1=ftok("./",1234);
	int shm_id, sem_id, item;

	shm_id=shmget(key1,sizeof(struct data), 0666);
	printf("SHMID -:> %d\n",shm_id);

	if(shm_id==-1)
		printf("%d\n",errno);

	key2=ftok("./",4567);
	sem_id = semget(key2, 3, IPC_CREAT|0666);
	printf("SEMID -:> %d\n",sem_id);

	if(sem_id==-1)
		printf("%d\n",errno);


	p = (struct data *)shmat(shm_id, NULL, 0);
	if((int)p<0)
	{
		printf("attaching isn't possible for shmat()\n");
		exit(0);
	}

	//semop (sem_id, struct sembuf *sops, size_t nsops);
	int empty=0,full=1,mutex=2,no;
	if(p->cflag != 1)
	{
		p->cflag = 1;
		p->out = 0;
	}

	while(1)
	{

		/*printf("Enter 0 to quit else continue: ");
		scanf("%d",&no);
		if(no==0)
			break;*/
		wait(sem_id,full);
		wait(sem_id,mutex);
		sleep(1);

		p->out = (p->out + 1)%n;
		item = p->circularQueue[p->out];
		printf("\nConsumer: %d:-> consumed %d\n",getpid(),item);

		signal(sem_id,mutex);
		signal(sem_id,empty);
	}	

	shmdt((void *)p);
	shmctl(shm_id,IPC_RMID,NULL);
	return 0;
}
