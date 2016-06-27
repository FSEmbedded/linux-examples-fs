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
/***                  CAN   C O N F I G U R A T I O N                      ***/
/***                                                                       ***/
/***                                                                       ***/
/*****************************************************************************/
/*** File:     can.c                                                       ***/
/*** Author:   P. Jakob, F&S Elektronik Systeme GmbH                       ***/
/*** Created:  27.06.2016                                                  ***/
/***                                                                       ***/
/*** Description                                                           ***/
/*** -----------                                                           ***/
/*** Show how CAN ports are used in Linux.                                 ***/
/***                                                                       ***/
/*** Compile with                                                          ***/
/***            arm-linux-gcc -o can can.c                                 ***/
/***                                                                       ***/
/*****************************************************************************/
/*** THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY ***/
/*** KIND,  EITHER EXPRESSED OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE ***/
/*** IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR ***/
/*** PURPOSE.                                                              ***/
/*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <stdlib.h>
#include <time.h>

#define u_to_m	1000

int soc;


/*****************************************************************************
*** Function:    int open_port(const char *port)                           ***
***                                                                        ***
*** Parameters:  port: name of the can device                              ***
***                                                                        ***
*** Return:      0: Success; 1: Failure                                    ***
***                                                                        ***
*** Description                                                            ***
*** -----------                                                            ***
*** This function open and initailize a can socket.                        ***
*****************************************************************************/
int open_port(const char *port)
{
	struct ifreq ifr;
	struct sockaddr_can addr;

	/* open socket */
	soc = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if(soc < 0) {
		fprintf(stderr, "failed to open socket\n");
		return 1;
	}

	addr.can_family = AF_CAN;
	strcpy(ifr.ifr_name, port);

	if(ioctl(soc, SIOCGIFINDEX, &ifr) < 0) {
		fprintf(stderr, "failed to initialize socket\n");
		return 1;
	}

	addr.can_ifindex = ifr.ifr_ifindex;

	fcntl(soc, F_SETFL, O_NONBLOCK);

	if(bind(soc, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		fprintf(stderr, "failed to bind socket\n");
		return 1;
	}

	return 0;
}


/*****************************************************************************
*** Function:    int send_port(int send_start)                             ***
***                                                                        ***
*** Parameters:  send_start: flag to check how much frames to send (1 or   ***
***              endless loop)                                             ***
***                                                                        ***
*** Return:      0: Success; 1: Failure                                    ***
***                                                                        ***
*** Description                                                            ***
*** -----------                                                            ***
*** This function sends one or more can frames to the bus.                 ***
*****************************************************************************/
int send_port(int send_start)
{
	struct can_frame frame;
	int retval, i;

	srand(time(NULL));
	while(1) {
		frame.can_id = rand() % 1000;
		frame.can_dlc = rand() % 8;
		for(i = 0; i <= frame.can_dlc; i++) {
			frame.data[i] = rand() % 255;
		}
		retval = write(soc, &frame, sizeof(struct can_frame));
		if (retval != sizeof(struct can_frame)) {
			fprintf(stderr, "failed to write can frame\n");
			return 1;
		}
		if(send_start == 0)
			break;

		usleep(200 * u_to_m);
	}
	return 0;
}


/*****************************************************************************
*** Function:    void read_port(const char *can_port)                      ***
***                                                                        ***
*** Parameters:  can_port: name of the can device                          ***
***                                                                        ***
*** Return:      -                                                         ***
***                                                                        ***
*** Description                                                            ***
*** -----------                                                            ***
*** This function reads out the CAN Frame and print it out in a endless    ***
*** loop.                                                                  ***
*****************************************************************************/
void read_port(const char *can_port) {
	struct can_frame frame;
	int nbytes, i;

	printf("ID \t [DLC] \t data\n");
	while(1) {
		nbytes = read(soc, &frame, sizeof(struct can_frame));
		if(nbytes > 0) {
			printf("%s \t %03X \t [%d] \t", can_port, frame.can_id,
								frame.can_dlc);
			for(i=0; i<frame.can_dlc; i++)
				printf("%02X ",frame.data[i]);
			printf("\n");
		}
	}
}


/*****************************************************************************
*** Function:    void close_port()                                         ***
***                                                                        ***
*** Parameters:  -                                                         ***
***                                                                        ***
*** Return:      -                                                         ***
***                                                                        ***
*** Description                                                            ***
*** -----------                                                            ***
*** This function close the can socket.                                    ***
*****************************************************************************/
void close_port()
{
	close(soc);
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
	       "Usage: %s <mode> <can_port> [flag]\n"
	       "\n"
	       "  directon: set can mode: \"read\" or \"write\"\n"
	       "  can_nr:   can port that will be used (e.g. can0)\n"
	       "  flag:     \"SEND_ONCE\" for sending one frame, "
			    "\"SEND_START\" for sending several frames. "
			    "default is SEND_ONCE\n"
	       "\n"
	       "you only have the flag option if the mode is set to write. "
	       "You can break up the SEND_START or the read mode with strg-c\n"
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
*** use CAN                                                               ***
*****************************************************************************/
int main(int argc, const char *argv[])
{
	const char *can_port = NULL;
	int send_start = 0;
	int ret;

	if ((argc < 3) || (argc > 4)) {
		usage(argv[0]);
		return 1;
	}

	can_port = argv[2];
	if(can_port == NULL) {
		usage(argv[0]);
		return 1;
	}

	ret = open_port(can_port);
	if(ret)
		return 1;

	if (strcmp((argv[1]),"read")==0)
		read_port(can_port);
	else if (strcmp((argv[1]),"write")==0) {
		if((argc > 3) && (strcmp((argv[3]),"SEND_START") == 0))
			send_start = 1;
		ret = send_port(send_start);
		if(ret)
			return 1;
	}
	else {
		usage(argv[0]);
		return 1;
	}

	close_port();

	return 0;
}
