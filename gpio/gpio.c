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
/***                  ADC CONVERSION EXAMPLE (VYBRID)                      ***/
/***                                                                       ***/
/***                                                                       ***/
/*****************************************************************************/
/*** File:     gpio.c                                                      ***/
/*** Author:   C. Canbaz                                                   ***/
/*** Created:  30.10.2015                                                  ***/
/*** Modified: 25.11.2015                                                  ***/
/***                                                                       ***/
/*** Description                                                           ***/
/*** -----------                                                           ***/
/*** Show how the GPIO ports are used in Linux on Vybrid architecture.     ***/
/***                                                                       ***/
/*** Compile with:                                                         ***/
/***              arm-linux-gcc -o gpio gpio.c                               ***/
/***                                                                       ***/
/*****************************************************************************/
/*** THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY ***/
/*** KIND,  EITHER EXPRESSED OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE ***/
/*** IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR ***/
/*** PURPOSE.                                                              ***/
/*****************************************************************************/

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
	int fd=getFileDeskriptor(SYS_EXP,O_WRONLY);
	
	// Create GPIO Files
	printf("Use GPIO%s\n",pinNr);
	write(fd,pinNr,sizeof(pinNr));
	fsync(fd);
	close(fd);
	return 0;
}

	
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
	int fd = open (file_path, MODE);
	if(fd<0)
	{
		perror(file_path);
		return 1;
	}	
	return fd;
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
int getValue(char *pinNr)
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
	       "Usage: %s OPTION\n"
		"\n"
		"OPTION:\n"
		"  test_output\n"
		"  test_input\n", progname);
}


/*****************************************************************************
*** Function:    void test_output()                                        ***
***                                                                        ***
*** Parameters:  -                                                         ***
***                                                                        ***
*** Return:      -                                                         ***
***                                                                        ***
*** Description                                                            ***
*** -----------                                                            ***
*** This function tests GPIO ports as output                               ***
*****************************************************************************/
void test_output()
{
	int i;
	createGPIO("81");
	createGPIO("82");
	createGPIO("83");
	createGPIO("84");
	createGPIO("85");
	createGPIO("65");
	createGPIO("64");
	createGPIO("63");
	setDirection("81","out");
	setDirection("82","out");
	setDirection("83","out");
	setDirection("84","out");
	setDirection("85","out");
	setDirection("65","out");
	setDirection("64","out");
	setDirection("63","out");

	for(i=0;i<9;i++)
	{
		setValue("81", "1");
		setValue("82", "1");
		setValue("83", "1");
		setValue("84", "1");
		setValue("85", "1");
		setValue("65", "1");
		setValue("64", "1");
		setValue("63", "1");

		sleep(1);

		setValue("81", "0");
		setValue("82", "0");
		setValue("83", "0");
		setValue("84", "0");
		setValue("85", "0");
		setValue("65", "0");
		setValue("64", "0");
		setValue("63", "0");

		sleep(1);
	}
}


/*****************************************************************************
*** Function:    void test_input()                                         ***
***                                                                        ***
*** Parameters:  -                                                         ***
***                                                                        ***
*** Return:      -                                                         ***
***                                                                        ***
*** Description                                                            ***
*** -----------                                                            ***
*** This function tests GPIO ports as input.                               ***
*****************************************************************************/
void test_input()
{
	createGPIO("81");
	createGPIO("82");
	createGPIO("83");
	createGPIO("84");
	createGPIO("85");
	createGPIO("65");
	createGPIO("64");
	createGPIO("63");
	setDirection("81","in");
	setDirection("82","in");
	setDirection("83","in");
	setDirection("84","in");
	setDirection("85","in");
	setDirection("65","in");
	setDirection("64","in");
	setDirection("63","in");

	int GPIO81_stored=49;
	int GPIO82_stored=49;
	int GPIO83_stored=49;
	int GPIO84_stored=49;
	int GPIO85_stored=49;
	int GPIO65_stored=49;
	int GPIO64_stored=49;
	int GPIO63_stored=49;
	
	int GPIO81;
	int GPIO82;
	int GPIO83;
	int GPIO84;
	int GPIO85;
	int GPIO65;
	int GPIO64;
	int GPIO63;
	do
	{
		sleep(1);

		GPIO81=getValue("81");
		GPIO82=getValue("82");
		GPIO83=getValue("83");
		GPIO84=getValue("84");
		GPIO85=getValue("85");
		GPIO65=getValue("65");
		GPIO64=getValue("64");
		GPIO63=getValue("63");

		if (GPIO81!=GPIO81_stored)
		{
			GPIO81_stored=GPIO81;
			printf ("Input value changed GPIO81 %d\n",
				(getValue("81")-48));
		} 
		if (GPIO82!=GPIO82_stored)
		{
			GPIO82_stored=GPIO82;
			printf ("Input value changed GPIO82 %d\n",
				(getValue("82")-48));
		} 
		if (GPIO83!=GPIO83_stored)
		{
			GPIO83_stored=GPIO83;
			printf ("Input value changed GPIO83 %d\n",
				(getValue("83")-48));
		} 
		if (GPIO84!=GPIO84_stored)
		{
			GPIO84_stored=GPIO84;
			printf ("Input value changed GPIO84 %d\n",
				(getValue("84")-48));
		} 
		if (GPIO85!=GPIO85_stored)
		{
			GPIO85_stored=GPIO85;
			printf ("Input value changed GPIO85 %d\n",
				(getValue("85")-48));
		} 
		if (GPIO65!=GPIO65_stored)
		{
			GPIO65_stored=GPIO65;
			printf ("Input value changed GPIO65 %d\n",
				(getValue("65")-48));
		} 
		if (GPIO64!=GPIO64_stored)
		{
			GPIO64_stored=GPIO64;
			printf ("Input value changed GPIO64 %d\n",
				(getValue("64")-48));
		} 
		if (GPIO63!=GPIO63_stored)
		{
			GPIO63_stored=GPIO63;
			printf ("Input value changed GPIO63 %d\n",
				(getValue("63")-48));
		} 
	
	}while(1);	
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
*** use GPIO                                                               ***
*****************************************************************************/
int main(int argc, const char *argv[])
{
	/* test command line arguments */
	if (argc != 2){
		usage(argv[0]);
		return 1;
	}
		
	if (strcmp ((argv[1]),"test_input")==0)
		test_input();
	
	else if (strcmp ((argv[1]),"test_output")==0)
		test_output();
		
	else{  
		usage(argv[0]);
		return 1;
	}
					
	return 0;
}
