#include "globals.h"

int oz_InitProgram(char * WndTitle, int Width, int Height)
{
	#ifdef WIN32
	return WinAPIInit(WndTitle, Width, Height);
	#else
	return XInit(WndTitle, Width, Height);
	#endif
}

int oz_APIMain()
{
	#ifdef WIN32
	return WinAPIMain();
	#else
	return XMain();
	#endif
}

int oz_ExitProgram()
{
	#ifdef WIN32
	WinAPIExit();
	#else
	return XExit();
	#endif

	return EXIT_SUCCESS;
}

int oz_SetWindowTitle(char * WndTitle)
{
	#ifdef WIN32
	return WinAPISetWindowTitle(WndTitle);
	#else
	return XSetWindowTitle(WndTitle);
	#endif
}

int oz_CreateFolder(char * Folder)
{
	#ifdef WIN32
	CreateDirectory(Folder, NULL);
	#else
	return mkdir(Folder, 0777);
	#endif

	return EXIT_SUCCESS;
}

int oz_ShowWindow(bool Stat)
{
	#ifdef WIN32
	return WinAPIShowWindow(Stat);
	#else
	return XShowWindow(Stat);
	#endif
}

int oz_Unimplemented(const char FuncName[])
{
	MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: Platform-specific code for %s not implemented\n", FuncName);
	return EXIT_FAILURE;
}
