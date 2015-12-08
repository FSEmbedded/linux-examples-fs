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
/***                  P W M   C O N F I G U R A T I O N                    ***/
/***                                                                       ***/
/***                                                                       ***/
/*****************************************************************************/
/*** File:     pwm.c                                                       ***/
/*** Author:   H. Keller, F&S Elektronik Systeme GmbH                      ***/
/*** Created:  22.10.2015                                                  ***/
/*** Modified: 11.11.2015 16:48:35 (HK)                                    ***/
/***                                                                       ***/
/*** Description                                                           ***/
/*** -----------                                                           ***/
/*** Show how PWM ports are used in Linux.                                 ***/
/***                                                                       ***/
/*** Compile with                                                          ***/
/***            arm-linux-gcc -o pwm pwm.c                                 ***/
/***                                                                       ***/
/*****************************************************************************/
/*** THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY ***/
/*** KIND,  EITHER EXPRESSED OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE ***/
/*** IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR ***/
/*** PURPOSE.                                                              ***/
/*****************************************************************************/

#include <stdio.h>			/* fopen(), fprintf, fscanf(), ... */
#include <stdlib.h>			/* strtoul() */
#include <dirent.h>			/* opendir(), closedir(), DIR */
#include <limits.h>			/* PATH_MAX */
#include <errno.h>			/* errno, ENOENT */

/* Define some values to have it easier if one of them changes in the future */
#define PWM_PATH	"/sys/class/pwm/pwmchip"
#define NPWM		"npwm"
#define EXPORT		"export"
#define DUTY_CYCLE	"duty_cycle"
#define PERIOD		"period"
#define ENABLE		"enable"

char path_pwmchip[PATH_MAX];
char path_channel[PATH_MAX];
char path[PATH_MAX];


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
int show_error(const char *reason, const char *bad_path)
{
	perror(reason);
	if (bad_path)
		fprintf(stderr, "Bad path: %s\n", bad_path);

	return 1;
}


/*****************************************************************************
*** Function:    int write_sysfs_number(char *dir, char *filename,         ***
***                                     unsigned long value)               ***
***                                                                        ***
*** Parameters:  dir:      Pointer to directory part of path               ***
***              filename: Pointer to filename part of path                ***
***              value:    Value to write to sysfs file                    ***
***                                                                        ***
*** Return:      0: Success; 1: Failure                                    ***
***                                                                        ***
*** Description                                                            ***
*** -----------                                                            ***
*** Write the provided value to the sysfs file given by directory and      ***
*** filename.                                                              ***
*****************************************************************************/
int write_sysfs_number(char *dir, char *filename, unsigned long value)
{
	FILE *f;

	/* Open the file */
	sprintf(path, "%s/%s", dir, filename);
	f = fopen(path, "w");
	if (!f)
		return 1;

	/* Write data */
	if (!fprintf(f, "%lu", value) < 0)
		return 1;

	/* Close file */
	if (fclose(f) == EOF)
		return 1;

	return 0;
}


/*****************************************************************************
*** Function:    int read_sysfs_number(char *dir, char *filename, unsigned ***
***                                  long *value)                          ***
***                                                                        ***
*** Parameters:  dir:      Pointer to directory part of path               ***
***              filename: Pointer to filename part of path                ***
***              value:    Pointer to variable where read value will be    ***
***                        stored                                          ***
*** Return:      0: Success, 1: Failure                                    ***
***                                                                        ***
*** Description                                                            ***
*** -----------                                                            ***
*** Read a decimal value from the sysfs file given by dir and filename.    ***
*****************************************************************************/
int read_sysfs_number(char *dir, char *filename, unsigned long *value)
{
	FILE *f;

	/* Open the file */
	sprintf(path, "%s/%s", dir, filename);
	f = fopen(path, "r");
	if (!f)
		return 1;

	/* Read data */
	*value = 0;
	if (fscanf(f, "%lu", value) == EOF)
		return 1;

	/* Close file */
	fclose(f);

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
void usage(const char *progname)
{
	printf("\n"
	       "Usage: %s pwmchip [channel [duty_cycle [period]]]\n"
	       "\n"
	       "  pwmchip:    PWM device to use (see " PWM_PATH "*)\n"
	       "  channel:    PWM channel to use on this chip (default: 0)\n"
	       "  duty_cycle: Duty cycle for this channel (in nanoseconds)\n"
	       "  period:     Period (in nanoseconds, keep if not given)\n"
	       "\n"
	       "If neither duty_cycle nor period are given, only show the"
	       " current values.\n"
	       "If period is zero, the PWM is disabled, otherwise enabled.\n"
	       "\n", progname);
}


/*****************************************************************************
*** Function:   int main(int argc, char *argv)                             ***
***                                                                        ***
*** Parameters: argc: Number of command line arguments                     ***
***             argv: Pointer to command line arguments                    ***
***                                                                        ***
*** Return:     Program return code                                        ***
***                                                                        ***
*** Description                                                            ***
*** -----------                                                            ***
*** Parse the command line options and configure PWM or show settings.     ***
*****************************************************************************/
int main(int argc, const char *argv[])
{
	unsigned int pwmchip;
	unsigned long channel = 0;
	unsigned long duty_cycle;
	unsigned long period;
	unsigned long npwm;
	unsigned long enable = 0;
	DIR *dir_pwmchip;
	DIR *dir_channel;

	if ((argc < 2) || (argc > 5)) {
		usage(argv[0]);
		return 1;
	}

	/* Get pwmchip number from command line and check if available */
	pwmchip = strtoul(argv[1], NULL, 0);
	sprintf(path_pwmchip, "%s%u", PWM_PATH, pwmchip);
	dir_pwmchip = opendir(path_pwmchip);
	if (!dir_pwmchip)
		return show_error("Can not access pwmchip", path_pwmchip);

	/* Get channel number and check if valid by reading npwm file */
	if (argc > 2)
		channel = strtoul(argv[2], NULL, 0);
	if (read_sysfs_number(path_pwmchip, NPWM, &npwm))
		return show_error("Can not read channel count", path);

	if (channel >= npwm) {
		fprintf(stderr, "Bad channel number: pwmchip%u has only ",
			pwmchip);
		if (npwm > 1)
			fprintf(stderr, "channels 0..%lu\n", npwm - 1);
		else
			fprintf(stderr, "channel 0\n");

		return 1;
	}

	/* Make sure that the channel is exported and available as directory */
	sprintf(path_channel, "%s/pwm%lu", path_pwmchip, channel);
	dir_channel = opendir(path_channel);
	if (!dir_channel) {
		if (errno != ENOENT)
			return show_error("Can not access pwm channel",
					  path_channel);

		if (write_sysfs_number(path_pwmchip, EXPORT, channel))
			return show_error("Can not export pwm channel", path);

		dir_channel = opendir(path_channel);
		if (!dir_channel)
			return show_error("Can not access exported pwm channel",
					  path_channel);
	}

	/* Set period first if given, enable or disable pwm */
	if (argc > 4) {
		period = strtoul(argv[4], NULL, 0);
		if (period > 0) {
			if (write_sysfs_number(path_channel, PERIOD, period))
				return show_error("Can not set " PERIOD, path);
			enable = 1;
		}
		if (write_sysfs_number(path_channel, ENABLE, enable))
			return show_error("Can not enable/disable pwm", path);
	}

	/* Set duty_cycle if given */
	if (argc > 3) {
		duty_cycle = strtoul(argv[3], NULL, 0);
		if (write_sysfs_number(path_channel, DUTY_CYCLE, duty_cycle))
			return show_error("Can not set " DUTY_CYCLE, path);
	}

	/* Read back duty_cycle, period and enable state */
	if (read_sysfs_number(path_channel, DUTY_CYCLE, &duty_cycle))
		return show_error("Can not read " DUTY_CYCLE, path);
	if (read_sysfs_number(path_channel, PERIOD, &period))
		return show_error("Can not read " PERIOD, path);
	if (read_sysfs_number(path_channel, ENABLE, &enable))
		return show_error("Can not read " ENABLE, path);

	/* Show info about current settings */
	printf("pwmchip%u/pwm%lu: duty_cycle=%lu period=%lu enable=%lu\n",
	       pwmchip, channel, duty_cycle, period, enable);

	closedir(dir_channel);
	closedir(dir_pwmchip);

	return 0;
}
