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
/*** Authors:  D. Kuhne, P. Jakob, H. Keller  F&S Elektronik Systeme GmbH  ***/
/*** Created:  19.10.2009                                                  ***/
/*** Modified: 08.08.2016 18:49:04 (HK)                                    ***/
/***                                                                       ***/
/*** Description                                                           ***/
/*** -----------                                                           ***/
/*** Show how to set the backlight brightness under linux.                 ***/
/***                                                                       ***/
/*** Compile with                                                          ***/
/***            arm-linux-gcc -o backlight backlight.c                     ***/
/***                                                                       ***/
/*** Modification History:                                                 ***/
/*** 25.10.2010 HK: Improve header comments. Remove unused variable i.     ***/
/*** 24.06.2016 PJ: Improve Header and optimize code.                      ***/
/*** 08.08.2016 HK: If only backlight device is given, show current value. ***/
/***                Simplify code, improve error handling and comments.    ***/
/*****************************************************************************/
/*** THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY ***/
/*** KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE   ***/
/*** IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR ***/
/*** PURPOSE.                                                              ***/
/*****************************************************************************/

#include <stdio.h>			/* printf(), fprintf(), ... */
#include <dirent.h>			/* DIR, opendir() */
#include <stdlib.h>			/* strtoul() */
#include <limits.h>			/* PATH_MAX */

#define BACKLIGHT_PATH	"/sys/class/backlight"

static char path_backlight[PATH_MAX];
static char path[PATH_MAX];


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
static void usage(const char *progname)
{
	printf("\n"
	       "Usage: %s device [value]\n"
	       "\n"
	       "  device: Backlight device (in /sys/class/backlight)\n"
	       "  value:  New brightness level; if not given, show current brightness\n"
	       "\n", progname);
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
*** Parse the command line options and access backlight.                   ***
*****************************************************************************/
int main(int argc, const char *argv[])
{
	int value;
	DIR *dir_backlight;
	FILE *file;

	/* Parse command line arguments */
	if ((argc < 2) || (argc > 3)) {
		usage(argv[0]);
		return 1;
	}

	/* Open backlight directory */
	sprintf(path_backlight, "%s/%s", BACKLIGHT_PATH, argv[1]);
	sprintf(path, "%s/brightness", path_backlight);
	dir_backlight = opendir(path_backlight);
	if (!dir_backlight)
		return show_error("Can not access backlight directory",
				  path_backlight);

	/* If requested, set new value */
	if (argc > 2) {
		value = strtoul(argv[2], NULL, 0);
		printf("Set new brightness %d\n", value);
		file = fopen(path, "w");
		if (!file)
			return show_error("Can not access brightness", path);
		if ((fprintf(file, "%d", value) < 0) || (fclose(file) == EOF))
			return show_error("Can not set brigthness", path);
	}

	/* Read current value */
	file = fopen(path, "r");
	if (!file)
		return show_error("Can not open brightness", path);
	value = -1;
	fscanf(file, "%d", &value);
	printf("Backlight '%s' has brightness %d\n", argv[1], value);

	fclose(file);
	closedir(dir_backlight);

	return 0;
}
