/*	------------------------------------------------------------
	INCLUDES
	------------------------------------------------------------ */

#include <windows.h>
#include <commctrl.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <math.h>
#include <stdio.h>
#include <stdarg.h>

#include "n64.h"
#include "resource.h"

typedef int bool;
enum { true = 1, false = 0 };

/*	------------------------------------------------------------
	SYSTEM FUNCTIONS - OPENGL & WINDOWS
	------------------------------------------------------------ */

extern int Viewer_Initialize();
extern int Viewer_OpenObject();
extern int Viewer_GetDisplayLists(unsigned long);

extern int Viewer_RenderObject();
extern int Viewer_RenderObject_DListParser(bool, unsigned int, unsigned long);

extern int Viewer_RenderObject_CMDVertexList();
extern int Viewer_GetVertexList(unsigned int, unsigned long, unsigned int, unsigned int);
extern int Viewer_RenderObject_CMDDrawTri1();
extern int Viewer_RenderObject_CMDDrawTri2();
extern int Viewer_RenderObject_CMDTexture();
extern int Viewer_RenderObject_CMDSetTImage();
extern int Viewer_RenderObject_CMDSetTile();
extern int Viewer_RenderObject_CMDSetTileSize();
extern int Viewer_RenderObject_CMDGeometryMode();
extern int Viewer_RenderObject_CMDSetFogColor();
extern int Viewer_RenderObject_CMDSetPrimColor();
extern int Viewer_RenderObject_CMDLoadTLUT(unsigned int, unsigned long);
extern int Viewer_RenderObject_CMDRDPHalf1();

extern GLuint Viewer_LoadTexture();

extern int Viewer_SFMemCopy(unsigned int, unsigned long, unsigned char *, unsigned long);

extern void HelperFunc_SplitCurrentVals(bool);
extern int HelperFunc_LogMessage(int, char[]);
extern int HelperFunc_GFXLogCommand(unsigned int);
extern int HelperFunc_CalculateFPS();

extern int InitGL(void);
extern int InitGLExtensions(void);
extern int DrawGLScene(void);
extern void KillGLTarget(void);
extern bool CreateGLTarget(int, int, int);

extern void GLUTCamera_Orientation(float,float);
extern void GLUTCamera_Movement(int);
extern void Camera_MouseMove(int, int);

extern void Dialog_OpenObj(HWND);

/*	------------------------------------------------------------
	VARIABLES
	------------------------------------------------------------ */

/* WINDOW HANDLING VARIABLES, ETC. */
extern HWND				hwnd;
extern HMENU			hmenu;
extern HWND				hogl;
extern HWND				hstatus;

extern HDC				hDC_ogl;
extern HGLRC			hRC;
extern HINSTANCE		hInstance;

extern char				szClassName[];

/* GENERAL GLOBAL PROGRAM VARIABLES */
extern bool				System_KbdKeys[256];

extern char				AppTitle[256];
extern char				AppVersion[256];
extern char				AppBuildName[256];
extern char				AppPath[512];
extern char				INIPath[512];
extern char				WindowTitle[256];
extern char				StatusMsg[256];
extern char				ErrorMsg[256];

extern bool				ObjLoaded;
extern bool				WndActive;
extern bool				ExitProgram;

extern char				CurrentGFXCmd[256];
extern char				CurrentGFXCmdNote[256];
extern char				GFXLogMsg[256];
extern char				SystemLogMsg[256];

extern bool				GFXLogOpened;

/* CAMERA / VIEWPOINT VARIABLES */
extern float			CamAngleX, CamAngleY;
extern float			CamX, CamY, CamZ;
extern float			CamLX, CamLY, CamLZ;

extern int				MousePosX, MousePosY;
extern int				MouseCenterX, MouseCenterY;

extern bool				MouseButtonDown;

/* FILE HANDLING VARIABLES */
extern FILE				* FileObj;
extern unsigned int		* ObjBuffer;
extern unsigned long	ObjFilesize;
extern char				Filename_Obj[256];
extern bool				ObjExists;

extern FILE				* FileGFXLog;
extern FILE				* FileSystemLog;

/* DATA READOUT VARIABLES */
extern unsigned long	Readout_Current1;
extern unsigned long	Readout_Current2;
extern unsigned int		Readout_CurrentByte1;
extern unsigned int		Readout_CurrentByte2;
extern unsigned int		Readout_CurrentByte3;
extern unsigned int		Readout_CurrentByte4;
extern unsigned int		Readout_CurrentByte5;
extern unsigned int		Readout_CurrentByte6;
extern unsigned int		Readout_CurrentByte7;
extern unsigned int		Readout_CurrentByte8;

extern unsigned long	Readout_NextGFXCommand1 ;

/* F3DZEX DISPLAY LIST HANDLING VARIABLES */
extern unsigned long	DLists[2048];
extern signed long		DListInfo_CurrentCount;
extern signed long		DListInfo_DListToRender;
extern unsigned long	DLTempPosition;

extern unsigned int		DLToRender;
extern bool				DListHasEnded;

extern bool				SubDLCall;

/* F3DZEX TEXTURE HANDLING VARIABLES */
extern unsigned char	* TextureData_OGL;
extern unsigned char	* TextureData_N64;

extern unsigned char	* PaletteData;

/* GENERAL RENDERER VARIABLES */
extern GLuint			Renderer_GLDisplayList;
extern GLuint			Renderer_GLDisplayList_Current;

extern GLuint			Renderer_GLTexture;

extern GLuint			Renderer_FilteringMode_Min;
extern GLuint			Renderer_FilteringMode_Mag;

extern DWORD			Renderer_LastFPS;
extern int				Renderer_FPS, Renderer_FrameNo;
extern char				Renderer_FPSMessage[32];

extern char				Renderer_CoordDisp[256];

extern bool				Renderer_EnableLighting;

extern GLfloat			LightAmbient[];
extern GLfloat			LightDiffuse[];
extern GLfloat			LightPosition[];

extern GLfloat			FogColor[];
extern GLfloat			PrimColor[];

/* OPENGL EXTENSION VARIABLES */
extern char				* GLExtension_List;
extern bool				GLExtension_TextureMirror;
extern bool				GLExtension_AnisoFilter;
extern char				GLExtensionsSupported[256];

extern bool				GLExtensionsUnsupported;
extern char				GLExtensionsErrorMsg[512];

/*	------------------------------------------------------------
	STRUCTURES
	------------------------------------------------------------ */

/* F3DZEX VERTEX DATA STRUCTURE */
struct Vertex_Struct {
	signed short X;
	signed short Y;
	signed short Z;
	signed short H;
	signed short V;
	GLbyte R;
	GLbyte G;
	GLbyte B;
	GLbyte A;
};
extern struct Vertex_Struct Vertex[4096];

/* F3DZEX TEXTURE DATA STRUCTURE */
struct Texture_Struct {
	unsigned int Height;
	unsigned int HeightRender;
	unsigned int Width;
	unsigned int WidthRender;
	unsigned int DataSource;
	unsigned int PalDataSource;
	unsigned long Offset;
	unsigned long PalOffset;
	unsigned int Format_N64;
	GLuint Format_OGL;
	unsigned int Format_OGLPixel;
	unsigned int Y_Parameter;
	unsigned int X_Parameter;
	signed short S_Scale;
	signed short T_Scale;
	unsigned int LineSize;
	unsigned int Palette;
};
extern struct Texture_Struct Texture;

/* CI TEXTURE PALETTE STRUCTURE */
struct Palette_Struct {
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;
};
extern struct Palette_Struct Palette[512];

