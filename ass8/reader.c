#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<string.h>
#include<sys/sem.h>
#include<fcntl.h>


union semun
{
	int val;
	struct semid_ds *buf;
	ushort* array;
}arg;

struct data
{
	int rcount;
	int flag;
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


int main(int argc,char* argv[])
{
	key_t pk;
	pk=ftok("./",4567);
	int shm_id;
		key_t k;
	k=ftok("./",1234);
	int i,sem_id,no,fd;
	char buf[100];
	int wrt=0;
	int mutex=1;


	shm_id=shmget(pk,sizeof(struct data *),IPC_CREAT | 0666);
	printf("shmid for reader:%d---\n",shm_id);
	p=(struct data *)shmat(shm_id,NULL,0);
	if((int)p<0)
	{
		printf("attaching isn't possible\n");
		exit(0);
	}

	sem_id=semget(k,3,IPC_CREAT | 0666);
	printf("semid:%d---\n",sem_id);

	if(p->flag!=1)
	{
		printf("initializing---\n");
		p->rcount=0;
		semctl(sem_id,wrt,SETVAL,1);
		semctl(sem_id,mutex,SETVAL,1);
		
	}
	p->flag=1;

	while(1)
	{
		printf("enter 0 to exit::");
		scanf("%d",&no);
		if(no==0)
			break;
		wait(sem_id,mutex);

		(p->rcount)++;
		if(p->rcount==1)
			wait(sem_id,wrt);
		signal(sem_id,mutex);

		printf("\nWRT - %d\n",semctl(sem_id,wrt,GETVAL));
		printf("\nMUTEX - %d\n",semctl(sem_id,mutex,GETVAL));
		printf("\nReading Started...\n");
		fd=open("foo.txt", O_RDONLY);
		if(read(fd, buf, 100) > 0)
		{
			buf[99] = '\0';
			i = 0;
			while(buf[i]!='\0')
			{
				putchar(buf[i]);
				i++;
			}
		}
		printf("\n");
		if(close(fd) < 0){
			printf("Error while closing.\n");
			exit(0);
		}
		sleep(10);
		printf("\nReading Completed...\n");


		wait(sem_id,mutex);
		(p->rcount)--;
		if(p->rcount==0)
			signal(sem_id,wrt);
		signal(sem_id,mutex);
	}

	shmdt((void *)p);
	shmctl(shm_id,IPC_RMID,NULL);
	semctl(sem_id,0,IPC_RMID,0);
	return 0;
}
