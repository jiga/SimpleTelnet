
/************************************************
*Filename: 
* 
*Authors:  JIGNESH PATEL  <----------  00/00/06 for CS590AC -----------> PRAVEEN LELLA	
*				
*			TCP Auction client in linux - main
*
*       This is a connection oriented client that works with the connectionless
*       server that enables the client to execute commands like a TELNET server with concurrent responses
*******************************************************************/
                                                                                


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <signal.h>

#define PORT 12345 // telnet port
#define MAX_BUF_LEN 200 //MAX length of the RECV buffer 
#define MAX_CMD_LEN 100 //MAX length of the command

//GLOBAL Variables
int sfd; //socket file descriptor
char command[MAX_CMD_LEN]; 
int rflag=0; //flag to indicate the completion of recv thread
int cflag=0; //flag to indicate the completion of command printing

//Function Declarations -- Referring to External ones
int errexit(const char *, ...); //prints error message and exits from the main program
int set_output_normal_color(void); //sets the color of terminal for normal messages
int set_output_error_color(void); //sets the color of terminal for error messages
int set_output_exit_color(void); //sets the color of terminal for exit messages
int connectTCP(const char *, const char *); //connection

//Functions declared -- Here
int telnetSession(void);
void recvfun(void);

int handleCtrlC(void)
{
	char cc;
	cc=0x03;
	send(sfd,&cc,1,0);
	close(sfd);
	exit(0);
	return 0;
}

int main(int argc, char *argv[])
{
	
	char    *service = "12345";      /* service name or port number --Default value */
        char    *host = "localhost";	/* hostname --Default value */
	
	switch (argc) 
	{
		case 1:			/* temporary convinience -- to be removed after  */
       	 		break;
		case 3:
        		host    = argv[1];
        		service = argv[2];
        		break;
        		/* FALL THROUGH */
		default:
        		set_output_error_color();
			fprintf(stdout, "usage: cl <host> <port>\n");
			set_output_default_color();
        		exit(1);
        }//end of switch()
	
	sfd = connectTCP(host, service); //function to connect to TCP TELNET SERVER
	telnetSession(); //Start the Telnet command execution.

	return 0;
}// end of main()


int telnetSession(void)
{
	long origstdinFlags; // to save stdin blocking status
	//char command[MAX_CMD_LEN]; relocated as a GLOBAL VARIABLE
	pthread_t tid; //holds the id of recv thread
	if ((pthread_create(&tid,NULL,(void *)&recvfun, NULL))!=0) 
		errexit("Can't create thread: %s\n",strerror(errno));
	
	signal(SIGINT,&handleCtrlC);

	while(1)
	{
		cflag=0;
		printf("\nSimpTel#");
		//len = getline(command,&len,stdin);
		memset(command,'\0',MAX_CMD_LEN);
		//scanf("%s",&command);
		gets(command);
		//len = strlen(command);
		//command[len+1]='\0';
		//printf("you typed %s %d\n",command,len);
		
		if ( strncmp(command,"quit", 4) ==0)
		{
			if(send(sfd,command,MAX_CMD_LEN,0) < 0)
				errexit("can't send data: %s\n",strerror(errno));
			while(!rflag) //wait for recv thread to complete
				continue;
			if(close(sfd) <0)
				errexit("",NULL) ;
				
			set_output_default_color();
			exit(1);
		}
			
		if(send(sfd,command,MAX_CMD_LEN,0) < 0)
			errexit("can't send data: %s\n",strerror(errno));
		
		fcntl(0,F_GETFL,origstdinFlags);
		fcntl(0,F_SETFL,O_NONBLOCK);
		
		while(!cflag)
		{
			char ch;

			if(read(0,&ch,1)==1)
				send(sfd,&ch,1,0);
		}
		fcntl(0,F_SETFL,origstdinFlags);
	}//end of while(1)

	if(close(sfd)==-1)  //close the socket file descriptor
		errexit("can't close socket: #%d",sfd); 
	return 0;

}//end of telnetSession()

void recvfun(void)
{
	
	int numbytes;
	char buf[MAX_BUF_LEN];
	int out=0;
	int i=0;
	char last='0';
	
	while(!rflag){	
	do{
		out=0;
		//Inititalize the buffers to \0
		memset(buf,'\0',MAX_BUF_LEN);
		//printf("\nin recv loop\n");
		//wait for the message to recv from server
		if((numbytes = recv(sfd, buf,MAX_BUF_LEN-1,0)) == -1) 
			errexit("can't RECV data: %s\n",strerror(errno));
		//if clienet's choice is "quit", set rflag, break and  exxit
//		printf("client recieved numbytes %d\n",numbytes);
		cflag = 0;
		if (strcmp(command,"quit") == 0) 
		{
			rflag=1;
			break;
		}
		for(i=0;i<(numbytes-1);i++){
			switch(buf[i]){
				case '$':
					if(i==0 && last=='$'){
					//	i++;
						continue;
					}
					
					i++;
					
					if(buf[i]=='$'){
						printf("%c",buf[i]);
					}
					else if(buf[i]=='@'){
						out=1;
					}
					break;
				case '@':
					if(i==0 && last=='$') out=1;
					break;
				default :
					printf("%c",buf[i]);
			}
		}
		if(!out) {printf("%c",buf[numbytes-1]);last = buf[numbytes-1];}
	fflush(stdout);
	//buf[numbytes]='\0';
	//printf("numbytes: %d\n",numbytes);
	//	set_output_normal_color();	
	//	printf("%s", buf);
	//getchar();
	//printf("%s\n", buf);
	}while(!out);//(buf[numbytes-1]!='$');
	fflush(stdout);
	cflag=1;
	}
}
