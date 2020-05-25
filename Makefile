CC=gcc

srcdir=src

#ifndef use_xpm
ifneq ($(use_xpm),1)
	CFLAGS = -std=gnu99 -Wall -O2 -DHAVE_LIBPNG
	CLIBS = -lX11 -lm -lpng
	IMG_LOADER = ximage_loader
	IMG_LOADER_OUT = ximage-loader.o
else
	CFLAGS = -std=gnu99 -Wall -O2
	CLIBS = -lX11 -lm -lXpm
endif

# action:
#	echo argument is $(argument)

all : $(srcdir)/main.c $(srcdir)/vroot.h twoballs $(IMG_LOADER)
		$(CC) $(CFLAGS) -o twoballs $(srcdir)/main.c twoballs.o $(IMG_LOADER_OUT) $(CLIBS)

twoballs : $(srcdir)/twoballs.c $(srcdir)/twoballs.h
		$(CC) $(CFLAGS) -c $(srcdir)/twoballs.c

ximage_loader : $(srcdir)/ximage-loader.c $(srcdir)/ximage-loader.h
		$(CC) $(CFLAGS) -c $(srcdir)/ximage-loader.c

.PHONY: clean

clean :
		-rm twoballs ximage-loader.o twoballs.o