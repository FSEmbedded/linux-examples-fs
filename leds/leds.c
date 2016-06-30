/*****************************************************************************/
/***     ______       _____    ______                           _          ***/
/***    |  ____|__   / ____|  |  ____|                         | |         ***/
/***    | |__ ( _ ) | (___    | |__  __  ____ _ _ __ ___  _ __ | | ___     ***/
/***    |  __|/ _ \/\\___ \   |  __| \ \/ / _` | '_ ` _ \| '_ \| |/ _ \    ***/
/***    | |  | (_>  <____) |  | |____ >  < (_| | | | | | | |_) | |  __/    ***/
/***    |_|   \___/\/_____/   |______/_/\_\__,_|_| |_| |_| .__/|_|\___|    ***/
/***                                                     |_|               ***/
/*****************************************************************************/
/***                                                                       ***/
/***                                                                       ***/
/***                    S t a t u s   L E D s   T e s t                    ***/
/***                                                                       ***/
/***                                                                       ***/
/*****************************************************************************/
/*** File:     leds.c                                                      ***/
/*** Author:   C. Canbaz, F&S Elektronik Systeme GmbH                 	   ***/
/*** Created:  30.10.2015                                              	   ***/
/*** Modified: 30.06.2016 PJ                                               ***/
/***                                                                       ***/
/*** Description                                                           ***/
/*** -----------                                                           ***/
/*** Show how could be use ADC ports on Vybrid-Armstone-A5/Linux.          ***/
/*** Compile with                                                          ***/
/***              arm-linux-gcc -o leds leds.c                             ***/
/***                                                                       ***/
/*** Modification History:                                                 ***/
/*** 30.06.2016 PJ: Improve methods. Improve comments                      ***/
/*****************************************************************************/
/*** THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY ***/
/*** KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE   ***/
/*** IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR ***/
/*** PURPOSE.                                                         	   ***/
/*****************************************************************************/
/* Fehler */
/* Wird die LED beim Bootvorgang auf on geschaltet kann man diese über den mode
 * "trigger" nicht auf none (aus) schalten bzw. wenn diese auf none geschalten
 * wird leuchtet die LED weiter. Man muss zuerst einen andere Selektierung
 * vornehmen anstatt "none" bsp. "default-on". Nachdem dies gesetzt wurde kann
 * mit "none" die LED ausgeschaltet werden.
 * */


#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>			/* PATH_MAX */


/*****************************************************************************
*** Function:    int setValue(char *path, char *value)                     ***
***                                                                        ***
*** Parameters:  path:  Path to the required file                          ***
***              value: value which will written to the file               ***
***                                                                        ***
*** Return:      0: Success, 1: Failure                                    ***
***                                                                        ***
*** Description                                                            ***
*** -----------                                                            ***
*** This function open, write and close the file.                          ***
*****************************************************************************/
int setValue(char *path, char *value)
{
	int fd = open(path,O_WRONLY);
	if (fd < 0)
	{
		fprintf(stderr, "cant open device %s\n", path);
		return 1;
	}

	write(fd, value, strlen(value));
	fsync(fd);
	close(fd);

	return 0;
}


/*****************************************************************************
*** Function:    void usage(const char *progname)                          ***
***                                                                        ***
*** Parameters:  progname: Name of the program                             ***
***                                                                        ***
*** Return:      -                                                         ***
***                                                                        ***
*** Description                                                            ***
*** -----------                                                            ***
*** Show the usage of the program.                                         ***
*****************************************************************************/
void usage (const char *progname)
{
	printf(":\n"
	       "Usage: %s status modification selection\n"
	       "\n"
	       "  status:       choose a led (one of /sys/class/leds/Status?/)\n"
	       "  modification: select trigger or brightness to change it\n"
	       "  selection:\n"
			"\tfor trigger:"
			" none nand-disk mmc0 timer heartbeat default-on\n"
			"\tfor brightness:"
			" 0 - max. brightness(255)\n"
	       "\n", progname);
}


/*****************************************************************************
*** Function:    int main(int argc, char *argv[])                          ***
***                                                                        ***
*** Parameters:  argc: Number of command line arguments                    ***
***              argv: Pointer to command line argmuents                   ***
***                                                                        ***
*** Return:      Program return code                                       ***
***                                                                        ***
*** Description                                                            ***
*** -----------                                                            ***
*** Parse the command line options and call the necessary functions to     ***
*** use leds.                                                              ***
*****************************************************************************/
int main (int argc,const char *argv[])
{
	char path[PATH_MAX];
	char value[PATH_MAX];
	int ret = 0;
	/* Get command line arguments */
	if (argc < 4) {
		usage(argv[0]);
		return 1;
	}
	sprintf(path, "%s%s", argv[1], argv[2]);
	sprintf(value, "%s", argv[3]);

	ret = setValue(path, value);

	return ret;
}
