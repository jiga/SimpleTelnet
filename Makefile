# telnet
all:	td cl

td :	telnetd.c passsock.c passTCP.c setTerminalColor.c errexit.c
	gcc -lpthread passTCP.c passsock.c setTerminalColor.c errexit.c telnetd.c -o td

cl :	client.c connectTCP.c connectsock.c errexit.c setTerminalColor.c 
	gcc -lpthread connectsock.c connectTCP.c errexit.c setTerminalColor.c client.c -o cl

clean:
	rm -f cl td

