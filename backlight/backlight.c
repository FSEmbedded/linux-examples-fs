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
/***           B a c k l i g h t   B r i g h t n e s s   T e s t           ***/
/***                                                                       ***/
/***                                                                       ***/
/*****************************************************************************/
/*** File:     backlight.c                                                 ***/
/*** Author:   Daniel Kuhne                                                ***/
/*** Created:  19.10.2009                                                  ***/
/*** Modified: 25.10.2010 16:04:51 (HK)                                    ***/
/***                                                                       ***/
/*** Description:                                                          ***/
/*** Show how to set the backlight brightness under linux via              ***/
/*** /sys/class/backlight/backlight_xxx.                                   ***/
/***                                                                       ***/
/*** Compile with                                                          ***/
/***            arm-linux-gcc -o backlight backlight.c                     ***/
/***                                                                       ***/
/*** Modification History:                                                 ***/
/*** 25.10.2010 HK: Improved header comments. Removed unused variable i.   ***/
/*** 24.06.2016 PJ: Improve Header and optimize code.                      ***/
/*****************************************************************************/
/*** THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY ***/
/*** KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE   ***/
/*** IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR ***/
/*** PURPOSE.                                                              ***/
/*****************************************************************************/
#include <stdio.h>			/* fprintf */
#include <stdlib.h>			/* strtoul() */
#include <unistd.h>
#include <fcntl.h>			/* open, read, write */
#include <limits.h>			/* PATH_MAX */


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
void usage(const char *progname)
{
	printf("\n"
	       "Usage: %s <backlight_device> [value]\n"
	       "\n"
	       "  backlight_device: Path to the backlight which you will test\n"
	       "  value: brightness level. Value can be between 0-15\n"
	       "\n", progname);
}


/*****************************************************************************
*** Function:    int set_backlight(char *brightness, char *max_brightness, ***
***              unsigned int value)                                       ***
***                                                                        ***
*** Parameters:  brightness:     Path to the brightness parameter          ***
***		 max_brightness: Path to the max brightness parameter      ***
***              value:          brightness level                          ***
***                                                                        ***
*** Return:      0: Success, 1: Failure                                    ***
***                                                                        ***
*** Description                                                            ***
*** -----------                                                            ***
*** write the new backlight brightness                                     ***
*****************************************************************************/
int set_backlight(char *brightness, char *max_brightness, unsigned int value)
{
	int fd = NULL;
	int max_brightness_val = NULL;
	char buf[4];

	fd = open(max_brightness, O_RDONLY);
	if(fd == -1) {
		fprintf(stderr, "cant open device %s\n", brightness);
		return 1;
	}

	read(fd, buf, sizeof(buf));
	max_brightness_val = atoi(buf);
	close(fd);
	if(value > max_brightness_val || value < 0) {
		fprintf(stderr, "wrong value parameter %d\n", value);
		return 1;
	}

	fd = open(brightness, O_WRONLY);
	if (fd == -1) {
		fprintf(stderr, "cant open device %s\n", brightness);
		return 1;
	}

	sprintf(buf, "%d", value);
	write(fd, buf, 3);
	fsync(fd);
	printf("set brightness to %d\n", value);
	close(fd);

	return 0;
}


/*****************************************************************************
*** Function:    int main(int argc, char *argv[])                          ***
***                                                                        ***
*** Parameters:  argc: Number of command line arguments                    ***
***              argv: Pointer to command line arguments                   ***
***                                                                        ***
*** Return:      Program return code                                       ***
***                                                                        ***
*** Description                                                            ***
*** -----------                                                            ***
*** Parse the command line options and call the necessary functions to     ***
*** use backlight                                                          ***
*****************************************************************************/
int main(int argc, const char *argv[])
{
	char brightness[PATH_MAX];
	char max_brightness[PATH_MAX];
	unsigned int value = 5;
	int ret = 0;

	/* test command line arguments */
	if (argc < 2) {
		usage(argv[0]);
		return 1;
	}

	if(argc > 2)
		value = strtoul(argv[2], NULL, 0);

	sprintf(brightness, "%sbrightness", argv[1]);
	sprintf(max_brightness, "%smax_brightness", argv[1]);
	ret = set_backlight(brightness, max_brightness, value);
	if(ret) {
		usage(argv[0]);
		return 1;
	}

	return 0;
}
