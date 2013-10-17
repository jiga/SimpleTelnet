#include<stdio.h>

int main()
{
	char ch;
	printf("\nenter any number \n");
	fflush(stdout);
	read(0,&ch,1);
	//scanf("%d",&i);
	printf("\nyou entered %c\n",ch);
}
