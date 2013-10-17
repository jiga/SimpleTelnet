/* Filename: setTerminalColor.c  --Sets the colors of the messages output  on the TERMINAL according to the context */
                                                                                                          
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
                                                                                                          
/*------------------------------------------------------------------------
 * set_output_normal_color - Sets the Color of normal output messages to decent blue
 *------------------------------------------------------------------------
 */

int set_output_normal_color(void)
{
        printf("\033[2;34;5m"); //Sets the Color of output(BG:FG:blinking) text lines of the TERMINAL.
}


/*------------------------------------------------------------------------
 * set_output_error_color - Sets the Color of error messages to decent red
 *------------------------------------------------------------------------
 */

int set_output_error_color(void)
{
        printf("\033[2;31;5m"); //Sets the Color of output(BG:FG:blinking) error text lines on the TERMINAL.
}


/*------------------------------------------------------------------------
 * set_output_exit_color - Sets the Color of exit messages to decent faded ASH COLOR on the TERMINAL
 *------------------------------------------------------------------------
 */

int set_output_exit_color(void)
{
        printf("\033[0;37;5m"); //Sets the Color of output(BG:FG:blinking) text lines on the TERMINAL.
}


/*------------------------------------------------------------------------
 * set_output_default_color - Sets the Color of error messages to decent red
 *------------------------------------------------------------------------
 */

int set_output_default_color(void)
{
        printf("\033[0m"); //Sets the Color of output(BG:FG:blinking) text lines to default color
}

