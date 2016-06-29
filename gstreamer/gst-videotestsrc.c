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
/***        G S t r e a m e r   S a m p l e   A p p l i c a t i o n        ***/
/***                                                                       ***/
/*****************************************************************************/
/*** File:     gst-videotestsrc.c                                          ***/
/*** Author:   Daniel Kuhne <kuhne@fs-net.de>                              ***/
/*** Created:  19.10.2009                                                  ***/
/*** Modified: 26.10.2010 11:46:47 (HK)                                    ***/
/***                                                                       ***/
/*** Description:                                                          ***/
/*** Connect the videotestsrc to the frame buffer sink. This requires that ***/
/*** the videotestsrc is configured in buildroot:                          ***/
/***                                                                       ***/
/***   Package Selection for the target                                    ***/
/***       Audio and video libraries and applications                      ***/
/***       -*- gst-plugins-base                                            ***/
/***           [*] videotestsrc                                            ***/
/***                                                                       ***/
/*** To compile, just edit the Makefile and change the line                ***/
/***                                                                       ***/
/***   BUILDROOT =                                                         ***/
/***                                                                       ***/
/*** to point to your version of the buildroot directory. Then type        ***/
/***                                                                       ***/
/***   make                                                                ***/
/***                                                                       ***/
/*** Then copy the file to your board and run it                           ***/
/***                                                                       ***/
/***   ./gst-videotestsrc [time]                                           ***/
/***                                                                       ***/
/*** The time is the optional parameter which playback the video for a     ***/
/*** specific time.                                                        ***/
/***                                                                       ***/
/*** The program runs the equivalent of the following command line:        ***/
/***                                                                       ***/
/***   gst-launch videotestsrc ! fbdevsink                                 ***/
/***                                                                       ***/
/*** Modification History:                                                 ***/
/*** 25.10.2010 HK: Improved header comments; use fbdevsink.               ***/
/*** 29.06.2016 PJ: Improved Comments, add usage, add time for playback    ***/
/***                video                                                  ***/
/*****************************************************************************/
/*** THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY ***/
/*** KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE   ***/
/*** IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR ***/
/*** PURPOSE.                                                              ***/
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>			/* atoi() */
#include <string.h>
#include <gst/gst.h>

#define BANNER "F&S Board GStreamer Test Application\n"
#define TAG "gst-videotstsrc: "

/*****************************************************************************
*** Function:    gboolean callback(gpointer data)                          ***
***                                                                        ***
*** Parameters:  data: Name of the gpointer                                ***
***                                                                        ***
*** Return:      -                                                         ***
***                                                                        ***
*** Description                                                            ***
*** -----------                                                            ***
*** If the interrupt occurs, then this fuction will be called and stops    ***
*** the video.                                                             ***
*****************************************************************************/
gboolean callback(gpointer data)
{
	g_main_loop_quit((GMainLoop*)data);
	return TRUE;
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
	       "Usage: %s [time]\n"
	       "\n"
	       "  time:   playback in seconds (default: 10)\n", progname);
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
*** Parse the command line options and configure the audiotestsrc.         ***
*****************************************************************************/
int main(int argc, const char *argv[])
{
	GMainLoop *loop = NULL;
	GstElement *pipeline = NULL;
	GstElement *videotestsrc = NULL;
        GstElement *sink = NULL;
	GMainContext *con1 = NULL;
	GSource *source1 = NULL;
	guint delay = 10;

	if (argc > 1) {
		delay = atoi(argv[1]);
		if(strcmp((argv[1]),"-h")==0 || strcmp((argv[1]),"--help")==0) {
			usage(argv[0]);
			return 1;
		}
	}

	gst_init(NULL, NULL);
	g_print("%s", BANNER);

	con1 = g_main_context_new();
	loop = g_main_loop_new(con1, FALSE);

	if(delay != 0) {
		source1 = g_timeout_source_new_seconds(delay);
		g_source_set_callback(source1, callback, loop, NULL);
		g_source_attach(source1, con1);
	}

	pipeline = gst_pipeline_new("testpipeline");
	videotestsrc = gst_element_factory_make("videotestsrc", "videotestsrc");
	sink = gst_element_factory_make("fbdevsink", "sink");
	//sink = gst_element_factory_make("ximagesink", "sink");
	g_print(TAG "loop         is %p\n", loop);
	g_print(TAG "pipeline     is %p\n", pipeline);
	g_print(TAG "videotestsrc is %p\n", videotestsrc);
	g_print(TAG "sink         is %p\n", sink);
	gst_bin_add_many(GST_BIN(pipeline), videotestsrc, sink, NULL);
	gst_element_link_many(videotestsrc, sink, NULL);
	gst_element_set_state(pipeline, GST_STATE_PLAYING);
	g_main_loop_run(loop);

	/* End of stream: clean up */
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
	g_main_loop_unref(loop);

	return 0;
}
