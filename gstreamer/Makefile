BUILDROOT = $(HOME)/git/Multi-Plattform/buildroot-f+s
CC = $(BUILDROOT)/output/host/usr/bin/arm-linux-gcc
LD = $(BUILDROOT)/output/host/usr/bin/arm-linux-ld
CFLAGS = -Wall -Os
INCLUDES += -I$(BUILDROOT)/output/staging/usr/include/gstreamer-0.10/ \
	-I$(BUILDROOT)/output/staging/usr/include/glib-2.0/ \
	-I$(BUILDROOT)/output/staging/usr/lib/glib-2.0/include/ \
	-I$(BUILDROOT)/output/staging/usr/include/libxml2/
LIBS +=	-L$(BUILDROOT)/output/staging/usr/lib -lgstreamer-0.10 \
	-lglib-2.0 \
	-lgobject-2.0

COMPILE = $(CC) $(CFLAGS) -c
TARGETS = gst-videotestsrc gst-audiotestsrc

all: $(TARGETS)

gst-videotestsrc: gst-videotestsrc.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $< $(LIBS)

gst-audiotestsrc: gst-audiotestsrc.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $< $(LIBS)

clean:
	rm -f $(TARGETS)
