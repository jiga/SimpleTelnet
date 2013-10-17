/* passsock.c - passivesock */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/errno.h>

#include <netinet/in.h>
#include <arpa/inet.h>
                                                                                              
#include <netdb.h>
                                                                                              
#ifndef INADDR_NONE
#define INADDR_NONE     0xffffffff
#endif  /* INADDR_NONE */


int s;

int	errexit(const char *, ...);

unsigned short	portbase = 0;		/* port base, for test servers		*/

/*------------------------------------------------------------------------
 * passivesock - allocate & bind a server socket using TCP or UDP
 *------------------------------------------------------------------------
 */

int
passivesock(const char *service, const char *transport, int qlen)
/*
 * Arguments:
 *      service   - service associated with the desired port
 *      transport - transport protocol to use ("tcp" or "udp")
 *      qlen      - maximum server request queue length
 */
{
        struct servent  *pse;   /* pointer to service information entry */
        struct protoent *ppe;   /* pointer to protocol information entry*/
        struct sockaddr_in sin; /* an Internet endpoint address         */
        int type;        /* socket descriptor and socket type    */
        int yes;                                                                                                                                     
        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = INADDR_ANY;
                                                                                                                                             
    /* Map service name to port number */
        if ( pse = getservbyname(service, transport) )
                sin.sin_port = htons(ntohs((unsigned short)pse->s_port)
                        + portbase);
        else if ((sin.sin_port=htons((unsigned short)atoi(service))) == 0)
                errexit("can't get \"%s\" service entry\n", service);
                                                                                                                                             
    /* Map protocol name to protocol number */
        if ( (ppe = getprotobyname(transport)) == 0)
                errexit("can't get \"%s\" protocol entry\n", transport);
                                                                                                                                             
    /* Use protocol to choose a socket type */
        if (strcmp(transport, "udp") == 0)
                type = SOCK_DGRAM;
        else
                type = SOCK_STREAM;
                                                                                                                                             
    /* Allocate a socket */
        s = socket(PF_INET, type, ppe->p_proto);
        if (s < 0)
                errexit("can't create socket: %s\n", strerror(errno));
        
	/* Allow address reuse */	
	yes = 1;
	if(setsockopt(s,SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
		perror("socket");
		exit(1);
	}
    /* Bind the socket */
        if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
                errexit("can't bind to %s port: %s\n", service,
                        strerror(errno));

   /* Listen on that Particular socket */
        if (type == SOCK_STREAM && listen(s, qlen) < 0)
                errexit("can't listen on %s port: %s\n", service,
                        strerror(errno));
        return s;
}
