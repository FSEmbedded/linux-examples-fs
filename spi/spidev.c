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
/***                        S P I D E V   T e s t                          ***/
/***                                                                       ***/
/***                                                                       ***/
/*****************************************************************************/
/*** File:     spidev.c                                                    ***/
/*** Authors:  Daniel Kuhne, Patrick Jakob, Hartmut Keller                 ***/
/*** Created:  16.09.2011                                                  ***/
/*** Modified: 08.08.2016 11:30:36 (HK)                                    ***/
/***                                                                       ***/
/*** Description                                                           ***/
/*** -----------                                                           ***/
/*** Use the spidev driver to transfer some test data.  No real SPI device ***/
/*** needs to be connected, the program will read back the data that it is ***/
/*** writing at the same time. To make this work you have to connect (loop ***/
/*** back) the MOSI to the MISO line.                                      ***/
/***                                                                       ***/
/*** A successful test run will show the same values for sent and received ***/
/*** data.  If the received data only shows 0xFF values, then the pins are ***/
/*** not connected correctly.                                              ***/
/***                                                                       ***/
/*** Compile with:                                                         ***/
/***              arm-linux-gcc -o spidev spidev.c                         ***/
/***                                                                       ***/
/*** Modification History:                                                 ***/
/*** 24.06.2016 PJ: Change C file name, works now for all boards because   ***/
/***                the user has to provide the device. Improve comments.  ***/
/*** 08.08.2016 HK: Allow setting SPI mode and speed_hz, show sent and re- ***/
/***                reived data, compare it. Simplify code. Improve error  ***/
/***                handling and comments.                                 ***/
/*****************************************************************************/
/*** THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY ***/
/*** KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE   ***/
/*** IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR ***/
/*** PURPOSE.                                                              ***/
/*****************************************************************************/

#include <stdint.h>			/* uint8_t, uint16_t, ... */
#include <stdlib.h>			/* strtoul() */
#include <stdio.h>			/* printf(), perror() */
#include <unistd.h>			/* sleep(), close() */
#include <fcntl.h>			/* open(), O_RDWR */
#include <sys/ioctl.h>			/* ioctl(), ... */
#include <linux/spi/spidev.h>		/* struct spi_ioc_transfer, ... */

#define TEST_LEN 5

static uint8_t mode = SPI_MODE_2;
static uint8_t bits_per_word = 8;
static uint16_t delay_usecs = 0;
static uint32_t speed_hz = 2000000;
static const uint8_t tx[TEST_LEN] = {0x03, 0x55, 0x40, 0x95, 0xBE};
static uint8_t rx[TEST_LEN];


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
	       "Usage: %s device [mode [speed_hz]]\n"
	       "\n"
	       "  device:   path to the spi device (/dev/spidevx.x)\n"
	       "  mode:     SPI mode (0..3, default 2)\n"
	       "  speed_hz: Transfer speed (default 2000000 Hz)\n"
	       "\n"
	       "For the SPI test you have to connect the MISO and MOSI pins\n"
	       "of one device to loop back the sent data. After that you can\n"
	       "start the test.\n"
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
*** Parse the command line options and call the necessary functions to     ***
*** use GPIO                                                               ***
*****************************************************************************/
int main(int argc, const char *argv[])
{
	int i;
	int fd;
	struct spi_ioc_transfer transfer;

	/* Parse command line arguments */
	if ((argc < 2) || (argc > 4)) {
		usage(argv[0]);
		return 1;
	}
	if (argc > 2)
		mode = strtoul(argv[2], NULL, 0) & 3;
	if (argc > 3)
		speed_hz = strtoul(argv[3], NULL, 0);

	fd = open(argv[1], O_RDWR);
	if (fd < 0)
		return show_error("Can not open device", argv[1]);

	/* Set SPI mode */
	if (ioctl(fd, SPI_IOC_WR_MODE, &mode) == -1)
		return show_error("Can not set SPI mode", NULL);
	if (ioctl(fd, SPI_IOC_RD_MODE, &mode) == -1)
		return show_error("Can not get SPI mode", NULL);

	/* Set bits per word */
	if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word) == -1)
		return show_error("Can not set bits per word", NULL);
	if (ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits_per_word) == -1)
		return show_error("Can not get bits per word", NULL);

	/* Set maximum transfer speed */
	if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed_hz) == -1)
		return show_error("Can not set max speed hz", NULL);
	if (ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed_hz) == -1)
		return show_error("Can not get max speed hz", NULL);

	/* Show settings and data to send */
	printf("SPI mode:      %d\n", mode);
	printf("Bits per word: %d\n", bits_per_word);
	printf("Max. speed:    %d Hz\n", speed_hz);
	printf("Sent data:    ");
	for (i = 0; i < TEST_LEN; i++)
		printf(" 0x%02X", tx[i]);
	printf("\n");

	sleep(1);

	/* Actually do the transfer */
	transfer.tx_buf = (unsigned long)tx;
	transfer.rx_buf = (unsigned long)rx;
	transfer.len = TEST_LEN;
	transfer.delay_usecs = delay_usecs;
	transfer.speed_hz = speed_hz;
	transfer.bits_per_word = bits_per_word;
	if (ioctl(fd, SPI_IOC_MESSAGE(1), &transfer) == -1)
		return show_error("Can not send SPI message", NULL);

	close(fd);

	/* Show received data and compare data with sent data */
	printf("Received data:");
	for (i = 0; i < TEST_LEN; i++)
		printf(" 0x%02X", rx[i]);
	printf("\n");
	for (i = 0; i < TEST_LEN; i++) {
		if (rx[i] != tx[i])
			break;
	}
	printf("-> Sent and received data %s.\n",
	       (i < TEST_LEN) ? "differs" : "is identical");

	return 0;
}
