/************************************************
*
* File Name: telnetd.c
* Author: Jignesh Patel, Praveen Lella
* Date: Copyright 00,00,2006
*
* Title: C/S Programming   (Linux connection-oriented Server, using Apparent Concurrency)
* Course: CS590ac
* Course Name: Advanced Computing Systems
*
*
* This project is about the use of interactive connection oriented protocol(TELNET) with
* Concurrent connections. It shows how a TCP TELNET SERVER program provides service to the 
* Client
* in a UNIX/LINUX Environment. This Server program basically shows the connection-oriented
* tcp functionality with Apparent Concurrency  to support concurrent connections in LINUX.
*
* This is a Server program which can be started by giving the Specified PORT
* for the service to run and listens to the Client connections after a succesful start.
*
* This Program sends some categories and receives requests from the clients. Based on the
* Client request, it sends the fortunes to the clients randomly.
*
* The actual work in this program is done in the routines "TCPfortune" and "passivesock".
*
* The main program takes one argument, port number. As it is a TCP connection, the routine * then looks for bit sequence
* according to the protocol defined. In this case * "1 XXXX".
* And depending on the client wish the server will either sends a fortune quote or bad
* category closes the connection.
*
********************************************************************************************************************/
                                                                                                                                             
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <linux/unistd.h>
#include <pthread.h>

#define __NR_sys_telnetChild	253
_syscall3(int,sys_telnetChild,char*,cmd,int,csd,int,ssd);

//Macros
#define PORT 12345
#define QLEN 10

//Global variables
int pid, status;
int ssd;

//Global variables --imported
extern int errno;

//Functions --External
void    errexit(const char *, ...);
int     passiveTCP(const char *, int);
int set_output_error_color();
int set_output_exit_color();
int set_output_default_color();

//Function Declarations -- those defined in this file
void* telnetSession(void *);
int telnetChild(char *, int);


void handleSigPipe(int s)
{
	// do nothing!
}

void sig_handler(int s) {
	while(waitpid(-1,NULL,WNOHANG) > 0);
}

/************************************************************************
* main - Concurrent TCP server for Telnet Service
**********************************************************************/

int main(int argc, char  *argv[]) 
{
	int csd; /* client & server socket descriptors */
	char    *service="12345";       /* service name or port number --default value  */
        struct  sockaddr_in cli_addr;    /* the address of a client   */
        unsigned int    size;      /* length of client's address   */
	//struct sockaddr_in serv_addr;
	struct sigaction sa;
	pthread_t *threads;


	switch (argc) //switch case to properly check the command line arguments
        {
        	case 1:
			break;	//to be removed after
		case 2:
                	service = argv[1]; //assigning the service value given by user.
                	break;
        	default:
			set_output_error_color();
                	printf("usage: td <port>\n");
			set_output_default_color();
			exit(1);
        }
                                                                                              
        printf("%s", "Server is beginning...\n"); //call this routine to create socket.
	
	ssd = passiveTCP(service, QLEN); //create slave socket descriptor
	
	/*
	sa.sa_handler = sig_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if(sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}*/ //have to dig through

	while(1) 
	{
		size = sizeof(cli_addr);
		printf("Waiting for the CLIENT connections...\n");
		if((csd=accept(ssd,(struct sockaddr*)&cli_addr,&size))==-1)
		{
			perror("accept");
			continue;
		}
		printf("server: Connection recvd from %s\n",inet_ntoa(cli_addr.sin_addr));
	        threads=(pthread_t *)malloc(sizeof(pthread_t));
		
		if(pthread_create(threads,NULL,telnetSession,(void*)&csd)!=0)
			perror("pthread_create"); //start the Telnet session	
		//telnetSession(csd,ssd); //start the Telnet session	
		//if(close(csd) <0) //close the Client socket descriptor
		//	perror("close:"); 

	}// end of while(1)
	return 0;
}//end of MAIN()

/*------------------------------------------------------
 telnetSession() -- Child process to handle telnet Clients
---------------------------------------------------------*/


void* telnetSession(void* cd)
{
	int len;
	int csd;
//	int *pcsd;
        char cmd[100];
	
//	pcsd = (int *) cd;
//	csd = *pcsd;
	csd = *((int*) cd);	
	while(1)
	{
		memset(cmd,'\0',100);
                if((len = recv(csd, cmd, 100, MSG_WAITALL)) == -1) {
	           	perror("can't RECV data:");
			break;
		   	//errexit("can't RECV data: %s\n",strerror(errno)); //beware of this ---wrong code
		}
                cmd[len]='\0';
		printf("cmd = %s\n",cmd);	
		if(strncmp(cmd,"quit",4) == 0)
		{
			printf("telnet session ended\n");
			fflush(stdout);
			break;	
		}
		signal(SIGPIPE,&handleSigPipe);
		// creates child process
		pid = fork();
        
		switch(pid)
        	{
			case -1: //(pid < 0)
				printf("Error in the fork process.\n");
                       		break;
                	case 0:
                        	// Child code
				errno = 0;
                        	if(sys_telnetChild(cmd,csd,ssd)<=0)
				{	
					perror("CHILD1: sys_telnetChild");
					printf("\nCHILD1: systelnet returned %d",errno);
					fflush(stdout);
				}
				//telnetChild(cmd, csd, ssd);
				
                	default:
                        	// Parent waits
                        	waitpid(pid,&status,0);
                        	break;
		}// end of switch()
		if(send(csd,"$@",2,0) < 0){ //send the end of "COMMAND RESULT" notification, delimited by '$'
			perror("can't send data:");	
			break;
		}
		//	errexit("can't send data: %s\n",strerror(errno));
		else fprintf(stdout, "sent '$' sign\n");
		fflush(stdout); //flush the OUTPUT buffer
	}///end of while(1) 

	if(close(csd) <0) //close the Client socket descriptor
		perror("close:");
       pthread_exit(NULL);	
} // end of telnetSession()



int telnetChild(char *cmd, int csd)
{
	printf("\nwithout sys_telnetChild\n");
	close(ssd); // need to think about it
        close(0);
        close(1);
        close(2);
        if(dup(csd) !=0 || dup(csd)!=1 || dup(csd)!=2)
        {
                perror("dup error");
                exit(1);
        }

	if (execl("/bin/sh","/bin/sh","-c",cmd,NULL) <0)
			errexit("can't exec:");
}// end of telnetChild

                                                                                                      

