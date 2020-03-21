#include<stdio.h>
int main()
{
	char string[500];
	char ch;
	int i=0;
	//printf("Enter your string: ");
	while((ch = getchar())!=EOF)
	{
		string[i] = ch;
		i++;
	}
	i = 0;
	//printf("\n");
	while(string[i]!='\0')
	{
		putchar(string[i]);
		i++;
	}
	//printf("\n");
	return 0;
}
