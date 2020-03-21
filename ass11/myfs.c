#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<string.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<sys/types.h>

#define clear() printf("\033[H\033[J")
#define SIZE 10
#define max_filesystem 4
#define max_OpenFile 5
#define MAX_FILE_SIZE 100

#define mkfs 1
#define use 2
#define cp 3
#define ls 4
#define rm 5
#define info 6
#define CLEAR 7
#define mv 8
#define EXIT 9

char *valid_dir_name[]={(char*)"C:",(char*)"D:",(char*)"E:",(char*)"F:"};	
struct inode
{
	char filename[15];
	int size,file_offset;
	char permission;
};
struct superblock{
        int bs,ts,free,occupied,start_offset,openfile;
        struct inode I[max_OpenFile];
        char Drive[8];
};
void setup()
{
	clear();
	printf("\033[0;36m");
	printf("---------------------------MYFILESYSTEM--------------------------\n");
	char* user = "Mayukh";
	printf("Owner  = %s : \n",user);
}

int filesystem=0;
struct superblock S[max_filesystem];

void inodeinfo(struct inode);
void metainfo(struct superblock);
void mount(char **);
char *read_line(void);
int getVal(char *);
void spacetokenizer(char*, char**);
void reverse(char*, int); 
char* itoa(int, char*, int);
void fileCreation(int,char *,char);
void writeFile(char *,int,int,char *);
void readFile(char *,int,int,char *);
void writeData(int,char*,int,int);
int coloncounter(char **);
void deleteFile(int,char *);
void store(int);
void initialize(char *,int);

/**************************MAIN********************************/

int main()
{
	//mount(valid_dir_name);
	char *line;
	char *parsed[50];
	int option,i,j,k,count,flag = 0;
	char temp[100],size[10],name[20],block[20],src[20],dest[20],destname[50];
	char temp1[100],srcname[50];
	clear();
	setup();
	while(1)
	{
		if(!flag)
		{
			mount(valid_dir_name);
			flag = 1;
		}
		printf("\033[1;31m");
		printf("\nmyfs> ");
		printf("\033[0;10m");
		line = read_line();
		option = getVal(line);

		switch (option)
		{
			case mkfs:
				spacetokenizer(line,parsed);
				strcpy(size,parsed[3]);
				strcpy(name,parsed[1]);
				strcpy(block,parsed[2]);
				for(i=0;i<strlen(size);i++)
				{
					if(size[i]!='M')
						size[i] = size[i];
					else
					{
						break;
					}
				}
				size[i] = '\0';
				char cmd[1000] = "dd if=/dev/zero of=";
				strcat(cmd,name);
				strcat(cmd," bs=");
				strcat(cmd,block);
				strcat(cmd," count=");
				count=1000000*atoi(size);
				count/=atoi(block);
				char *t = itoa(count,temp,10);
				strcat(cmd,t);
				//printf("\n%s\n",cmd);
				spacetokenizer(cmd,parsed);
				if(!fork())
				{
					execvp(parsed[0],parsed);
					printf("\n!!CAN NOT EXECUTE!!\n");
					exit(0);
				}
				else
				{
					wait(NULL);
				}
				strcpy(S[filesystem].Drive,name);
				S[filesystem].bs = atoi(block);
				S[filesystem].ts = 1000000 * atoi(size);
				S[filesystem].free = S[filesystem].ts / S[filesystem].bs;
				S[filesystem].occupied = 0;
				S[filesystem].openfile = 0;
				S[filesystem].start_offset = sizeof(S[filesystem]);
				metainfo(S[filesystem]);
				filesystem++;
				break;
			case cp:
				spacetokenizer(line,parsed);
				strcpy(temp,parsed[1]);
				int ncolon = coloncounter(parsed);
				if(ncolon == 1 && temp[1]!=':')
				{
					strcpy(temp,parsed[2]);
					for(i=2;i<strlen(temp);i++)
					{
						temp1[i-2] = temp[i];
					}
					temp1[i-2] = '\0';
					temp[2] = '\0';
					strcpy(src,parsed[1]);
					strcpy(dest,temp);
					strcpy(destname,temp1);
					for(i=0;i<filesystem;i++)
					{
						if(strcmp(S[i].Drive,dest)==0)
						{
							fileCreation(i,destname,'w');
							int filesize = getFileSize(src);
							int bno = filesize/S[i].bs;
							char totalbuffer[filesize];
							memset(totalbuffer,0,sizeof(totalbuffer));
							FILE *fd = NULL;
							fd=fopen(src,"r");
                            if(fd==NULL)
							{
                            	printf("\n!!File Opening Error!!\n");
                                exit(0);
                            }
							char buff[S[i].bs];
							int Start=0;
							int R=0;
							
							for(j=0 ; j<bno ; j++)
							{
								fseek(fd,Start,SEEK_SET);
								int b=fread(buff,sizeof(char),sizeof(buff),fd);
								for(k=0;k<b;k++)
									totalbuffer[R++]=buff[j];
								
								Start += b;
							}
							fclose(fd);
							writeFile(destname,i,filesize,totalbuffer);
							printf("\n%dth System partition super block is updated\n",i+1);
						}
					}
				}
				else if(ncolon == 1 && temp[1]==':')
				{
					strcpy(temp,parsed[1]);
					for(i=2;i<strlen(temp);i++)
					{
						temp1[i-2] = temp[i];
					}
					temp1[i-2] = '\0';
					temp[2] = '\0';
					strcpy(src,temp);
					strcpy(dest,parsed[2]);
					strcpy(srcname,temp1);
					int src_index;
					for(i = 0 ; i < filesystem ; i++)
					{
						if(strcmp(S[i].Drive,src)==0)
						{
							src_index=i;
						}
					}
					for(i=0;i<S[src_index].openfile;i++){
						struct inode temp=S[src_index].I[i];
						if(strcmp(temp.filename,srcname)==0)
						{
							char FinalBuffer[temp.size];
							readFile(FinalBuffer,src_index,temp.size,srcname);
							FILE* fd = fopen(dest,"w");
							fwrite(FinalBuffer,sizeof(char),temp.size,fd);
							fclose(fd);
							break;
						}
					}
				}
				else
				{
					strcpy(temp,parsed[1]);
					for(i=2;i<strlen(temp);i++)
					{
						temp1[i-2] = temp[i];
					}
					temp1[i-2] = '\0';
					temp[2] = '\0';
					strcpy(src,temp);
					strcpy(srcname,temp1);
					strcpy(temp,parsed[2]);
					for(i=2;i<strlen(temp);i++)
					{
						temp1[i-2] = temp[i];
					}
					temp1[i-2] = '\0';
					temp[2] = '\0';
					strcpy(dest,temp);
					strcpy(destname,temp1);
					int filesize;
					for(i=0;i<filesystem;i++)
					{
						if(strcmp(S[i].Drive,src)==0)
						{
							for(j=0;j<S[i].openfile;j++)
							{
								struct inode temp = S[i].I[j];
								if(strcmp(temp.filename,srcname)==0)
								{
									filesize=temp.size;
									break;
								}
							}
							printf("\nSelected file size: %d\n",filesize);
							char buffer[filesize];
							
							for(j=0;j<filesize;j++)
								buffer[j]='.';

							readFile(buffer,i,filesize,srcname);
							for(j=0;j<filesystem;j++)
							{
								if(strcmp(S[j].Drive,dest)==0)
								{
									fileCreation(j,destname,'w');
									writeFile(destname,j,filesize,buffer);
									break;
								}
							}
							break;
						}
					}

				}
				
				break;
			case rm:
				spacetokenizer(line,parsed);
				strcpy(temp,parsed[1]);
				for(i=2;i<strlen(temp);i++)
				{
					temp1[i-2] = temp[i];
				}
				temp1[i-2] = '\0';
				temp[2] = '\0';
				strcpy(src,temp);
				strcpy(srcname,temp1);
				for(i=0;i<filesystem;i++)
				{
					if(strcmp(src,S[i].Drive) == 0)
					{
						deleteFile(i,srcname);
						printf("\n!!Successfully deleted file: %s!!\n",srcname);
						break;
					}
				}
				break;
			case mv:
				spacetokenizer(line,parsed);
				strcpy(temp,parsed[1]);
				for(i=2;i<strlen(temp);i++)
				{
					temp1[i-2] = temp[i];
				}
				temp1[i-2] = '\0';
				temp[2] = '\0';
				strcpy(src,temp);
				strcpy(srcname,temp1);
				strcpy(temp,parsed[2]);
				for(i=2;i<strlen(temp);i++)
				{
					temp1[i-2] = temp[i];
				}
				temp1[i-2] = '\0';
				temp[2] = '\0';
				strcpy(dest,temp);
				strcpy(destname,temp1);
				int source_drive_index,dest_drive_index;
				for(i=0;i<filesystem;i++){
					if(strcmp(S[i].Drive,src)==0)
					{
						source_drive_index=i;
					}
					if(strcmp(S[i].Drive,dest)==0)
					{
						dest_drive_index=i;
					}
				}
				int TotalSize;
				for(i=0;i<S[source_drive_index].openfile;i++)
				{
					struct inode temp = S[source_drive_index].I[i];
					if(strcmp(temp.filename,srcname)==0)
					{
						TotalSize = temp.size;
						char Buffer[TotalSize];
						readFile(Buffer,source_drive_index,TotalSize,srcname);
						deleteFile(source_drive_index,srcname);
						fileCreation(dest_drive_index,destname,'w');
						writeFile(destname,dest_drive_index,TotalSize,Buffer);
						break;
					}
				}
				break;
			case use:
				spacetokenizer(line,parsed);
				strcpy(src,parsed[1]);
				strcpy(dest,parsed[3]);
				char *args[] = {(char*)"mv",src,dest,(char*)0};
				if(!fork())
				{
					execvp(args[0],args);
					printf("\n!!CAN NOT EXECUTE!!\n");
					exit(0);
				}
				else
				{
					wait(NULL);
				}
				for(i=0;i<filesystem;i++)
				{
					if(strcmp(S[i].Drive,src)==0)
					{
						strcpy(S[i].Drive,dest);
						printf("\nDrive name changed to: %s\n", S[i].Drive);
					}
				}
				break;
			case ls:
				spacetokenizer(line,parsed);
				for(i=0;i<filesystem;i++)
				{
					if(strcmp(S[i].Drive,parsed[1])==0)
					{
						for(j=0;j<S[i].openfile;j++)
						{
							inodeinfo(S[i].I[j]);
						}
					}
				}
				break;
			case info:
				printf("\nFILESYSTEM INFO:\n");
				for(i=0;i<filesystem;i++)
				{
					metainfo(S[i]);
				}
				break;
			case CLEAR:
				clear();
				setup();
				break;
			case EXIT:
				for(i=0;i<filesystem;i++)
				{
					store(i);
				}
				exit(0);
				break;
		}
	}
	return 0;
}




/*************************APIs***************************/



void inodeinfo(struct inode I)
{
        printf("\nINODE DESCRIPTION : \n");
        printf("File name : %s\n",I.filename);
        printf("File Size : %d\n",I.size);
        printf("File Offset : %d\n",I.file_offset);
        printf("File Permission : %c\n",I.permission);
}

void metainfo(struct superblock s)
{
		int i;
		printf("SUPER BLOCK DESCRIPTION : ");
        printf("Drive: %s\nBlock size : %d\nTotal size: %d\nFree: %d\nOccupied: %d\nStart offset:  %d\nTotal open file: %d\n",s.Drive,s.bs,s.ts,s.free,s.occupied,s.start_offset,s.openfile);
		for(i=0;i<s.openfile;i++)
		{
			inodeinfo(s.I[i]);
        }
}

void mount(char *dir[])
{
	FILE *fd=NULL;
	int flag=0,i;
	for(i=0;i<max_filesystem;i++)
	{
		char* drive = dir[i];
		fd = fopen(drive,"r");
		if(fd!=NULL)
		{
			flag++;
			initialize(drive,i);
			printf("\nSuccessfully Mounted: %s\n",drive);
			fclose(fd);
		}
	}
}

char *read_line(void)
{
  int pos = 0;
  char *buffer = malloc(sizeof(char) * 1024);
  char ch;

  if (!buffer) {
    fprintf(stderr, "\nAllocation error!!\n");
    exit(0);
  }

  while (1) {
    ch = getchar();

    if (ch == EOF) 
    {
      exit(EXIT_SUCCESS);
    } 
    else if (ch == '\n') 
    {
      buffer[pos] = '\0';
      return buffer;
    } 
    else 
    {
      buffer[pos] = ch;
    }
    pos++;
  }
}

int getVal(char *line)
{
	char str[50];
	int i;
	for(i = 0; line[i] != ' ' && line[i] != '\0'; i++)
	{
		str[i] = line[i];
	}
	str[i] = '\0';
	if(strcmp(str,"mkfs")==0)
		return 1;
	if(strcmp(str,"use")==0)
		return 2;
	if(strcmp(str,"cp")==0)
		return 3;
	if(strcmp(str,"ls")==0)
		return 4;
	if(strcmp(str,"rm")==0)
		return 5;
	if(strcmp(str,"info")==0)
		return 6;
	if(strcmp(str,"clear")==0)
		return 7;
	if(strcmp(str,"mv")==0)
		return 8;
	if(strcmp(str,"exit")==0)
		return 9;
}

void spacetokenizer(char* str, char** parsed)
{ 
	int i; 
	for (i = 0; i < 1024; i++) 
	{ 
		parsed[i] = strsep(&str, " "); 
		if (parsed[i] == NULL) 
			break; 
		if (strlen(parsed[i]) == 0) 
			i--; 
	} 
} 


void reverse(char str[], int length) 
{ 
	char ch;
    int start = 0; 
    int end = length -1; 
    while (start < end) 
    {
		ch = str[start];
		str[start] = str[end];
		str[end] = ch;
        start++; 
        end--; 
    } 
} 
  
char* itoa(int num, char* str, int base) 
{ 
    int i = 0; 
    int isNegative = 0; 
  
    if (num == 0) 
    { 
        str[i++] = '0'; 
        str[i] = '\0'; 
        return str; 
    } 
    if (num < 0 && base == 10) 
    { 
        isNegative = 1; 
        num = -num; 
    } 
    while (num != 0) 
    { 
        int rem = num % base; 
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0'; 
        num = num/base; 
    } 
    if (isNegative) 
        str[i++] = '-'; 
  
    str[i] = '\0'; 
  
    reverse(str, i); 
  
    return str; 
} 

void fileCreation(int driveIndex,char *filename,char permission)
{
	if(S[driveIndex].openfile < max_OpenFile-1 ){
		struct inode temp;
		strcpy(temp.filename,filename);
		temp.permission=permission;
		temp.size=0;
		temp.file_offset=S[driveIndex].start_offset;
		S[driveIndex].I[S[driveIndex].openfile]=temp;
		S[driveIndex].openfile++;
	}
	else{
		printf("Max No of File Limit on Disk Exceeded %d",max_OpenFile);
		for(int i=0;i<filesystem;i++){
			store(i);
		}
		exit(0);
	}
}

int getFileSize(char *src)
{
	FILE *fp;
    char ch;
    int size = 0;
    fp = fopen(src, "r");
    if (fp == NULL)
        printf("\nFile unable to open ");
    else 
        printf("\nFile opened ");
    fseek(fp, 0, SEEK_END); 
    size = ftell(fp);   
    printf("The size of given file is : %d\n", size);    
    fclose(fp);
	return size;
}

void writeFile(char *filename,int driveIndex,int data_size,char *buff)
{
	struct inode temp;
	int i;
	for(i=0;i<S[driveIndex].openfile;i++){
		temp=S[driveIndex].I[i];
		if(strcmp(temp.filename,filename)==0)
		{
				if(temp.permission=='w' || temp.permission=='a')
				{
					printf("File is writable");
					printf("TOTAL BYTE TO BE WRITTEN : %d\n",data_size);
					writeData(driveIndex,buff,data_size,i);
				}
				else
				{
					printf("permission denied!! not writable");
				}
				break;
		}
	}
	S[driveIndex].free -= data_size/S[driveIndex].bs;
	S[driveIndex].occupied += data_size/S[driveIndex].bs;
}

void writeData(int driveIndex,char *buff,int data_size,int inode_index)
{
	int i;
	FILE *fd=NULL;
	struct inode temp = S[driveIndex].I[inode_index];
	char* filename=S[driveIndex].Drive;
	if((fd=fopen(filename,"rw+"))!=NULL)
	{
		printf("\n!!successfully opened file!!\n");
	}
	else
	{
		printf("\n!!Writing Aborted!!\n");
		for(i=0;i<filesystem;i++)
		{
			store(i);
		}
		exit(0);
	}
	int offset=S[driveIndex].start_offset;
	printf("\nWriting offset: %d\n",offset);
	if(fseek(fd,offset,SEEK_SET)!=0){
		printf("\n!!Fseek unsuccessful!!\n");
		exit(0);
	}

	int n;
	n = fwrite(buff,sizeof(char),data_size,fd);
	printf("\nBytes written: %d\n",n);
	temp.file_offset=S[driveIndex].start_offset;
	S[driveIndex].start_offset += data_size;
	
	printf("\n!!Written Successfully!!-- %d bytes of data\n",data_size);
	temp.size=data_size;
	S[driveIndex].I[inode_index]=temp;	
	fclose(fd);
}
int coloncounter(char **parsed)
{
	int nc = 0;
	int i = 0;
	while(parsed[i])
	{
		if(strstr(parsed[i],":"))
			nc++;
		i++;
	}
	return nc;
}

void readFile(char *buf,int driveIndex,int data_size,char *filename){
	struct inode temp;
	int i;
	for(i=0;i<S[driveIndex].openfile;i++){
		temp=S[driveIndex].I[i];
		if(strcmp(temp.filename,filename)==0)
		{
			int start=temp.file_offset;
			FILE *fd=NULL;
			char *drive=S[driveIndex].Drive;
			fd=fopen(drive,"r");
			fseek(fd,start,SEEK_CUR);
			int n=fread(buf,sizeof(char),data_size,fd);
			fclose(fd);
			printf("\n!!Reading Successful and read %d bytes of data\n",n);
		}
	}
}

void deleteFile(int driveIndex,char *filename)
{
	struct inode temp;
	int start,filesize,index,start_offset,i,j;
	for(i=0;i<S[driveIndex].openfile;i++)
	{
		temp=S[driveIndex].I[i];
		if(strcmp(temp.filename,filename)==0)
		{
			index = i;
			start_offset = temp.file_offset;
			filesize = temp.size;
			printf("\nFile starting offset %d\n",start_offset);
			printf("\nFile Size = %d\n",filesize);
			
			char buf[filesize];
			memset(buf,0,sizeof(buf));
			
			FILE *fd = NULL;
			char* drive = S[driveIndex].Drive;
			fd=fopen(drive,"rw+");
			
			if(fd==NULL){
				printf("\nError in opening file\n");
				exit(0);
			}
			fseek(fd,start_offset,SEEK_SET);
		    int n=fwrite(buf,sizeof(char),sizeof(buf),fd);
			fclose(fd);
			
			
			int start;	
			printf("\n%d bytes of %s file removed and now shifting back remaining files\n",n,filename);
			fd=fopen(drive,"rw+");
			
			for(j=i;j<S[driveIndex].openfile-1;j++)
			{
				S[driveIndex].I[j]=S[driveIndex].I[j+1];
				temp=S[driveIndex].I[j];
				start=temp.file_offset;
				filesize=temp.size;
				char buf[filesize];
				char replace[filesize];
				memset(buf,0,sizeof(buf));
				fseek(fd,start,SEEK_SET);
				n = fread(buf,sizeof(char),sizeof(buf),fd);
				memset(replace,0,sizeof(replace));
				fseek(fd,start,SEEK_SET);
				fwrite(replace,sizeof(char),sizeof(replace),fd);
				fseek(fd,start_offset,SEEK_SET);
				fwrite(buf,sizeof(char),sizeof(buf),fd);
				temp.file_offset=start_offset;
				S[driveIndex].I[j]=temp;
				start_offset += filesize;	
			}
			S[driveIndex].openfile--;
			S[driveIndex].free += filesize/S[driveIndex].bs;
			S[driveIndex].occupied -= filesize/S[driveIndex].bs;
			S[driveIndex].start_offset = start_offset;
			
			printf("\nShifting accomplished\n");
			fclose(fd);
			break;
		}
	}
}

void store(int driveIndex)
{
	FILE *fd=NULL;
	char *drive = S[driveIndex].Drive;
	fd=fopen(drive,"r+");

	if(fd==NULL)
	{
		printf("\n!!Can not open File!!\n");
		exit(0);
	}

	fseek(fd,0,SEEK_SET);
	int w=fwrite(&S[driveIndex].bs,sizeof(S[driveIndex].bs),1,fd);
	fwrite(&S[driveIndex].ts,sizeof(S[driveIndex].ts),1,fd);
	fwrite(&S[driveIndex].free,sizeof(S[driveIndex].free),1,fd);
	fwrite(&S[driveIndex].occupied,sizeof(S[driveIndex].occupied),1,fd);
	fwrite(&S[driveIndex].start_offset,sizeof(S[driveIndex].start_offset),1,fd);
	fwrite(&S[driveIndex].openfile,sizeof(S[driveIndex].openfile),1,fd);
	fwrite(&S[driveIndex].Drive,2,1,fd);
	fwrite(S[driveIndex].I,sizeof(struct inode),S[driveIndex].openfile,fd);
	if(driveIndex==0)
	{
		fwrite(&filesystem,sizeof(filesystem),1,fd);
	}	
	printf("\nsize of drive : %lu\n",sizeof(S[driveIndex].Drive));
	printf("\nsize os inode : %lu\n",sizeof(struct inode));
	printf("\nDrive: %s\n",S[driveIndex].Drive);
	printf("\nBlock Size: %d\n",S[driveIndex].bs);
	printf("\nTotal Size: %d\n",S[driveIndex].ts);
	printf("\nFree: %d\n",S[driveIndex].free);
	printf("\nOccupied: %d\n",S[driveIndex].occupied);
	printf("\nStart Offset: %d\n",S[driveIndex].start_offset);
	printf("\nNo of Open File: %d\n",S[driveIndex].openfile);
	printf("\nTotal Partition Created so Far: %d\n",filesystem);
	fclose(fd);
}

void initialize(char *drive,int driveIndex)
{
	FILE *fd=NULL;
	fd=fopen(drive,"r");
	if(fd==NULL){
			printf("CAN NOT INITIALIZE SUPER BLOCK\n");
			exit(0);
	}
	fseek(fd,0,SEEK_SET);
	fread(&S[driveIndex].bs,sizeof(S[driveIndex].bs),1,fd);
	fread(&S[driveIndex].ts,sizeof(S[driveIndex].ts),1,fd);
	fread(&S[driveIndex].free,sizeof(S[driveIndex].free),1,fd);
	fread(&S[driveIndex].occupied,sizeof(S[driveIndex].occupied),1,fd);
	fread(&S[driveIndex].start_offset,sizeof(S[driveIndex].start_offset),1,fd);
	fread(&S[driveIndex].openfile,sizeof(S[driveIndex].openfile),1,fd);
	fread(&S[driveIndex].Drive,2,1,fd);
	fread(&S[driveIndex].I,sizeof(struct inode),S[driveIndex].openfile,fd);
	if(driveIndex==0){
		fread(&filesystem,sizeof(filesystem),1,fd);
	}
	printf("\nBlock Size: %d\n",S[driveIndex].bs);
	printf("\nTotal Size: %d\n",S[driveIndex].ts);
	printf("\nFree: %d\n",S[driveIndex].free);
	printf("\nOccupied: %d\n",S[driveIndex].occupied);
	printf("\nStart Offset: %d\n",S[driveIndex].start_offset);
	printf("\nNo of Open File: %d\n",S[driveIndex].openfile);
	printf("\nDrive: %s",S[driveIndex].Drive);
	printf("\nTotal Partition: %d",filesystem);
	fclose(fd);
}