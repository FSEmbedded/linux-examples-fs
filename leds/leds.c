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
/***         V Y B R I D    A R M S T O N E - A 5                          ***/
/***                                                                       ***/
/***                                                                       ***/
/*****************************************************************************/
/*** File:     leds.c                                                      ***/
/*** Author:   C. Canbaz, F&S Elektronik Systeme GmbH                 	   ***/
/*** Created:  30.10.2015                                              	   ***/
/*** Modified: -    	                                                   ***/
/***                                                                       ***/
/*** Description                                                           ***/
/*** -----------                                                           ***/
/*** Show how could be use ADC ports on Vybrid-Armstone-A5/Linux.          ***/
/*** Compile with                                                          ***/
/***              arm-linux-gcc -o leds leds.c                             ***/
/***                                                                       ***/
/*****************************************************************************/
/*** THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY ***/
/*** KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE   ***/
/*** IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR ***/
/*** PURPOSE.                                                         	   ***/
/*****************************************************************************/
/* Fehler */
/* /sys/class/leds/Status2/trigger beim ersten mal auf none keine veränderung
 * zu seheh. in der trigger datei kann man die änderung auf none sehen, jedoch
 * leuchtet die led wie als ob es auf default-on wäre.
 * wenn man als erstes auf einen anderen trigger aus none wählt, wird bei 
 * nächsten wechsel none problemlos ausgeführt.
 * */



#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


char device_path[] = "/sys/class/leds/Status?";


/*****************************************************************************
*** Function:    int setModification(const char *status,                   ***
***              const char *modification, const char *selection)          ***
***                                                                        ***
*** Parameters:  status:       which led modifying?                        ***
***              modification: trigger or brightness?                      ***
***              selection:    select an option for modification           ***
***                                                                        ***
*** Return:      -                                                         ***
***                                                                        ***
*** Description                                                            ***
*** -----------                                                            ***
*** This function set the options for the chosen led.                      ***
*****************************************************************************/
void setModification(const char *status, const char *modification,
		 const char *selection)
{
	int fd = getFileDeskriptor(status, modification);
	write(fd, selection, strlen(selection));
	fsync(fd);
	close(fd);	
}


/*****************************************************************************
*** Function:    int getFileDeskriptor(const char *sys, const int mode)    ***
***                                                                        ***
*** Parameters:  sys:  Path from required file                             ***
***              mode: Open file to be read or write or both?              ***
***                                                                        ***
*** Return:      fd = filedescriptor                                       ***
***                                                                        ***
*** Description                                                            ***
*** -----------                                                            ***
*** This function open the filedescriptor and returns it.                  ***
*****************************************************************************/
int getFileDeskriptor(const char *status, const char *modification)
{
	char sys_path[100] = {"/sys/class/leds/Status"};
	strcat(sys_path, status);
	strcat(sys_path, "/");
	strcat(sys_path, modification);
	printf("sys_path %s\n", sys_path);
	
	int fd = open(sys_path,O_WRONLY);
	if (fd < 0)
	{
		perror("File could not be opened\n");
		return 1;
	}
	return fd;
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
void usage (const char *progname)
{
	printf(":\n"
	       "Usage: %s status modification selection\n"
	       "\n"
	       "  status:       choose a led (one of /sys/class/leds/Status?)\n"
	       "  modification: select trigger or brightness to change it\n"
	       "  selection:\n"
			"\tfor trigger:"
			" none nand-disk mmc0 timer heartbeat default-on\n"
			"\tfor brightness:"
			" 0-max.brightness(255)\n"
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
*** Parse the command line options and call the necessary functions to     ***
*** use leds.                                                              ***
*****************************************************************************/
int main (int argc,const char *argv[])
{

	/* Get command line arguments */
	if ((argc < 2) || (argc > 4)){
		usage(argv[0]);
		return 1;
	}
	if ((argv[1][0] > '0') && (argv[1][0] < '3') && !argv[1][1]){
		device_path[strlen(device_path) - 1] = argv[1][0];
		setModification(argv[1],argv[2],argv[3]);
	}
	else 
		usage(argv[0]);

	return 0;
}
