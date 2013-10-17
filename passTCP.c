/* passTCP.c - passiveTCP */

//Functions Declared ---those externally defined
int	passivesock(const char *, const char *, int);

/*------------------------------------------------------------------------
 * passiveTCP - create a passive socket for use in a TCP server
 *------------------------------------------------------------------------
 */
int
passiveTCP(const char *service, int qlen)
{
	return passivesock(service, "tcp", qlen);
}
