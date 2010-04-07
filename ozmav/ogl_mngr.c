/*	------------------------------------------------------------
	OZMAV - OpenGL Zelda Map Viewer

	Written 2008/2009 by xdaniel & contributors
	http://ozmav.googlecode.com/
	------------------------------------------------------------
	ogl_mngr.c - OGL + ext. initialization, etc.
	------------------------------------------------------------ */

#include "globals.h"

/*	------------------------------------------------------------ */

int OGL_Init(void)
{
	OGL_InitExtensions();

	OGL_ResetProperties();

	return true;
}

int OGL_ResetProperties(void)
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(0.2f, 0.5f, 0.7f, 1.0f);
//	glClearColor(FogColor[0], FogColor[1], FogColor[2], FogColor[3]);
	glClearDepth(5.0f);

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
	glEnable(GL_LIGHT0);

	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* fake fog */
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glHint(GL_FOG_HINT, GL_NICEST);
/*	glFogf(GL_FOG_START, 1.0f);
	glFogf(GL_FOG_END, 75.0f);

	glFogfv(GL_FOG_COLOR, FogColor);
*/
	if(GLExtension_FragProgram) {
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, EnvColor[0], EnvColor[1], EnvColor[2], EnvColor[3]);
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 1, PrimColor.R, PrimColor.G, PrimColor.B, PrimColor.A);
	}

	return true;
}

int OGL_InitExtensions(void)
{
	memset(GLExtensionsSupported, 0x00, sizeof(GLExtensionsSupported));

	GLExtension_List = strdup(glGetString(GL_EXTENSIONS));
	int ExtListLen = strlen(GLExtension_List);
	int i;
	for(i = 0; i < ExtListLen; i++) {
		if(GLExtension_List[i] == ' ') GLExtension_List[i] = '\n';
	}

	if(strstr(GLExtension_List, "GL_ARB_multitexture")) {
		GLExtension_MultiTexture = true;
		glMultiTexCoord1fARB		= (PFNGLMULTITEXCOORD1FARBPROC) wglGetProcAddress("glMultiTexCoord1fARB");
		glMultiTexCoord2fARB		= (PFNGLMULTITEXCOORD2FARBPROC) wglGetProcAddress("glMultiTexCoord2fARB");
		glMultiTexCoord3fARB		= (PFNGLMULTITEXCOORD3FARBPROC) wglGetProcAddress("glMultiTexCoord3fARB");
		glMultiTexCoord4fARB		= (PFNGLMULTITEXCOORD4FARBPROC) wglGetProcAddress("glMultiTexCoord4fARB");
		glActiveTextureARB			= (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
		glClientActiveTextureARB	= (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");
		sprintf(GLExtensionsSupported, "%sGL_ARB_multitexture\n", GLExtensionsSupported);
	} else {
		GLExtensionsUnsupported = true;
		sprintf(GLExtensionsErrorMsg, "%sGL_ARB_multitexture\n", GLExtensionsErrorMsg);
	}

	if(strstr(GLExtension_List, "GL_ARB_texture_mirrored_repeat")) {
		GLExtension_TextureMirror = true;
		sprintf(GLExtensionsSupported, "%sGL_ARB_texture_mirrored_repeat\n", GLExtensionsSupported);
	} else {
		GLExtensionsUnsupported = true;
		sprintf(GLExtensionsErrorMsg, "%sGL_ARB_texture_mirrored_repeat\n", GLExtensionsErrorMsg);
	}

	if(strstr(GLExtension_List, "GL_EXT_texture_filter_anisotropic")) {
		GLExtension_AnisoFilter = true;
		sprintf(GLExtensionsSupported, "%sGL_EXT_texture_filter_anisotropic\n", GLExtensionsSupported);
	} else {
		GLExtensionsUnsupported = true;
		sprintf(GLExtensionsErrorMsg, "%sGL_EXT_texture_filter_anisotropic\n", GLExtensionsErrorMsg);
	}

	if(strstr(GLExtension_List, "GL_ARB_fragment_program")) {
		GLExtension_FragProgram = true;
		glGenProgramsARB				= (PFNGLGENPROGRAMSARBPROC) wglGetProcAddress("glGenProgramsARB");
		glBindProgramARB				= (PFNGLBINDPROGRAMARBPROC) wglGetProcAddress("glBindProgramARB");
		glDeleteProgramsARB				= (PFNGLDELETEPROGRAMSARBPROC) wglGetProcAddress("glDeleteProgramsARB");
		glProgramStringARB				= (PFNGLPROGRAMSTRINGARBPROC) wglGetProcAddress("glProgramStringARB");
		glProgramEnvParameter4fARB		= (PFNGLPROGRAMENVPARAMETER4FARBPROC) wglGetProcAddress("glProgramEnvParameter4fARB");
		glProgramLocalParameter4fARB	= (PFNGLPROGRAMLOCALPARAMETER4FARBPROC) wglGetProcAddress("glProgramLocalParameter4fARB");
		sprintf(GLExtensionsSupported, "%sGL_ARB_fragment_program\n", GLExtensionsSupported);
	} else {
		GLExtensionsUnsupported = true;
		sprintf(GLExtensionsErrorMsg, "%sGL_ARB_fragment_program\n", GLExtensionsErrorMsg);
	}

	if(strstr(GLExtension_List, "WGL_EXT_swap_control")) {
		GLExtension_SwapControl = true;
		wglSwapIntervalEXT				= (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");
		wglGetSwapIntervalEXT			= (PFNWGLGETSWAPINTERVALEXTPROC) wglGetProcAddress("wglGetSwapIntervalEXT");
		sprintf(GLExtensionsSupported, "%sWGL_EXT_swap_control\n", GLExtensionsSupported);
	} else {
		GLExtensionsUnsupported = true;
		sprintf(GLExtensionsErrorMsg, "%sWGL_EXT_swap_control\n", GLExtensionsErrorMsg);
	}

	if(GLExtensionsUnsupported) {
		sprintf(ErrorMsg, "The following OpenGL Extensions are not supported by your hardware:\n\n%s", GLExtensionsErrorMsg);
		MessageBox(hwnd, ErrorMsg, "Extension Error", MB_OK | MB_ICONERROR);
	}

	return 0;
}

int OGL_DrawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int i = 0;

	if(AreaLoaded) {
		Helper_CalculateFPS();

		sprintf(Renderer_CoordDisp, "Cam X: %4.2f, Y: %4.2f, Z: %4.2f (%.0f)", CamX, CamY, CamZ, (CamSpeed > 0 ? (CamSpeed / 2) : -(CamSpeed / 2)));

		glLoadIdentity();

		gluLookAt(CamX, CamY, CamZ,
				CamX + CamLX, CamY + CamLY, CamZ + CamLZ,
				0.0f, 1.0f, 0.0f);

		glScalef(0.005, 0.005, 0.005);

		/* #0 - GET COLLISION GL DLIST */
		unsigned int GLCollisionDList = Renderer_GLDisplayList;

		for(i = 0; i < SceneHeader[SceneHeader_Current].Map_Count; i++) {
			int j = 0;
			for(j = 0; j < DListInfo_CurrentCount[i]; j++) {
				//do nothing, just simulating stuff
			}
			GLCollisionDList += j;
		}

		/* #1 - RENDER ACTORS, PASS 1 */
		if(GLExtension_FragProgram) { glDisable(GL_FRAGMENT_PROGRAM_ARB); }

		glDisable(GL_BLEND);
		for(i = 0; i < SceneHeader[SceneHeader_Current].Map_Count; i++) {
			ActorInfo_CurrentCount[i] = 0;
		}
		ScActorInfo_CurrentCount = 0;
		DoorInfo_CurrentCount = 0;
		Viewer_RenderAllActors();

		/* skybox test */
/*		glDisable(GL_FOG);
		glDisable(GL_LIGHTING);
		glCullFace(GL_FRONT);

		glBegin(GL_QUADS);
			glColor3f(0.0f, 0.0f, 0.0f);

			glVertex3f( 15000.0f, 15000.0f, 15000.0f);   //V2
			glVertex3f( 15000.0f,-15000.0f, 15000.0f);   //V1
			glVertex3f( 15000.0f,-15000.0f,-15000.0f);   //V3
			glVertex3f( 15000.0f, 15000.0f,-15000.0f);   //V4

			glVertex3f( 15000.0f, 15000.0f,-15000.0f);   //V4
			glVertex3f( 15000.0f,-15000.0f,-15000.0f);   //V3
			glVertex3f(-15000.0f,-15000.0f,-15000.0f);   //V5
			glVertex3f(-15000.0f, 15000.0f,-15000.0f);   //V6

			glVertex3f(-15000.0f, 15000.0f,-15000.0f);   //V6
			glVertex3f(-15000.0f,-15000.0f,-15000.0f);   //V5
			glVertex3f(-15000.0f,-15000.0f, 15000.0f);   //V7
			glVertex3f(-15000.0f, 15000.0f, 15000.0f);   //V8

			glVertex3f(-15000.0f, 15000.0f,-15000.0f);   //V6
			glVertex3f(-15000.0f, 15000.0f, 15000.0f);   //V8
			glVertex3f( 15000.0f, 15000.0f, 15000.0f);   //V2
			glVertex3f( 15000.0f, 15000.0f,-15000.0f);   //V4

			glVertex3f(-15000.0f,-15000.0f, 15000.0f);   //V7
			glVertex3f(-15000.0f,-15000.0f,-15000.0f);   //V5
			glVertex3f( 15000.0f,-15000.0f,-15000.0f);   //V3
			glVertex3f( 15000.0f,-15000.0f, 15000.0f);   //V1

			glVertex3f(-15000.0f, 15000.0f, 15000.0f);   //V8
			glVertex3f(-15000.0f,-15000.0f, 15000.0f);   //V7
			glVertex3f( 15000.0f,-15000.0f, 15000.0f);   //V1
			glVertex3f( 15000.0f, 15000.0f, 15000.0f);   //V2
		glEnd();
		glEnable(GL_LIGHTING);
		if(Renderer_EnableFog) { glEnable(GL_FOG); } else { glDisable(GL_FOG); }*/
		/* skybox test */

		/* #2 - RENDER MAP */
		Renderer_GLDisplayList_Current = Renderer_GLDisplayList;

		glEnable(GL_BLEND);
		glPolygonMode(GL_FRONT_AND_BACK, (Renderer_EnableWireframe ? GL_LINE : GL_FILL));

		int StartMap = 0, EndMap = 0, SkipDLs = 0;
		if(ROM_CurrentMap == -1) {
			StartMap = 0;
			EndMap = SceneHeader[SceneHeader_Current].Map_Count;
		} else {
			StartMap = ROM_CurrentMap;
			EndMap = ROM_CurrentMap + 1;
		}

		for(i = 0; i < StartMap; i++) {
			SkipDLs += DListInfo_CurrentCount[i];
		}

		for(i = StartMap; i < EndMap; i++) {
			int j = 0;
			for(j = 0; j < DListInfo_CurrentCount[i]; j++) {
				if(Renderer_EnableFog) { glEnable(GL_FOG); } else { glDisable(GL_FOG); }
				if(Renderer_EnableMap) glCallList(Renderer_GLDisplayList_Current + SkipDLs + j);
			}
			Renderer_GLDisplayList_Current += SkipDLs + j;
		}

		/* #3 - RENDER COLLISION */
		if(Renderer_EnableCollision) {
			if(GLExtension_FragProgram) { glDisable(GL_FRAGMENT_PROGRAM_ARB); }

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glAlphaFunc(GL_GREATER, 0.0f);

			glEnable(GL_BLEND);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			if(Renderer_EnableMap) { Renderer_CollisionAlpha = 0.3f; } else { Renderer_CollisionAlpha = 0.1f; }
			glCallList(GLCollisionDList);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glLineWidth(8.5f);
			glCallList(GLCollisionDList);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDisable(GL_BLEND);
		}

		/* #4 - RENDER ACTORS, PASS 2 */
		if(GLExtension_FragProgram) { glDisable(GL_FRAGMENT_PROGRAM_ARB); }

		glDisable(GL_BLEND);
		for(i = 0; i < SceneHeader[SceneHeader_Current].Map_Count; i++) {
			ActorInfo_CurrentCount[i] = 0;
		}
		ScActorInfo_CurrentCount = 0;
		DoorInfo_CurrentCount = 0;
		Viewer_RenderAllActors();
	}

	return true;
}

void OGL_ResizeScene(GLsizei width, GLsizei height)
{
/*	if (height == 0) height = 1;

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(60.0f, (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
*/
	GLfloat aspect, fnear = 0.1f, ffar = 1000000.0f;
	glViewport (0, 0, width, height);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	aspect = ((GLfloat) width ) / ((GLfloat) height );
    GLfloat right, top;
    top = fnear * tan(0.5 * 60.0f * 3.14159265f / 180.0);
    right = aspect * top;
    glFrustum(-right, right, -top, top, fnear, ffar);
	glMatrixMode(GL_MODELVIEW);
}

void OGL_KillTarget(void)
{
	if (hRC)
	{
		if (!wglMakeCurrent(NULL, NULL)) MessageBox(NULL, "Release of DC and RC failed!", "Error", MB_OK | MB_ICONERROR);
		if (!wglDeleteContext(hRC)) MessageBox(NULL, "Release of Rendering Context failed!", "Error", MB_OK | MB_ICONERROR);
		hRC = NULL;
	}

	if (hDC_ogl && !ReleaseDC(hwnd, hDC_ogl))
	{
		MessageBox(NULL, "Release of Device Context failed!", "Error", MB_OK | MB_ICONERROR);
		hDC_ogl = NULL;
	}
}

BOOL OGL_CreateTarget(int width, int height, int bits)
{
	GLuint PixelFormat;

	static PIXELFORMATDESCRIPTOR pfd=
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		0,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0,
		16,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};
	pfd.cColorBits = bits;

	if (!(hDC_ogl = GetDC(hogl)))
	{
		OGL_KillTarget();
		MessageBox(NULL, "Can't create OpenGL Device Context!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!(PixelFormat = ChoosePixelFormat(hDC_ogl, &pfd)))
	{
		OGL_KillTarget();
		MessageBox(NULL, "Can't find suitable PixelFormat!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if(!SetPixelFormat(hDC_ogl, PixelFormat, &pfd))
	{
		OGL_KillTarget();
		MessageBox(NULL,"Can't set PixelFormat!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!(hRC = wglCreateContext(hDC_ogl)))
	{
		OGL_KillTarget();
		MessageBox(NULL, "Can't create OpenGL Rendering Context!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if(!wglMakeCurrent(hDC_ogl, hRC))
	{
		OGL_KillTarget();
		MessageBox(NULL, "Can't activate OpenGL Rendering Context!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!OGL_Init())
	{
		OGL_KillTarget();
		MessageBox(NULL, "Initialization failed!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	return TRUE;
}

/*	------------------------------------------------------------ */
