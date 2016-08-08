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
/*** Authors:  C. Canbaz, P. Jakob, H. Keller, F&S Elektronik Systeme GmbH ***/
/*** Created:  30.10.2015                                                  ***/
/*** Modified: 08.08.2016 17:58:26 (HK)                                    ***/
/***                                                                       ***/
/*** Description                                                           ***/
/*** -----------                                                           ***/
/*** Show how LED brightness or trigger can be set.                        ***/
/***                                                                       ***/
/*** Compile with:                                                         ***/
/***              arm-linux-gcc -o leds leds.c                             ***/
/***                                                                       ***/
/*** Modification History:                                                 ***/
/*** 30.06.2016 PJ: Improve methods and comments.                          ***/
/*** 08.08.2016 HK: Only use two arguments: led and value. A number is set ***/
/***                as brightness, other values as trigger. Improve error  ***/
/***                handling and comments.                                 ***/
/*****************************************************************************/
/*** THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY ***/
/*** KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE   ***/
/*** IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR ***/
/*** PURPOSE.                                                              ***/
/*****************************************************************************/

#include <stdio.h>			/* printf(), sprintf(), fprintf() */
#include <dirent.h>			/* DIR, opendir() */
#include <limits.h>			/* PATH_MAX */

#define LED_PATH	"/sys/class/leds"

static char path_led[PATH_MAX];
static char path_value[PATH_MAX];


/*****************************************************************************
*** Function:    int show_error(char *reason, char *bad_path)              ***
***                                                                        ***
*** Parameters:  reason:   Pointer to string with error reason             ***
***              bad_path: Optional pointer to path (added as second       ***
***                        output line if not NULL)                        ***
***                                                                        ***
*** Return:      1: Failure; value is meant as final program status        ***
***                                                                        ***
*** Description                                                            ***
*** -----------                                                            ***
*** Print error reason, actual error (from errno) and (if not NULL) the    ***
*** given path. This function always returns 1, which is meant as program  ***
*** status at progam end.                                                  ***
*****************************************************************************/
static int show_error(const char *reason, const char *bad_path)
{
	perror(reason);
	if (bad_path)
		fprintf(stderr, "Bad path: %s\n", bad_path);

	return 1;
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
static void usage (const char *progname)
{
	printf(":\n"
	       "Usage: %s led value\n"
	       "\n"
	       "  led:   one of the leds in /sys/class/leds/\n"
	       "  value: A number is set as led brightness, a trigger name is\n"
	       "         set as led trigger. See sys/class/leds/<led>/trigger\n"
	       "         for possible values. Common trigger names are:\n"
	       "         'none', 'heartbeat', 'default-on', 'nand-disk'\n"
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
*** Parse the command line options and set led value.                      ***
*****************************************************************************/
int main (int argc,const char *argv[])
{
	DIR *dir_led;
	FILE *value;

	/* Get command line arguments */
	if (argc != 3) {
		usage(argv[0]);
		return 1;
	}

	/* Determine LED directory and value path: brightness or trigger */
	sprintf(path_led, "%s/%s", LED_PATH, argv[1]);
	if ((argv[2][0] >= '0') && (argv[2][0] <= '9'))
		sprintf(path_value, "%s/%s", path_led, "brightness");
	else
		sprintf(path_value, "%s/%s", path_led, "trigger");
	printf("Setting %s to %s\n", path_value, argv[2]);

	/* Actually set value */
	dir_led = opendir(path_led);
	if (!dir_led)
		return show_error("Can not access led directory", path_led);
	value = fopen(path_value, "w");
	if (!value)
		return show_error("Can not access led value", path_value);
	if ((fprintf(value, "%s", argv[2]) < 0) || (fclose(value) == EOF))
		return show_error("Can not set value", path_value);
	closedir(dir_led);

	return 0;
}
