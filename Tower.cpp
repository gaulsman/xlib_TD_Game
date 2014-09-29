/*
Commands to compile and run:

    g++ -o Tower Tower.cpp -L/usr/X11R6/lib -lX11 -lstdc++
    ./Tower

Note: the -L option and -lstdc++ may not be needed on some machines.

*/

#include <cstdlib>
#include <iostream>
#include <list>
#include <unistd.h>
#include "floor.h"
#include "monster.h"
#include "projectile.h"
#include "scoreboard.h"
#include "turret.h"

/*
 * Header files for X functions
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>

using namespace std;

const int FPS = 30, width = 900, height = 540, startX = 300, startY = 60, initialGold = 1000, lives = 50, tileSize = 30;



/*
 * Information to draw on the window.
 */

/*
 * Function to put out a message on error exits.
 */
void error(string str) {
    cerr << str << endl;
    exit(0);
}

/*
 * Create a window
 */
void initX(int argc, char* argv[], XInfo& xInfo) {
	XSizeHints hints;
	hints.x = 100;
	hints.y = 100;
	hints.width = width;
	hints.height = height;
	hints.flags = PPosition | PSize;
    /*
    * Display opening uses the DISPLAY	environment variable.
    * It can go wrong if DISPLAY isn't set, or you don't have permission.
    */
    xInfo.display = XOpenDisplay("");
    if (!xInfo.display)	{
        error("Can't open display.");
    }

    /*
    * Find out some things about the display you're using.
    */
    xInfo.screen = DefaultScreen(xInfo.display); // macro to get default screen index

    unsigned long white, black;
	XColor yellow, gold, orange, darkOrange, red, darkRed, green1, green2, green3, blue1, blue2, blue3, brown, pink, purple, gray;
	Colormap colormap = XDefaultColormap(xInfo.display, xInfo.screen);
    white = XWhitePixel(xInfo.display, xInfo.screen); 
    black = XBlackPixel(xInfo.display, xInfo.screen);

	if (XAllocNamedColor(xInfo.display, colormap, "yellow", &yellow, &yellow) == 0) 
		error("ERROR: XAllocNamedColor: failed to allocated yellow");
	if (XAllocNamedColor(xInfo.display, colormap, "gold", &gold, &gold) == 0) 
		error("ERROR: XAllocNamedColor: failed to allocated gold");
	if (XAllocNamedColor(xInfo.display, colormap, "dark orange", &orange, &orange) == 0) 
		error("ERROR: XAllocNamedColor: failed to allocated orange");
	if (XAllocNamedColor(xInfo.display, colormap, "orange red", &darkOrange, &darkOrange) == 0) 
		error("ERROR: XAllocNamedColor: failed to allocated darkOrange");
	if (XAllocNamedColor(xInfo.display, colormap, "red", &red, &red) == 0) 
		error("ERROR: XAllocNamedColor: failed to allocated red");
	if (XAllocNamedColor(xInfo.display, colormap, "dark red", &darkRed, &darkRed) == 0) 
		error("ERROR: XAllocNamedColor: failed to allocated darkRed");
	if (XAllocNamedColor(xInfo.display, colormap, "green", &green1, &green1) == 0) 
		error("ERROR: XAllocNamedColor: failed to allocated green1");
	if (XAllocNamedColor(xInfo.display, colormap, "forest green", &green2, &green2) == 0) 
		error("ERROR: XAllocNamedColor: failed to allocated green2");
	if (XAllocNamedColor(xInfo.display, colormap, "dark green", &green3, &green3) == 0) 
		error("ERROR: XAllocNamedColor: failed to allocated green3");
	if (XAllocNamedColor(xInfo.display, colormap, "cyan", &blue1, &blue1) == 0) 
		error("ERROR: XAllocNamedColor: failed to allocated blue1");
	if (XAllocNamedColor(xInfo.display, colormap, "blue", &blue2, &blue2) == 0) 
		error("ERROR: XAllocNamedColor: failed to allocated blue2");
	if (XAllocNamedColor(xInfo.display, colormap, "dark blue", &blue3, &blue3) == 0) 
		error("ERROR: XAllocNamedColor: failed to allocated blue3");
	if (XAllocNamedColor(xInfo.display, colormap, "gray66", &gray, &gray) == 0) 
		error("ERROR: XAllocNamedColor: failed to allocated gray");
	if (XAllocNamedColor(xInfo.display, colormap, "deep pink", &pink, &pink) == 0) 
		error("ERROR: XAllocNamedColor: failed to allocated pink");
	if (XAllocNamedColor(xInfo.display, colormap, "purple", &purple, &purple) == 0) 
		error("ERROR: XAllocNamedColor: failed to allocated purple");
	if (XAllocNamedColor(xInfo.display, colormap, "chocolate", &brown, &brown) == 0) 
		error("ERROR: XAllocNamedColor: failed to allocated brown");
    xInfo.window = XCreateSimpleWindow(
                       xInfo.display,				// display where window appears
                       DefaultRootWindow( xInfo.display ), // window's parent in window tree
                       hints.x, hints.y,			           // upper left corner location
                       hints.width, hints.height,	               // size of the window
                       5,						     // width of window's border
                       black,						// window border colour
                       white);					    // window background colour

    // extra window properties like a window title
    XSetStandardProperties(
        xInfo.display,		// display containing the window
        xInfo.window,		// window whose properties are set
        "Tower Defense",	// window's title
        "TD",				// icon's title
        None,				// pixmap for the icon
        argv, argc,			// applications command line args
        None );			// size hints for the window


	/* 
	 * Create Graphics Contexts
	 */
	int i = 0;
	xInfo.gc.push_back(XCreateGC(xInfo.display, xInfo.window, 0, 0));
	XSetForeground(xInfo.display, xInfo.gc[i], black);
	XSetBackground(xInfo.display, xInfo.gc[i], white);
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i], 1, LineSolid, CapButt, JoinRound);

	i = 1;
	xInfo.gc.push_back(XCreateGC(xInfo.display, xInfo.window, 0, 0));
	XSetForeground(xInfo.display, xInfo.gc[i], white);
	XSetBackground(xInfo.display, xInfo.gc[i], white);
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i], 1, LineSolid, CapButt, JoinRound);

	i = 2;
	xInfo.gc.push_back(XCreateGC(xInfo.display, xInfo.window, 0, 0));
	XSetForeground(xInfo.display, xInfo.gc[i], yellow.pixel);
	XSetBackground(xInfo.display, xInfo.gc[i], white);
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i], 1, LineSolid, CapButt, JoinRound);
	 
	i = 3;
	xInfo.gc.push_back(XCreateGC(xInfo.display, xInfo.window, 0, 0));
	XSetForeground(xInfo.display, xInfo.gc[i], gold.pixel);
	XSetBackground(xInfo.display, xInfo.gc[i], white);
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i], 1, LineSolid, CapButt, JoinRound);

	i = 4;
	xInfo.gc.push_back(XCreateGC(xInfo.display, xInfo.window, 0, 0));
	XSetForeground(xInfo.display, xInfo.gc[i], orange.pixel);
	XSetBackground(xInfo.display, xInfo.gc[i], white);
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i], 1, LineSolid, CapButt, JoinRound);
	
	i = 5;
	xInfo.gc.push_back(XCreateGC(xInfo.display, xInfo.window, 0, 0));
	XSetForeground(xInfo.display, xInfo.gc[i], darkOrange.pixel);
	XSetBackground(xInfo.display, xInfo.gc[i], white);
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i], 1, LineSolid, CapButt, JoinRound);
	
	i = 6;
	xInfo.gc.push_back(XCreateGC(xInfo.display, xInfo.window, 0, 0));
	XSetForeground(xInfo.display, xInfo.gc[i], red.pixel);
	XSetBackground(xInfo.display, xInfo.gc[i], white);
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i], 1, LineSolid, CapButt, JoinRound);

	i = 7;
	xInfo.gc.push_back(XCreateGC(xInfo.display, xInfo.window, 0, 0));
	XSetForeground(xInfo.display, xInfo.gc[i], darkRed.pixel);
	XSetBackground(xInfo.display, xInfo.gc[i], white);
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i], 1, LineSolid, CapButt, JoinRound);

	i = 8;
	xInfo.gc.push_back(XCreateGC(xInfo.display, xInfo.window, 0, 0));
	XSetForeground(xInfo.display, xInfo.gc[i], green1.pixel);
	XSetBackground(xInfo.display, xInfo.gc[i], white);
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i], 1, LineSolid, CapButt, JoinRound);

	i = 9;
	xInfo.gc.push_back(XCreateGC(xInfo.display, xInfo.window, 0, 0));
	XSetForeground(xInfo.display, xInfo.gc[i], green2.pixel);
	XSetBackground(xInfo.display, xInfo.gc[i], white);
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i], 1, LineSolid, CapButt, JoinRound);

	i = 10;
	xInfo.gc.push_back(XCreateGC(xInfo.display, xInfo.window, 0, 0));
	XSetForeground(xInfo.display, xInfo.gc[i], green3.pixel);
	XSetBackground(xInfo.display, xInfo.gc[i], white);
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i], 1, LineSolid, CapButt, JoinRound);

	i = 11;
	xInfo.gc.push_back(XCreateGC(xInfo.display, xInfo.window, 0, 0));
	XSetForeground(xInfo.display, xInfo.gc[i], blue1.pixel);
	XSetBackground(xInfo.display, xInfo.gc[i], white);
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i], 1, LineSolid, CapButt, JoinRound);

	i = 12;
	xInfo.gc.push_back(XCreateGC(xInfo.display, xInfo.window, 0, 0));
	XSetForeground(xInfo.display, xInfo.gc[i], blue2.pixel);
	XSetBackground(xInfo.display, xInfo.gc[i], white);
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i], 1, LineSolid, CapButt, JoinRound);

	i = 13;
	xInfo.gc.push_back(XCreateGC(xInfo.display, xInfo.window, 0, 0));
	XSetForeground(xInfo.display, xInfo.gc[i], blue3.pixel);
	XSetBackground(xInfo.display, xInfo.gc[i], white);
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i], 1, LineSolid, CapButt, JoinRound);

	i = 14;
	xInfo.gc.push_back(XCreateGC(xInfo.display, xInfo.window, 0, 0));
	XSetForeground(xInfo.display, xInfo.gc[i], gray.pixel);
	XSetBackground(xInfo.display, xInfo.gc[i], white);
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i], 1, LineDoubleDash, CapButt, JoinRound);

	i = 15;
	xInfo.gc.push_back(XCreateGC(xInfo.display, xInfo.window, 0, 0));
	XSetForeground(xInfo.display, xInfo.gc[i], pink.pixel);
	XSetBackground(xInfo.display, xInfo.gc[i], white);
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i], 1, LineSolid, CapButt, JoinRound);

	i = 16;
	xInfo.gc.push_back(XCreateGC(xInfo.display, xInfo.window, 0, 0));
	XSetForeground(xInfo.display, xInfo.gc[i], purple.pixel);
	XSetBackground(xInfo.display, xInfo.gc[i], white);
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i], 1, LineSolid, CapButt, JoinRound);
	 
	i = 17;
	xInfo.gc.push_back(XCreateGC(xInfo.display, xInfo.window, 0, 0));
	XSetForeground(xInfo.display, xInfo.gc[i], brown.pixel);
	XSetBackground(xInfo.display, xInfo.gc[i], white);
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i], 1, LineSolid, CapButt, JoinRound);

	i = 18;
	xInfo.gc.push_back(XCreateGC(xInfo.display, xInfo.window, 0, 0));
	XSetForeground(xInfo.display, xInfo.gc[i], black);
	XSetBackground(xInfo.display, xInfo.gc[i], white);
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i], 2, LineSolid, CapRound, JoinRound);
	
	i = 19;
	xInfo.gc.push_back(XCreateGC(xInfo.display, xInfo.window, 0, 0));
	XSetForeground(xInfo.display, xInfo.gc[i], black);
	XSetBackground(xInfo.display, xInfo.gc[i], white);
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i], 1, LineSolid, CapButt, JoinRound);

	// Create Pixmap for double buffer
	int depth = DefaultDepth(xInfo.display, DefaultScreen(xInfo.display));
	xInfo.pixmap = XCreatePixmap(xInfo.display, xInfo.window, 3000, 2000, depth);
	xInfo.width = hints.width;
	xInfo.height = hints.height;

	XSetWindowBackgroundPixmap(xInfo.display, xInfo.window, None);
    /*
     * Put the window on the screen.
     */
	XSelectInput(xInfo.display, xInfo.window,
                  ButtonPressMask | KeyPressMask |
                  ExposureMask | PointerMotionMask | 
				  EnterWindowMask | LeaveWindowMask |			
				  StructureNotifyMask);
    XMapRaised( xInfo.display, xInfo.window);

    XFlush(xInfo.display);
    sleep(2);	// let server get set up before sending drawing commands
}

void repaint(XInfo &xInfo, Floor *fl, bool resize) {
	XFillRectangle(xInfo.display, xInfo.pixmap, xInfo.gc[1], 
		0, 0, xInfo.width, xInfo.height);
	if (resize)
		fl->resize();
	else 
		fl->draw();
	// copy buffer to window
	XCopyArea(xInfo.display, xInfo.pixmap, xInfo.window, xInfo.gc[0], 
		0, 0, xInfo.width, xInfo.height,  // region of pixmap to copy
		0, 0); // position to put top left corner of pixmap in window
	XFlush(xInfo.display);
}

void handleButtonPress(XInfo &xInfo, XEvent &e, Floor *fl){
	//cout << e.xbutton.x - 2 << ", " << e.xbutton.y - 2 << " " << e.xbutton.button << endl;
	fl->click(e.xbutton.x - 2, e.xbutton.y - 2, e.xbutton.button);
}

void handleMotion(XInfo &xInfo, XEvent &e, bool inside, Floor *fl){
	if (inside)
		fl->move(e.xmotion.x, e.xmotion.y);
}

void handleKeyPress(XInfo &xInfo, XEvent &e, Floor *fl){
	//cout << "pressed key " << e.xkey.keycode << endl;
	fl->press(e.xkey.keycode);
}

void handleResize(XInfo &xInfo, XEvent &e, Floor *fl){
	xInfo.width = e.xconfigure.width;
	xInfo.height = e.xconfigure.height;
	repaint(xInfo, fl, true);
}

void eventLoop(XInfo &xInfo, Floor *fl) {
	// Add stuff to paint to the display list

	XEvent event;
	unsigned long lastRepaint = 0;
	bool inside = false;
	int tick = 0;
	while(true){
		if (XPending(xInfo.display) > 0) {
			XNextEvent(xInfo.display, &event);
			switch(event.type) {
				case ButtonPress:
					handleButtonPress(xInfo, event, fl);
					break;
				case KeyPress:
					handleKeyPress(xInfo, event, fl);
					break;
				case MotionNotify:
					handleMotion(xInfo, event, inside, fl);
					break;
				case EnterNotify:
					inside = true;
					break;
				case LeaveNotify:
					inside = false;
					break;
				case ConfigureNotify:
					handleResize(xInfo, event, fl);
					break;	
			}
		} 
		else {
			usleep(2000 / ((fl->gameSpeed == 0) ? 1 : fl->gameSpeed));
			tick++;
			if (fl->gameSpeed != 0){
				if (fl->tick())
					fl->gameOver();
			}
            if (tick >= 20 * ((fl->gameSpeed == 0) ? 1 : fl->gameSpeed)){
		     	repaint(xInfo, fl, false);
				tick = 0;
			}
		}
	}
}

/*
 *   Start executing here.
 *	 First initialize window.
 *	 Next loop responding to events.
 *	 Exit forcing window manager to clean up - cheesy, but easy.
 */
int main ( int argc, char* argv[] ) {
	
    XInfo xInfo;
	vector<pair<int, int> > path;
	//Initialize path for monsters
	for (int x = 0; x < 7; x++)
		path.push_back(make_pair(x, 3));
	for (int x = 2; x > 0; x--)
		path.push_back(make_pair(6, x));
	for (int x = 7; x < 19; x++)
		path.push_back(make_pair(x, 1));
	for (int x = 2; x < 11; x++)
		path.push_back(make_pair(18, x));
	for (int x = 17; x > 7; x--)
		path.push_back(make_pair(x, 10));
	for (int x = 9; x > 6; x--)
		path.push_back(make_pair(8, x));
	for (int x = 9; x < 16; x++)
		path.push_back(make_pair(x, 7));
	for (int x = 6; x > 2; x--)
		path.push_back(make_pair(15, x));
	for (int x = 14; x > 7; x--)
		path.push_back(make_pair(x, 3));
	for (int x = 4; x < 6; x++)
		path.push_back(make_pair(8, x));
	for (int x = 7; x > 0; x--)
		path.push_back(make_pair(x, 5));
	for (int x = 6; x < 14; x++)
		path.push_back(make_pair(1, x));
	for (int x = 2; x < 20; x++)
		path.push_back(make_pair(x, 13));
	Scoreboard *sb = new Scoreboard(startX, startY, initialGold, lives, tileSize, xInfo);
	Floor *fl = new Floor(startX, startY, tileSize, path, sb, xInfo);
    initX(argc, argv, xInfo);
    eventLoop(xInfo, fl);
	delete fl;
    XCloseDisplay(xInfo.display);
}


