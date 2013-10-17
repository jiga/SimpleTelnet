/* errexit.c - errexit */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/*------------------------------------------------------------------------
 * errexit - print an error message and exit
 *------------------------------------------------------------------------
 */

//External variables --linked at link time
extern int s; //socket file descriptor imported from connectsock.c file

//External functions referred
int set_output_error_color(void); 
int set_output_exit_color(void);
int set_output_default_color(void); 


int errexit(const char *format, ...)
{
        va_list args;
                                                                                                          
        va_start(args, format);
        set_output_error_color(); //Sets the Color of output(BG:FG:blinking) text lines of the TERMINAL.
        vfprintf(stdout, format, args);
        va_end(args);
        //int set_output_default_color(void);  //Resets the color of the TERMINAL to default
	set_output_exit_color();
	printf("Closing socket file descriptor #%d\n",s);
        if(close(s) ==-1)  //close the socket file descriptor
        	perror("can't close socket:");
        printf("Exiting from the Program....");
        set_output_default_color();  //Resets the color of the TERMINAL to default
        printf("\n");
        exit(1);
}
