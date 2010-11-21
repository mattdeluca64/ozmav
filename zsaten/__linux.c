#include "globals.h"

#ifndef WIN32
static int snglBuf[] = {GLX_RGBA, GLX_DEPTH_SIZE, 16, None};
static int dblBuf[] = {GLX_RGBA, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};

Display *dpy;
Window win;

XVisualInfo *vi;
Colormap cmap;
XSetWindowAttributes swa;
GLXContext cx;
XEvent event;

int dummy;

int XInit(char * WndTitle, int Width, int Height)
{
	dpy = XOpenDisplay(NULL);
	if (dpy == NULL) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: Could not open display\n");
		return EXIT_FAILURE; }
	if(!glXQueryExtension(dpy, &dummy, &dummy)) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: X server has no OpenGL GLX extension\n");
		return EXIT_FAILURE; }

	vi = glXChooseVisual(dpy, DefaultScreen(dpy), dblBuf);
	if (vi == NULL) {
		vi = glXChooseVisual(dpy, DefaultScreen(dpy), snglBuf);
		if (vi == NULL) {
			dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: No RGB visual with depth buffer\n");
			return EXIT_FAILURE; }
	}
	if(vi->class != TrueColor) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: TrueColor visual required for this program\n");
		return EXIT_FAILURE; }

	cx = glXCreateContext(dpy, vi, None, GL_TRUE);
	if (cx == NULL) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: Could not create rendering context\n");
		return EXIT_FAILURE; }

	cmap = XCreateColormap(dpy, RootWindow(dpy, vi->screen), vi->visual, AllocNone);
	swa.colormap = cmap;
	swa.border_pixel = 0;
	swa.event_mask = KeyPressMask | ExposureMask | ButtonPressMask | StructureNotifyMask;
	win = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 0, 0, Width, Height, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask, &swa);
	XSetStandardProperties(dpy, win, WndTitle, WndTitle, None, NULL, 0, NULL);

	glXMakeCurrent(dpy, win, cx);

	XMapWindow(dpy, win);

	XSelectInput(dpy, win, ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | Button1MotionMask | StructureNotifyMask);

	return EXIT_SUCCESS;
}

int XMain()
{
	while(XPending(dpy) > 0) {
		XNextEvent(dpy, &event);
		switch (event.type) {
			case Expose: {
				XFlush(dpy);
				break; }
			case KeyPress: {
				KeySym     keysym;
				XKeyEvent *kevent;
				char       buffer[1];
				kevent = (XKeyEvent *) &event;
				if((XLookupString((XKeyEvent *)&event,buffer,1,&keysym,NULL) == 1) && (keysym == (KeySym)XK_Escape)) vProgram.isRunning = false;
				vProgram.key[keysym & 0xFF] = true;
				break; }
			case KeyRelease: {
				KeySym     keysym;
				char       buffer[1];
				XLookupString((XKeyEvent *)&event,buffer,1,&keysym,NULL);
				vProgram.key[keysym & 0xFF] = false;
				break; }

			case ButtonPress: {
				vProgram.mouseCenterX = event.xbutton.x;
				vProgram.mouseCenterY = event.xbutton.y;
				break; }

			case MotionNotify: {
				if(event.xmotion.state & Button1Mask) {
					vProgram.mousePosX = event.xbutton.x;
					vProgram.mousePosY = event.xbutton.y;
					ca_MouseMove(vProgram.mousePosX, vProgram.mousePosY);
					ca_Orientation(vCamera.angleX, vCamera.angleY);
				}

				break; }

			case ConfigureNotify: {
				vProgram.windowWidth = event.xconfigure.width;
				vProgram.windowHeight = event.xconfigure.height;
				gl_SetupScene3D(vProgram.windowWidth, vProgram.windowHeight);
				break; }
		}
	}

	doKbdInput();

	return EXIT_SUCCESS;
}

int XExit()
{
	glXMakeCurrent(dpy, None, NULL);
	glXDestroyContext(dpy, cx);
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);

	return EXIT_SUCCESS;
}

int XSetWindowTitle(char * WndTitle)
{
	XSetStandardProperties(dpy, win, WndTitle, WndTitle, None, NULL, 0, NULL);

	return EXIT_SUCCESS;
}
#endif
