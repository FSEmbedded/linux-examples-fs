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
/***                           GPIO EXAMPLE                                ***/
/***                                                                       ***/
/***                                                                       ***/
/*****************************************************************************/
/*** File:     gpio.c                                                      ***/
/*** Author:   C. Canbaz, P. Jakob                                         ***/
/*** Created:  30.10.2015                                                  ***/
/*** Modified: 08.08.2016 17:53:00 (HK)                                    ***/
/***                                                                       ***/
/*** Description                                                           ***/
/*** -----------                                                           ***/
/*** Show how the GPIO ports are used on F&S Boards.                       ***/
/***                                                                       ***/
/*** Compile with:                                                         ***/
/***              arm-linux-gcc -o gpio gpio.c                             ***/
/***                                                                       ***/
/*** Modification History:                                                 ***/
/*** 24.06.2016 PJ: Improve Code now the gpio can work for all boards.     ***/
/*** 08.08.2016 HK: Simplify code, use streams, command line now has gpio  ***/
/***                first, then in/out/low/high, then count, then delay.   ***/
/*****************************************************************************/
/*** THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY ***/
/*** KIND,  EITHER EXPRESSED OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE ***/
/*** IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR ***/
/*** PURPOSE.                                                              ***/
/*****************************************************************************/

#include <stdlib.h>			/* strtoul() */
#include <stdio.h>			/* fprintf(), sprintf(), perror() */
#include <unistd.h>			/* sleep() */
#include <dirent.h>			/* DIR, opendir() */
#include <string.h>			/* strcmp() */
#include <limits.h>			/* PATH_MAX, MAX_INT */
#include <errno.h>			/* errno, ENOENT */

#define GPIO_PATH	"/sys/class/gpio"
#define EXPORT_PATH	"/sys/class/gpio/export"

static char path_gpio[PATH_MAX];
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
void usage(const char *progname)
{
	printf("\n"
	       "Usage: %s gpio [dir [count [delay]]]\n"
	       "\n"
	       "  gpio:  GPIO index (see GPIO reference card)\n"
	       "  dir:   Direction, one of 'in', 'out', 'high', 'low' (default 'in')\n"
	       "  count: Number of loop iterations (default 10)\n"
	       "  delay: Delay in seconds between loop iterations (default 1)\n"
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
*** Parse the command line options and handle GPIO.                        ***
*****************************************************************************/
int main(int argc, const char *argv[])
{
	unsigned int gpio;
	unsigned int count = 10;
	unsigned int delay = 1;
	int direction_in;
	int i;
	int value;
	const char *direction = "in";
	DIR *dir_gpio;
	FILE *file;

	/* Parse command line arguments */
	if ((argc < 2) || (argc > 5)) {
		usage(argv[0]);
		return 1;
	}
	gpio = strtoul(argv[1], NULL, 0);
	if (argc > 2)
		direction = argv[2];
	if (argc > 3)
		count = strtoul(argv[3], NULL, 0);
	if (argc > 4)
		delay = strtoul(argv[4], NULL, 0);
	direction_in = (strcmp(direction, "in") == 0);
	if (!count)
		count = INT_MAX;

	/* If gpio is not exported yet, export it now. Open gpio directory. */
	sprintf(path_gpio, "%s/gpio%d", GPIO_PATH, gpio);
	dir_gpio = opendir(path_gpio);
	if (!dir_gpio) {
		if (errno != ENOENT)
			return show_error("Can not access gpio", path_gpio);

		file = fopen(EXPORT_PATH, "w");
		if (!file)
			return show_error("Can not open export", EXPORT_PATH);
		if ((fprintf(file, "%d", gpio) < 0) || (fclose(file) == EOF))
			return show_error("Can not export gpio", EXPORT_PATH);

		dir_gpio = opendir(path_gpio);
		if (!dir_gpio)
			return show_error("Can not access gpio", path_gpio);
	}

	/* Set direction */
	sprintf(path, "%s/direction", path_gpio);
	file = fopen(path, "w");
	if (!file)
		return show_error("Can not access direction", path);
	if ((fprintf(file, "%s", direction) < 0) || (fclose(file) == EOF))
		return show_error("Can not set direction", path);

	/* Open value stream in unbuffered mode */
	sprintf(path, "%s/value", path_gpio);
	file = fopen(path, direction_in ? "r" : "w");
	if (!file)
		return show_error("Can not open value", path);
	if (setvbuf(file, NULL, _IONBF, 0) != 0)
		return show_error("Can not set file to unbuffered mode", path);

	/* Handle loop */
	i = 0;
	do {
		if (i)
			sleep(delay);
		i++;
		if (direction_in) {
			value = -1;
			fseek(file, 0, SEEK_SET);
			fscanf(file, "%d", &value);
			printf("Loop %i: Value is %d\n", i, value);
		} else {
			printf("Loop %i: Setting value 1\n", i);
			fprintf(file, "1");
			sleep(delay);
			printf("Loop %i: Setting value 0\n", i);
			fprintf(file, "0");
		}
	} while (i < count);

	fclose(file);
	closedir(dir_gpio);

	return 0;
}
