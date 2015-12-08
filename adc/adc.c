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
/***                  ADC CONVERSION EXAMPLE (VYBRID)                      ***/
/***                                                                       ***/
/***                                                                       ***/
/*****************************************************************************/
/*** File:     adc.c                                                       ***/
/*** Author:   C. Canbaz, H. Keller, F&S Elektronik Systeme GmbH           ***/
/*** Created:  30.10.2015                                                  ***/
/*** Modified: 11.11.2015 16:49:13 (HK)                                    ***/
/***                                                                       ***/
/*** Description                                                           ***/
/*** -----------                                                           ***/
/*** Show how the ADC ports are used in Linux on Vybrid architecture.      ***/
/***                                                                       ***/
/*** Compile with:                                                         ***/
/***              arm-linux-gcc -o adc adc.c                               ***/
/***                                                                       ***/
/*****************************************************************************/
/*** THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY ***/
/*** KIND,  EITHER EXPRESSED OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE ***/
/*** IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR ***/
/*** PURPOSE.                                                              ***/
/*****************************************************************************/

#include <stdio.h>			/* perror() */
#include <stdlib.h>			/* strtol() */
#include <string.h>			/* strcat() */
#include <fcntl.h>			/* open(), O_RDWR */
#include <unistd.h>			/* write(), close(), sleep() */
#include <sys/ioctl.h>			/* ioctl(), _IO(), _IOWR() */
#include "mvf_adc.h"			/* struct adc_feature, ... */

/* Default values */
#define DEFAULT_CHANNEL 0
#define DEFAULT_SAMPLES 1
#define DEFAULT_DELAY 1

char device_path[] = "/dev/mvf-adc.?";

/* ADC specific information */
struct adc_feature testfeature = {
	.channel = ADC8,
	.clk_sel = ADCIOC_ADACK_SET,
	.clk_div_num = CLK_DIV2,
	.res_mode = BIT12,
};


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
	       "Usage: %s device [channel [samples [delay]]]\n"
	       "\n"
	       "  device:  ADC device to use (one of /dev/mvf_adc.?)\n"
	       "  channel: ADC channel to use (default: %u)\n"
	       "  samples: number of samples to convert (default: %u)\n"
	       "  delay:   delay between samples (in seconds, default: %u)\n"
	       "\n",
	       progname, DEFAULT_CHANNEL, DEFAULT_SAMPLES, DEFAULT_DELAY);
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
*** use ADC.                                                               ***
*****************************************************************************/
int main(int argc, char *argv[])
{
	int fd;
	unsigned int i;
	char *device = device_path;
	unsigned int channel = DEFAULT_CHANNEL;
	unsigned int samples = DEFAULT_SAMPLES;
	unsigned int delay = DEFAULT_DELAY;

	/* Get command line arguments */
	if ((argc < 2) || (argc > 5)) {
		usage(argv[0]);
		return 1;
	}
	if ((argv[1][0] >= '0') && (argv[1][0] <= '9') && !argv[1][1])
		device_path[strlen(device_path) - 1] = argv[1][0];
	else
		device = argv[1];
	if (argc > 2)
		channel = strtoul(argv[2], NULL, 0);
	if (argc > 3)
		samples = strtoul(argv[3], NULL, 0);
	if (argc > 4)
		delay = strtoul(argv[4], NULL, 0);

	printf("Using device '%s', channel %u, %u sample(s), delay %us\n",
	       device, channel, samples, delay);
	fd = open(device, O_RDWR);
	if (fd < 0) {
		perror("Can not open device");
		return 1;
	}

	testfeature.channel = (enum adc_channel)channel;
	if (ioctl(fd, ADC_INIT, &testfeature) == -1) {
		perror("Can not init ADC");
		return 1;
	}

	if (ioctl(fd, ADC_CONFIGURATION, &testfeature) == -1) {
		perror("Can not configure ADC");
		return 1;
	}

	for (i = 1; i <= samples; i++) {
		sleep(delay);

		if (ioctl(fd, ADC_REG_CLIENT, &testfeature) == -1) {
			perror("Can not register client");
			return 1;
		}

		if (ioctl(fd, ADC_CONVERT, &testfeature) == -1) {
			perror("can not convert ADC value");
			return 1;
		}

		printf("Sample %d: %d\n", i, testfeature.result0);
	}

	close(fd);

	return 0;
}
