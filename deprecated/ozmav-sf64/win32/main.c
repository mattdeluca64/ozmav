/*	------------------------------------------------------------
	OZMAV SF64
	 - Branch of 'OpenGL Zelda Map Viewer' for Starfox 64

	Written in October/November 2008 by xdaniel & contributors
	http://ozmav.googlecode.com/
	------------------------------------------------------------
	main.c - Windows stuff, basic initialization + camera
	------------------------------------------------------------ */

#include "../globals.h"

/*	------------------------------------------------------------
	SYSTEM FUNCTIONS - OPENGL & WINDOWS
	------------------------------------------------------------ */

int WINAPI WinMain (HINSTANCE, HINSTANCE, LPSTR, int);
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*	------------------------------------------------------------
	VARIABLES
	------------------------------------------------------------ */

/* WINDOW HANDLING VARIABLES, ETC. */
HWND			hwnd = NULL;
HMENU			hmenu = NULL;
HWND			hogl = NULL;
HWND			hstatus = NULL;

HDC				hDC_ogl = NULL;
HGLRC			hRC = NULL;
HINSTANCE		hInstance;

char			szClassName[] = "OZMAVClass";

/* GENERAL GLOBAL PROGRAM VARIABLES */
bool			System_KbdKeys[256];

char			AppTitle[256] = "OZMAV-SF64";
char			AppVersion[256] = "V0.15 (Z0.55)";
char			AppBuildName[256] = "insanity strikes - in space!";
char			AppPath[512] = "";
char			INIPath[512] = "";
char			WindowTitle[256] = "";
char			StatusMsg[256] = "";
char			ErrorMsg[256] = "";

bool			ObjLoaded = false;
bool			WndActive = true;
bool			ExitProgram = false;

char			CurrentGFXCmd[256] = "";
char			CurrentGFXCmdNote[256] = "";
char			GFXLogMsg[256] = "";
char			SystemLogMsg[256] = "";

bool			GFXLogOpened = false;

/* CAMERA / VIEWPOINT VARIABLES */
float			CamAngleX, CamAngleY;
float			CamX, CamY, CamZ;
float			CamLX, CamLY, CamLZ;

int				MousePosX = 0, MousePosY = 0;
int				MouseCenterX = 0, MouseCenterY = 0;

bool			MouseButtonDown = false;

/* FILE HANDLING VARIABLES */
FILE			* FileObj;
unsigned int	* ObjBuffer;
unsigned long	ObjFilesize = 0;
char			Filename_Obj[256] = "";
bool			ObjExists = false;

FILE			* FileGFXLog;
FILE			* FileSystemLog;

/* DATA READOUT VARIABLES */
unsigned long	Readout_Current1 = 0;
unsigned long	Readout_Current2 = 0;
unsigned int	Readout_CurrentByte1 = 0;
unsigned int	Readout_CurrentByte2 = 0;
unsigned int	Readout_CurrentByte3 = 0;
unsigned int	Readout_CurrentByte4 = 0;
unsigned int	Readout_CurrentByte5 = 0;
unsigned int	Readout_CurrentByte6 = 0;
unsigned int	Readout_CurrentByte7 = 0;
unsigned int	Readout_CurrentByte8 = 0;

unsigned long	Readout_NextGFXCommand1 = 0;

/* F3DZEX DISPLAY LIST HANDLING VARIABLES */
unsigned long	DLists[2048];
signed long		DListInfo_CurrentCount = 0;
signed long		DListInfo_DListToRender = 0;
unsigned long	DLTempPosition = 0;

unsigned int	DLToRender = 0;
bool			DListHasEnded = false;

bool			SubDLCall = false;

/* F3DZEX TEXTURE HANDLING VARIABLES */
unsigned char	* TextureData_OGL;
unsigned char	* TextureData_N64;

unsigned char	* PaletteData;

/* GENERAL RENDERER VARIABLES */
GLuint			Renderer_GLDisplayList = 0;
GLuint			Renderer_GLDisplayList_Current = 0;

GLuint			Renderer_GLTexture = 0;

GLuint			Renderer_FilteringMode_Min = GL_LINEAR;
GLuint			Renderer_FilteringMode_Mag = GL_LINEAR;

DWORD			Renderer_LastFPS = 0;
int				Renderer_FPS, Renderer_FrameNo = 0;
char			Renderer_FPSMessage[32] = "";

char			Renderer_CoordDisp[256] = "";

bool			Renderer_EnableLighting = true;

GLfloat			LightAmbient[]= { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat			LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat			LightPosition[]= { 1.0f, 1.0f, 1.0f, 1.0f };

GLfloat			FogColor[]= { 0.0f, 0.0f, 0.0f, 0.5f };
GLfloat			PrimColor[]= { 1.0f, 1.0f, 1.0f, 1.0f };

/* OPENGL EXTENSION VARIABLES */
char			* GLExtension_List;
bool			GLExtension_TextureMirror = false;
bool			GLExtension_AnisoFilter = false;
char			GLExtensionsSupported[256] = "";

bool			GLExtensionsUnsupported = false;
char			GLExtensionsErrorMsg[512] = "";

/*	------------------------------------------------------------
	STRUCTURES
	------------------------------------------------------------ */

/* VERTEX DATA STRUCTURE */
struct Vertex_Struct Vertex[4096];
/* TEXTURE DATA STRUCTURE */
struct Texture_Struct Texture;
/* CI TEXTURE PALETTE STRUCTURE */
struct Palette_Struct Palette[512];

/*	------------------------------------------------------------ */

int Viewer_Initialize()
{
	FileSystemLog = fopen("log.txt", "w");

	if(Viewer_OpenObject() == -1) {
		return 0;
	}

	Viewer_RenderObject();

	EnableMenuItem(hmenu, IDM_CAMERA_RESETCOORDS, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_MAP_PREVDLIST, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_MAP_NEXTDLIST, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_OPTIONS_FILTERNEAREST, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_OPTIONS_FILTERLINEAR, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_OPTIONS_FILTERMIPMAP, MF_BYCOMMAND | MF_ENABLED);

	sprintf(WindowTitle, "%s %s - %s", AppTitle, AppVersion, Filename_Obj);
	SetWindowText(hwnd, WindowTitle);

	return 0;
}

int Viewer_OpenObject()
{
	ObjLoaded = false;

	/* FREE PREVIOUSLY ALLOCATED MEMORY */
	if(ObjBuffer != NULL) free(ObjBuffer);
	if(PaletteData != NULL) free(PaletteData);

	/* OPEN FILE */
	FileObj = fopen(Filename_Obj, "r+b");
	/* GET FILESIZE */
	size_t Result;
	fseek(FileObj, 0, SEEK_END);
	ObjFilesize = ftell(FileObj);
	rewind(FileObj);
	/* LOAD FILE INTO BUFFER */
	ObjBuffer = (unsigned int*) malloc (sizeof(int) * ObjFilesize);
	Result = fread(ObjBuffer, 1, ObjFilesize, FileObj);
	/* CLOSE FILE */
	fclose(FileObj);

	DListInfo_DListToRender = 0;

	glEnable(GL_LIGHTING); glEnable(GL_NORMALIZE); Renderer_EnableLighting = true;

	memset(Vertex, 0x00, sizeof(Vertex));

	PaletteData = (unsigned char *) malloc (1024);
	memset(PaletteData, 0x00, sizeof(PaletteData));

	Viewer_GetDisplayLists(ObjFilesize);

	CamAngleX = 0.0f, CamAngleY = 0.0f;
	CamX = 0.0f, CamY = 0.0f, CamZ = 5.0f;
	CamLX = 0.0f, CamLY = 0.0f, CamLZ = -1.0f;

	memset(CurrentGFXCmd, 0x00, sizeof(CurrentGFXCmd));
	memset(CurrentGFXCmdNote, 0x00, sizeof(CurrentGFXCmdNote));
	memset(GFXLogMsg, 0x00, sizeof(GFXLogMsg));
	memset(SystemLogMsg, 0x00, sizeof(SystemLogMsg));

	sprintf(StatusMsg, "File loaded successfully! Entry Point: #%d", DListInfo_DListToRender);

	return 0;
}

/*	------------------------------------------------------------ */

void GLUTCamera_Orientation(float ang, float ang2)
{
	CamLX = sin(ang);
	CamLY = ang2;
	CamLZ = -cos(ang);
}

void GLUTCamera_Movement(int direction)
{
	CamX = CamX + direction * (CamLX) * 0.025f;
	CamY = CamY + direction * (CamLY) * 0.025f;
	CamZ = CamZ + direction * (CamLZ) * 0.025f;
}

void Camera_MouseMove(int x, int y)
{
	MousePosX = x - MouseCenterX;
	MousePosY = y - MouseCenterY;

	CamAngleX = CamAngleX + (0.01f * MousePosX);
	CamAngleY = CamAngleY - (0.01f * MousePosY);

	MouseCenterX = x;
	MouseCenterY = y;
}

/*	------------------------------------------------------------ */

void Dialog_OpenObj(HWND hwnd)
{
	char			filter[] = "All files (*.*)\0;*.*\0";
	OPENFILENAME	ofn;

	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize		= sizeof(OPENFILENAME);
	ofn.hwndOwner		= hwnd;
	ofn.lpstrFilter		= filter;
	ofn.nFilterIndex	= 1;
	ofn.lpstrFile		= Filename_Obj;
	ofn.nMaxFile		= 256;
	ofn.Flags			= OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	if(GetOpenFileName(&ofn)) ObjExists = true;

	return;
}

/*	------------------------------------------------------------ */

/* WINMAIN - WINDOWS PROGRAM MAIN FUNCTION */
int WINAPI WinMain (HINSTANCE hThisInstance,
					HINSTANCE hPrevInstance,
					LPSTR lpszArgument,
					int nFunsterStil)
{
	MSG messages;
	WNDCLASSEX wincl;

	InitCommonControls();

	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = szClassName;
	wincl.lpfnWndProc = WindowProcedure;
	wincl.style = CS_DBLCLKS;
	wincl.cbSize = sizeof (WNDCLASSEX);

	wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
	wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
	wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
	wincl.lpszMenuName = MAKEINTRESOURCE(IDM_MAINMENU);
	wincl.cbClsExtra = 0;
	wincl.cbWndExtra = 0;
	wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

	if (!RegisterClassEx (&wincl))
		return 0;

	hwnd = CreateWindowEx (
			0,
			szClassName,
			"",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			648,
			551,
			HWND_DESKTOP,
			NULL,
			hThisInstance,
			NULL
			);

	hmenu = GetMenu(hwnd);

	hogl = CreateWindowEx (
			WS_EX_CLIENTEDGE,
			"STATIC",
			"",
			WS_CHILD | WS_VISIBLE,
			0,
			0,
			128,
			128,
			hwnd,
			(HMENU)IDC_MAIN_OPENGL,
			GetModuleHandle(NULL),
			NULL
			);

	int hstatuswidths[] = {40, 220, -1};
	hstatus = CreateWindowEx(
			0,
			STATUSCLASSNAME,
			"",
			WS_CHILD | WS_VISIBLE,
			0,
			0,
			0,
			0,
			hwnd,
			(HMENU)IDC_MAIN_STATUSBAR,
			hThisInstance,
			NULL);

	SendMessage(hstatus, SB_SETPARTS, sizeof(hstatuswidths)/sizeof(int), (LPARAM)hstatuswidths);

	if (!CreateGLTarget(640,480,16))
	{
		return 0;
	}

	sprintf(WindowTitle, "%s %s", AppTitle, AppVersion);
	SetWindowText(hwnd, WindowTitle);

	GetModuleFileName(NULL, AppPath, sizeof(AppPath) - 1);

	char *AppPathTemp = strrchr(AppPath, '\\');
	if(AppPathTemp) ++AppPathTemp; if(AppPathTemp) *AppPathTemp = 0;
	sprintf(INIPath, "%s\\ozmav.ini", AppPath);

	GetPrivateProfileString("Viewer", "LastObject", "", Filename_Obj, sizeof(Filename_Obj), INIPath);
	Renderer_FilteringMode_Min = GetPrivateProfileInt("Viewer", "TexFilterMin", GL_LINEAR_MIPMAP_LINEAR, INIPath);
	Renderer_FilteringMode_Mag = GetPrivateProfileInt("Viewer", "TexFilterMag", GL_LINEAR, INIPath);

	ShowWindow(hwnd, nFunsterStil);

	while(!ExitProgram) {
		if(PeekMessage (&messages, NULL, 0, 0, PM_REMOVE)) {
			if (messages.message == WM_CLOSE) {
				ExitProgram = true;
			} else {
				TranslateMessage(&messages);
				DispatchMessage(&messages);
			}
		} else {
			if (WndActive) {
				if (System_KbdKeys[VK_ESCAPE]) {
					ExitProgram = true;
				}

				if (ObjLoaded) {
					if (System_KbdKeys[VK_F1]) {
						System_KbdKeys[VK_F1] = false;
						if(!(DListInfo_DListToRender == -1)) {
							DListInfo_DListToRender--;
						}
						if(DListInfo_DListToRender == -1) {
							sprintf(StatusMsg, "Entry Point: rendering all");
						} else {
							sprintf(StatusMsg, "Entry Point: #%d", (int)DListInfo_DListToRender + 1);
						}
					}
					if (System_KbdKeys[VK_F2]) {
						System_KbdKeys[VK_F2] = false;
						if(!(DListInfo_DListToRender == DListInfo_CurrentCount)) {
							DListInfo_DListToRender++;
						}
						sprintf(StatusMsg, "Entry Point: #%d", (int)DListInfo_DListToRender + 1);
					}

					if (System_KbdKeys['W']) {
						if(System_KbdKeys[VK_SHIFT]) {
							GLUTCamera_Movement(6);
						} else if(System_KbdKeys[VK_CONTROL]) {
							GLUTCamera_Movement(1);
						} else {
							GLUTCamera_Movement(3);
						}
					}
					if (System_KbdKeys['S']) {
						if(System_KbdKeys[VK_SHIFT]) {
							GLUTCamera_Movement(-6);
						} else if(System_KbdKeys[VK_CONTROL]) {
							GLUTCamera_Movement(-1);
						} else {
							GLUTCamera_Movement(-3);
						}
					}
					if (System_KbdKeys[VK_LEFT]) {
						CamAngleX -= 0.02f;
					}
					if (System_KbdKeys[VK_RIGHT]) {
						CamAngleX += 0.02f;
					}

					if (System_KbdKeys[VK_UP]) {
						CamAngleY += 0.01f;
					}
					if (System_KbdKeys[VK_DOWN]) {
						CamAngleY -= 0.01f;
					}

					if (System_KbdKeys[VK_TAB]) {
						System_KbdKeys[VK_TAB] = false;
						CamAngleX = 0.0f, CamAngleY = 0.0f;
						CamX = 0.0f, CamY = 0.0f, CamZ = 5.0f;
						CamLX = 0.0f, CamLY = 0.0f, CamLZ = -1.0f;
					}

					SendMessage(hstatus, SB_SETTEXT, 1, (LPARAM)Renderer_CoordDisp);
					SendMessage(hstatus, SB_SETTEXT, 2, (LPARAM)StatusMsg);
				}
			}

			GLUTCamera_Orientation(CamAngleX, CamAngleY);

			DrawGLScene();
			SwapBuffers(hDC_ogl);
		}
	}

	WritePrivateProfileString("Viewer", "LastObject", Filename_Obj, INIPath);
	char TempStr[256];
	sprintf(TempStr, "%d", Renderer_FilteringMode_Min);
	WritePrivateProfileString("Viewer", "TexFilterMin", TempStr, INIPath);
	sprintf(TempStr, "%d", Renderer_FilteringMode_Mag);
	WritePrivateProfileString("Viewer", "TexFilterMag", TempStr, INIPath);

	KillGLTarget();
	DestroyWindow(hwnd);

	return (messages.wParam);
}

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_ACTIVATE: {
			if (!HIWORD(wParam)) {
				WndActive = true;
			} else {
				WndActive = false;
			}
			break;
		}
		case WM_SIZE: {
			HWND hogl;
			RECT rcClient;
			GetClientRect(hwnd, &rcClient);
			hogl = GetDlgItem(hwnd, IDC_MAIN_OPENGL);
			SetWindowPos(hogl, NULL, 0, 0, rcClient.right, rcClient.bottom - 25, SWP_NOZORDER);

			RECT rcStatus;
			GetDlgItem(hwnd, IDC_MAIN_STATUSBAR);
			SendMessage(hstatus, WM_SIZE, 0, 0);
			GetWindowRect(hstatus, &rcStatus);

			ReSizeGLScene(rcClient.right, rcClient.bottom);
			DrawGLScene();
			SwapBuffers(hDC_ogl);
			break;
		}
		case WM_COMMAND: {
			switch(LOWORD(wParam))
			{
				case IDM_FILE_OPEN:
					ObjExists = false;
					Dialog_OpenObj(hwnd);
					if(ObjExists) Viewer_Initialize();
					break;
				case IDM_FILE_SAVE:
					break;
				case IDM_FILE_EXIT:
					ExitProgram = true;
					break;
				case IDM_MAP_PREVDLIST:
					System_KbdKeys[VK_F1] = true;
					break;
				case IDM_MAP_NEXTDLIST:
					System_KbdKeys[VK_F2] = true;
					break;
				case IDM_CAMERA_RESETCOORDS:
					System_KbdKeys[VK_TAB] = true;
					break;
				case IDM_OPTIONS_FILTERNEAREST:
					Renderer_FilteringMode_Min = GL_NEAREST;
					Renderer_FilteringMode_Mag = GL_NEAREST;
					Viewer_RenderObject();
					break;
				case IDM_OPTIONS_FILTERLINEAR:
					Renderer_FilteringMode_Min = GL_LINEAR;
					Renderer_FilteringMode_Mag = GL_LINEAR;
					Viewer_RenderObject();
					break;
				case IDM_OPTIONS_FILTERMIPMAP:
					Renderer_FilteringMode_Min = GL_LINEAR_MIPMAP_LINEAR;
					Renderer_FilteringMode_Mag = GL_LINEAR;
					Viewer_RenderObject();
					break;
				case IDM_HELP_ABOUT: ;
					sprintf(GLExtensionsSupported, "OpenGL extensions supported and used:\n");
					if(GLExtension_TextureMirror) sprintf(GLExtensionsSupported, "%sGL_ARB_texture_mirrored_repeat\n", GLExtensionsSupported);
					if(GLExtension_AnisoFilter) sprintf(GLExtensionsSupported, "%sGL_EXT_texture_filter_anisotropic\n", GLExtensionsSupported);

					char AboutMsg[256] = "";
					sprintf(AboutMsg, "%s %s (Build '%s')\nBranch of 'OpenGL Zelda Map Viewer' for Starfox 64\n\nWritten in October/November 2008 by xdaniel & contributors\nhttp://ozmav.googlecode.com/\n\n%s", AppTitle, AppVersion, AppBuildName, GLExtensionsSupported);
					MessageBox(hwnd, AboutMsg, "About", MB_OK | MB_ICONINFORMATION);
					break;
			}
			break;
		}
		case WM_CLOSE: {
			ExitProgram = true;
			break;
		}
		case WM_DESTROY: {
			PostQuitMessage(0);
			break;
		}
		case WM_KEYDOWN: {
			System_KbdKeys[wParam] = true;
			break;
		}
		case WM_KEYUP: {
			System_KbdKeys[wParam] = false;
			break;
		}
		case WM_LBUTTONDOWN: {
			MouseButtonDown = true;
			MouseCenterX = (signed int)LOWORD(lParam);
			MouseCenterY = (signed int)HIWORD(lParam);
			break;
		}
		case WM_LBUTTONUP: {
			MouseButtonDown = false;
			break;
		}
		case WM_MOUSEMOVE: {
			if((MouseButtonDown) && (WndActive)) {
				MousePosX = (signed int)LOWORD(lParam);
				MousePosY = (signed int)HIWORD(lParam);
				Camera_MouseMove(MousePosX, MousePosY);
				GLUTCamera_Orientation(CamAngleX, CamAngleY);
			}
			break;
		}
		default: return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}

/*	------------------------------------------------------------ */
