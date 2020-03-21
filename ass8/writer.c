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

void wait(int semid,int semnum)
{
	printf("in wait---\n");
	struct sembuf a;
	a.sem_num=semnum;
	a.sem_op=-1;
	a.sem_flg=0;
	semop(semid,&a,1);
}

void signal(int semid,int semnum)
{
	printf("in signal---\n");
	struct sembuf a;
	a.sem_num=semnum;
	a.sem_op=1;
	a.sem_flg=0;
	semop(semid,&a,1);
}


int main(int argc,char* argv[])
{
	key_t k;
	k=ftok("./",1234);
	int i,temp,fd,sem_id,no;
	char ch;
	int wrt=0;
	sem_id=semget(k,2,IPC_CREAT | 0666);
	printf("semid:%d---\n",sem_id);
	char buf[100];


	while(1)
	{
		printf("\nEnter 0 to quit else continue: ");
		scanf("%d",&no);
		if(no==0)
			break;
		wait(sem_id,wrt);
		printf("\nWRT - %d\n",semctl(sem_id,wrt,GETVAL));
		printf("\nWriting started...\n");
		fd=open("foo.txt", O_WRONLY);
		i = 0;
		printf("Enter String: ");
		while((ch = getchar())!=EOF)
		{
			buf[i] = ch;
			i++;
		}
		buf[i] = '\0';
		temp = write(fd, buf, strlen(buf));
		if(temp<0){
			printf("error on writing.\n");
		}
		if(close(fd) < 0){
			printf("Error while closing.\n");
			exit(0);
		}
		printf("\nWriting ended...\n");
		signal(sem_id,wrt);

	}
	return 0;
}
