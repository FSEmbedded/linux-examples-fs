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
/***                           GPIO EXAMPLE                      	   ***/
/***                                                                       ***/
/***                                                                       ***/
/*****************************************************************************/
/*** File:     gpio.c                                                      ***/
/*** Author:   C. Canbaz                                                   ***/
/*** Created:  30.10.2015                                                  ***/
/*** Modified: 24.06.2016 PJ                                               ***/
/***                                                                       ***/
/*** Description                                                           ***/
/*** -----------                                                           ***/
/*** Show how the GPIO ports are used on F&S Boards.			   ***/
/***                                                                       ***/
/*** Compile with:                                                         ***/
/***              arm-linux-gcc -o gpio gpio.c                             ***/
/***                                                                       ***/
/*** Modification History:                                                 ***/
/*** 24.06.2016 PJ: Improve Code now the gpio can work for all boards      ***/
/*****************************************************************************/
/*** THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY ***/
/*** KIND,  EITHER EXPRESSED OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE ***/
/*** IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR ***/
/*** PURPOSE.                                                              ***/
/*****************************************************************************/

#include <stdlib.h>		/* strtoul() */
#include <stdio.h>		/* perror() */
#include <unistd.h>		/* write(), close(), fsync(), sleep() */
#include <fcntl.h>		/* open(), O_WRONLY */
#include <string.h>		/* strlen() */


/* File paths */
#define SYS_EXP "/sys/class/gpio/export"
#define SYS_ADD "/sys/class/gpio/gpio??"
#define SYS_DIR "/direction"
#define SYS_VAL "/value"


/*****************************************************************************
*** Function:    int getFileDeskriptor(const char *file_path,              ***
***                              const int MODE)                           ***
***                                                                        ***
*** Parameters:  file_path: Path from the file, which want to modifying.   ***
***              MODE:      MODE for open the filedescriptor with          ***
***                         read or write permissions                      ***
***                                                                        ***
*** Return:      fd:        Filedescriptor                                 ***
***                                                                        ***
*** Description                                                            ***
*** -----------                                                            ***
*** This function open the filedeskritpor and returns it.                  ***
*****************************************************************************/
int getFileDeskriptor(const char *file_path, const int MODE)
{
	int fd = open(file_path, MODE);

	if(fd<0)
	{
		perror(file_path);
		return 1;
	}	
	return fd;
}


/*****************************************************************************
*** Function:    int createGPIO(const char *pinNr)                         ***
***                                                                        ***
*** Parameters:  pinNr: Number of GPIO pin                                 ***
***                                                                        ***
*** Return:      -                                                         ***
***                                                                        ***
*** Description                                                            ***
*** -----------                                                            ***
*** Creates the GPIO files in /sys/class/gpio.                             ***
*****************************************************************************/
int createGPIO(const char *pinNr)
{
	// Open Filedescriptor
	int fd = getFileDeskriptor(SYS_EXP, O_WRONLY);

	// Create GPIO Files
	printf("Use GPIO%s\n",pinNr);
	write(fd,pinNr,sizeof(pinNr));
	fsync(fd);
	close(fd);
	return 0;
}


/*****************************************************************************
*** Function:    int set Direction(const char *pinNr, const char *outin)   ***
***                                                                        ***
*** Parameters:  pinNr: Number of GPIO pin                                 ***
***              outin: To be set direction as output or input             ***
***                                                                        ***
*** Return:      -                                                         ***
***                                                                        ***
*** Description                                                            ***
*** -----------                                                            ***
*** This function sets the direction of the GPIO port as input or output.  ***
*****************************************************************************/
void setDirection(const char *pinNr,const char *outin)
{	
	char file_path[] = SYS_ADD SYS_DIR;

	file_path[strlen(SYS_ADD) - 2] = *pinNr;
	file_path[strlen(SYS_ADD) - 1] = *(pinNr+1);
	int fd=getFileDeskriptor(file_path,O_WRONLY);

	//Configuration Direction Output/Input
	printf("GPIO%s als %sput konfiguriert\n",pinNr,outin);	
	write(fd,outin,sizeof(outin));		
	fsync(fd);			
	close(fd);			 
}


/*****************************************************************************
*** Function:    int setValue(const char *pinNr, const char *value)        ***
***                                                                        ***
*** Parameters:  pinNr: Number of GPIO pin                                 ***
***              value: To be set the value for the GPIO port as output    ***
***                                                                        ***
*** Return:      -                                                         ***
***                                                                        ***
*** Description                                                            ***
*** -----------                                                            ***
*** This function sets the value of the GPIO port, which used as output.   ***
*****************************************************************************/
void setValue(const char *pinNr, const char *value)
{
	char file_path[] = SYS_ADD SYS_VAL;

	file_path[strlen(SYS_ADD) - 2] = *pinNr;
	file_path[strlen(SYS_ADD) - 1] = *(pinNr+1);

	int fd=getFileDeskriptor(file_path,O_WRONLY);

	printf("GPIO%s Wert auf %s\n", pinNr, value);
	write(fd,value,sizeof(value));
	fsync(fd);
	sleep(1);
	close(fd);
}


/*****************************************************************************
*** Function:    int getValue(char *pinNr)                                 ***
***                                                                        ***
*** Parameters:  pinNr:        Number of GPIO pin                          ***
***                                                                        ***
*** Return:      return_value: Input value of GPIO                         ***
***                                                                        ***
*** Description                                                            ***
*** -----------                                                            ***
*** This function gives the input value, which was read.                   ***
*****************************************************************************/
int getValue(const char *pinNr)
{
	/* create the correct path */
	char file_path[] = SYS_ADD SYS_VAL;

	file_path[strlen(SYS_ADD) - 2] = *pinNr;
	file_path[strlen(SYS_ADD) - 1] = *(pinNr+1);

	/* give filedescriptor */	
	int fd=getFileDeskriptor(file_path, O_RDONLY);

	/* read value */
	char value[2];
	read (fd,&value,sizeof(value));
	close (fd);
	int return_value;
	return_value = (int)value[0];
	return return_value;
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
	       "Usage: %s <direction> <gpio_nr> [count] [delay]\n"
	       "\n"
	       "  directon: set gpio direction: \"input\" or \"output\"\n"
	       "  count:    set the number of loop iterations (default: 10)\n"
	       "  delay:    set the delay in seconds between the set values"
						"of the gpio (default: 1)\n"
	       "\n"
	       "If you want to use delay option you have to use count option "
	       "too. So you cant use delay without using count\n"
	       "\n", progname);
}


/*****************************************************************************
*** Function:    void test_output()                                        ***
***                                                                        ***
*** Parameters:  gpio: GPIO Value as String                                ***
***		 count: number of loop interations			   ***
***		 delay: delay in seconds between set values		   ***
***                                                                        ***
*** Return:      -                                                         ***
***                                                                        ***
*** Description                                                            ***
*** -----------                                                            ***
*** This function tests GPIO ports as output                               ***
*****************************************************************************/
void test_output(const char *gpio, int count, int delay)
{
	int i = 0;

	createGPIO(gpio);
	setDirection(gpio, "out");

	do
	{
		setValue(gpio, "1");
		sleep(delay);
		setValue(gpio, "0");
		sleep(delay);
		i++;
	} while(count != i);
}


/*****************************************************************************
*** Function:    void test_input()                                         ***
***                                                                        ***
*** Parameters:  gpio: GPIO Value as String                                ***
***		 count: number of loop interations			   ***
***		 delay: delay in seconds between get value		   ***
***                                                                        ***
*** Return:      -                                                         ***
***                                                                        ***
*** Description                                                            ***
*** -----------                                                            ***
*** This function tests GPIO ports as input.                               ***
*****************************************************************************/
void test_input(const char *gpio, int count, int delay)
{
	int i = 0;
	int gpio_stored = 49;
	int gpio_val = NULL;

	createGPIO(gpio);
	setDirection(gpio, "in");
	printf("Input value %d\n", (getValue(gpio) - 48));

	do
	{
		sleep(delay);
		gpio_val = getValue(gpio);
		if(gpio_val != gpio_stored) {
			gpio_stored = gpio_val;
			printf("Input value changed %d\n",
						(getValue(gpio) - 48));
		}
		i++;
	} while(count != i);
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
	/* initialize values and set default parameters */
	const char *gpio = NULL;
	unsigned int count = 10;
	unsigned int delay = 1;

	/* test command line arguments */
	if ((argc < 3) || (argc > 5)) {
		usage(argv[0]);
		return 1;
	}
	gpio =argv[2];
	if(gpio == NULL) {
		usage(argv[0]);
		return 1;
	}

	if(argc > 3)
		count = strtoul(argv[3], NULL, 0);
		if(count == 0)
			printf("Please note you have an endless loop. Abort"
			" with strg-c\n");

	if(argc > 4)
		delay = strtoul(argv[4], NULL, 0);

	if (strcmp((argv[1]),"input")==0)
		test_input(gpio, count, delay);
	else if (strcmp((argv[1]),"output")==0)
		test_output(gpio, count, delay);
	else {
		usage(argv[0]);
		return 1;
	}

	return 0;
}
