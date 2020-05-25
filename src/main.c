#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <X11/Xlib.h>

#define USE_GRAPHIC_BALLS  // png images for balls

#ifdef USE_GRAPHIC_BALLS

#include "libpng/png.h"
#include "ximage-loader.h"

#endif 

#ifndef HAVE_LIBPNG
#include "b2.xpm"
#include "r2.xpm"
#include <X11/xpm.h>
#endif 

#include "vroot.h"
#include "twoballs.h"

#define BLACK 	0x000000
#define WHITE 	0xFFFFFF
#define LTBLUE 	0xA090FF
#define LTRED  	0xFF80A0

char * progname = NULL; // needed for ximage-loader.c to compile. Initialize with argv[0].

struct gameStruct
{
    int ball_w, ball_h;
    int width, height, depth;
} ;

static int parse_arguments(int argc, char *argv[]) 
{
	int idx;

	progname = strdup (argv[0]);

	for (idx = 1; idx < argc; idx++) {
		if (strcasecmp(argv[idx], "--debug") == 0) {
			return TRUE;
		}
	}

	return FALSE;
}

int main(int argc, char *argv[]) 
{
	int debug = parse_arguments(argc, argv);

	// Create our display
	Display *dpy = XOpenDisplay(getenv("DISPLAY"));

	char *xwin = getenv ("XSCREENSAVER_WINDOW");

	int root_window_id = 0;

  	if (xwin)
  	{
    	root_window_id = strtol (xwin, NULL, 0);
  	}

	// Get the root window
	Window root;
	if (debug == FALSE) 
	{
		// Get the root window
		// root = DefaultRootWindow(dpy);
		if (root_window_id == 0)
    	{
		    // root = DefaultRootWindow(dpy);
      	    printf ("usage as standalone app: %s --debug\n", argv[0]);
			free (progname);
      		return EXIT_FAILURE;
    	}
    	else
    	{
      		root = root_window_id;
    	}
	} 
	else 
	{
		// Let's create our own window.
		int screen = DefaultScreen(dpy);
		root = XCreateSimpleWindow(dpy, RootWindow(dpy, screen), 24, 48, 860,
				640, 1, BlackPixel(dpy, screen), BlackPixel(dpy, screen));

		XMapWindow(dpy, root);

		XStoreName(dpy, root, "Bouncing Balls");
	}

	XSelectInput (dpy, root, ExposureMask | StructureNotifyMask);

	// Get the window attributes
	XWindowAttributes wa;
	XGetWindowAttributes(dpy, root, &wa);

    struct gameStruct gs; 

    gs.width = wa.width;
    gs.height = wa.height;
    gs.depth = wa.depth;

	// Create the buffer
	Pixmap double_buffer = XCreatePixmap(dpy, root, wa.width, wa.height,
			wa.depth);
    
#ifdef USE_GRAPHIC_BALLS

    int ball_w = 0;
    int ball_h = 0;

#ifdef HAVE_LIBPNG
	Pixmap blue_ball = file_to_pixmap (dpy, root, "./images/b2.png",
                              &ball_w, &ball_h,
                              NULL); 

	Pixmap red_ball = file_to_pixmap (dpy, root, "./images/r2.png",
                              &ball_w, &ball_h,
                              NULL);

#else // no LIBPNG
    Pixmap blue_ball, red_ball;

    XpmAttributes attrs;
    memset (&attrs, 0, sizeof(attrs));
     
    attrs.valuemask = XpmColorKey | XpmDepth | XpmVisual | XpmColormap;
    attrs.color_key = XPM_COLOR;
    attrs.visual = DefaultVisual(dpy, DefaultScreen(dpy));
    attrs.colormap = DefaultColormap(dpy, DefaultScreen(dpy));
    attrs.depth = DefaultDepth(dpy, DefaultScreen(dpy));

    XpmCreatePixmapFromData (dpy, root, b2_data, &blue_ball, NULL, &attrs) ;

    assert(attrs.width == BALL_SIZE);

    XpmCreatePixmapFromData (dpy, root, r2_data, &red_ball, NULL, &attrs) ;
    
    ball_w = attrs.width, ball_h = attrs.height;
#endif  // HAVE_LIBPNG   

    assert(ball_h == ball_w);
    assert(ball_w == BALL_SIZE);

    gs.ball_h = ball_h;
    gs.ball_w = ball_w;

#endif // GRAPHIC_BALLS    

	// And new create our graphics context to draw on
	GC gc = XCreateGC(dpy, root, 0, NULL);

	srand(time(NULL));

	game_init();

	setSizes (wa.width, wa.height);

	// this is to terminate nicely:
	Atom wmDeleteMessage = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(dpy, root, &wmDeleteMessage, 1);

    struct point ball_a, ball_b;

	while ( TRUE )
	{
		XEvent event;
		if (XCheckWindowEvent(dpy, root, StructureNotifyMask, &event) ||
		    XCheckTypedWindowEvent (dpy, root, ClientMessage, &event)) // needed to catch ClientMessage
		{
			if (event.type == ConfigureNotify) 
        	{
          		XConfigureEvent xce = event.xconfigure;

		        // This event type is generated for a variety of
          		// happenings, so check whether the window has been
          		// resized.

          		if (xce.width != gs.width || xce.height != gs.height) 
          		{
            		gs.width = xce.width;
            		gs.height = xce.height;

    				XFreePixmap(dpy, double_buffer);
    				double_buffer = XCreatePixmap(dpy, root, gs.width, gs.height,
							gs.depth);

					setSizes (gs.width, gs.height);
          
            		continue;
          		}
        	}
			else if (event.type == ClientMessage)
        	{
            	if (event.xclient.data.l[0] == wmDeleteMessage)
            	{
                	break;
            	}
			}
		}

		// Clear the pixmap used for double buffering
		XSetBackground(dpy, gc, BLACK);
		XSetForeground(dpy, gc, BLACK);
		XFillRectangle(dpy, double_buffer, gc, 0, 0, gs.width, gs.height);

		// Move the points around

		game_cycle (&ball_a, &ball_b);

		// draw two balls
#ifndef USE_GRAPHIC_BALLS        
		XSetForeground(dpy, gc, LTBLUE);
		XSetBackground(dpy, gc, LTBLUE);

		XFillArc (dpy, double_buffer, gc, ball_a.x, ball_a.y, 
				BALL_SIZE + 1, BALL_SIZE + 1, 0, 360 * 64);
#else 
		XCopyArea(dpy, blue_ball, double_buffer, gc, 0, 0, 
				gs.ball_w, gs.ball_h, ball_a.x, ball_a.y);
#endif 

#ifndef USE_GRAPHIC_BALLS        
		XSetForeground(dpy, gc, LTRED);
		XSetBackground(dpy, gc, LTRED);

        XFillArc (dpy, double_buffer, gc, ball_b.x, ball_b.y, 
				BALL_SIZE + 1, BALL_SIZE + 1, 0, 360 * 64);
#else                 

		XCopyArea(dpy, red_ball, double_buffer, gc, 0, 0, 
				gs.ball_w, gs.ball_h, ball_b.x, ball_b.y);
#endif 

		XCopyArea(dpy, double_buffer, root, gc, 0, 0, gs.width, gs.height, 0, 0);

		XFlush (dpy);

		usleep(15000);
	}

	// cleanup
#ifdef USE_GRAPHIC_BALLS
    XFreePixmap (dpy, blue_ball);
    XFreePixmap (dpy, red_ball);
#endif     

	XFreePixmap(dpy, double_buffer);
	XFreeGC (dpy, gc);
  	XDestroyWindow(dpy, root);
  	XCloseDisplay (dpy);

	free (progname);

	return EXIT_SUCCESS;
}




