** This program is based on a small JavaScript written around 2004. **

*******************************************************************************************

To compile:

# sudo apt-get update -y
# sudo apt-get install -y libx11-dev

To install libpng (to show balls in good quality):

# sudo apt-get install -y libpng-dev

To install xpm (a library that can use .xmp files to load them into Pixmaps, see
  XpmCreatePixmapFromData) use:

# sudo apt-get install -y libxpm-dev

In this case to build run: 

# make use_xpm=1

*******************************************************************************************

To run as a standalone app:

# ./twoballs --debug

To intall as xscreensaver, use xpm option to build (libpng requires reading .png files 
from local dir - see file_to_pixmap() - so unless you also want to intstall them along 
with the app or use global path, it is not going to work).

# make use_xpm=1

# sudo cp ./twoballs /usr/lib/xscreensaver 

*******************************************************************************************

Some ideas for improvement:

1. Make xscreensaver version use png by side-stepping the logic with loading files:
    for example store .png files as binary array within the app, save them into a temp file and then use libpng to access this file (dont forget to delete it).

2. Currently two balls are bouncing on even flat surface. We can make surface sloped or
    bumpy - or both - also perhaps must display it.

3. Make balls "rotate" as they move.    






