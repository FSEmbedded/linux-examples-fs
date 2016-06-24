//*****************************************************************************/
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
/*** Author:   Daniel Kuhne                                                ***/
/*** Created:  16.09.2011                                                  ***/
/*** Modified: 24.06.2016 (PJ)                                             ***/
/***                                                                       ***/
/*** Description                                                           ***/
/*** -----------                                                           ***/
/*** Use spidev driver (/dev/spidev0.0) to read and write status register. ***/
/*** Application is based on SPI testing utility (using spidev driver)     ***/
/*** located within linux kernel documentation tree. For the SPI test you  ***/
/*** have to connect the MISO and MOSI pins from the same device. After    ***/
/*** that you can run the program. If everything is correct you will see   ***/
/*** that the READING STATUS have the same hex values like the tx array in ***/
/*** in the transfer function. If you see 0xff as READING STATUS then you  ***/
/*** maybe connected the wrong pins or something you changed in            ***/
/*** the program is wrong                                                  ***/
/***                                                                       ***/
/*** Compile with:                                                         ***/
/***              arm-linux-gcc -o spidev spidev.c                         ***/
/***                                                                       ***/
/*** Modification History:                                                 ***/
/*** 24.06.2016 PJ: change c-file name, works now for all boards because   ***/
/***                the user have to deliver the device. Improve headers   ***/
/*****************************************************************************/
/*** THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY ***/
/*** KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE   ***/
/*** IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR ***/
/*** PURPOSE.                                                              ***/
/*****************************************************************************/

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))


static const char *device = "/dev/spidev0.0";
static uint8_t mode = SPI_MODE_2;
static uint8_t bits = 8;
static uint32_t speed = 2000000;
static uint16_t delay = 0;


/*****************************************************************************
*** Function:    void pabort(const char *s)                                ***
***                                                                        ***
*** Parameters:  s: error message	                                   ***
***                                                                        ***
*** Return:      -                                                         ***
***                                                                        ***
*** Description                                                            ***
*** -----------                                                            ***
*** Shows the error message                                                ***
*****************************************************************************/
static void pabort(const char *s)
{
	perror(s);
	abort();
}


/*****************************************************************************
*** Function:    void transfer(int fd)                                     ***
***                                                                        ***
*** Parameters:  fd: filedescriptor                                        ***
***                                                                        ***
*** Return:      -                                                         ***
***                                                                        ***
*** Description                                                            ***
*** -----------                                                            ***
*** Write and read function of the spi device                              ***
*****************************************************************************/
static void transfer(int fd)
{
	int ret;
	uint8_t tx[] = {0x03, 0x55,0x40, 0x95, 0xBE};
	uint8_t rx[ARRAY_SIZE(tx)] = {0, };

	sleep(1);
	printf("READING STATUS\n");
	struct spi_ioc_transfer tr1 = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = ARRAY_SIZE(tx),
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr1);
	if (ret < 1)
		pabort("can't send spi message");

	for(ret = 0; ret < ARRAY_SIZE(tx); ret++)
	{
		printf("0x%.2x ", rx[ret]);
	}
	printf("\n");
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
	       "Usage: %s <spidev>\n"
	       "\n"
	       "  spidev: path to the spi device (/dev/spidevx.x)\n"
	       "\n"
	       "For the spi test you have to connect the miso and mosi pins "
	       "of one device. After that you can start the test.\n"
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
	int ret = 0;
	int fd;

	/* test command line arguments */
	if (argc < 1) {
		usage(argv[0]);
		return 1;
	}
	/* If an argument is given, it is the spi device */
	device = argv[1];

	fd = open(device, O_RDWR);
	if (fd < 0)
		pabort("->can't open device");

	/* spi mode */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	/* bits per word */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/* max speed hz */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	printf("spi mode: %d\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);

	transfer(fd);

	close(fd);

	return ret;
}
