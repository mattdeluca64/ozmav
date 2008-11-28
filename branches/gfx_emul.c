/*	------------------------------------------------------------
	OZMAV SF64
	 - Branch of 'OpenGL Zelda Map Viewer' for Starfox 64

	Written in October/November 2008 by xdaniel & contributors
	http://ozmav.googlecode.com/
	------------------------------------------------------------
	gfx_emul.c - Display List interpreter & main map renderer
	------------------------------------------------------------ */

#include "globals.h"

/*	------------------------------------------------------------ */

int Viewer_RenderObject()
{
	/* OPEN GFX COMMAND LOG */
	if(!GFXLogOpened) FileGFXLog = fopen("gfxlog.txt", "w"); GFXLogOpened = true;

	/* IF GL DLISTS EXIST, DELETE THEM ALL */
	if(Renderer_GLDisplayList != 0) { glDeleteLists(Renderer_GLDisplayList, 256); }
	/* REGENERATE GL DLISTS */
	Renderer_GLDisplayList = glGenLists(256);

	/* SET GL DLIST BASE */
	glListBase(Renderer_GLDisplayList);

	/* RESET SOME VALUES FOR DLIST READING/BUILDING */
	Renderer_GLDisplayList_Current = Renderer_GLDisplayList;
	DLToRender = 0;

	while(!(Renderer_GLDisplayList_Current == Renderer_GLDisplayList + (DListInfo_CurrentCount + 1))) {
		if(Renderer_GLDisplayList_Current != 0) {
			glNewList(Renderer_GLDisplayList_Current, GL_COMPILE);
				DLTempPosition = DLists[DLToRender] / 4;

				SubDLCall = false;
				Viewer_RenderObject_DListParser(false, DLToRender, DLTempPosition);

				PrimColor[0] = 1.0f;
				PrimColor[1] = 1.0f;
				PrimColor[2] = 1.0f;
				PrimColor[3] = 1.0f;

				Blender_Cycle1 = 0x00;
				Blender_Cycle2 = 0x00;

				DListHasEnded = false;
			glEndList();
		}

		Renderer_GLDisplayList_Current++;
		DLToRender++;
	}

	ObjLoaded = true;

	fclose(FileGFXLog); GFXLogOpened = false;
	fclose(FileSystemLog);

	return 0;
}

/*	------------------------------------------------------------ */

int Viewer_RenderObject_DListParser(bool CalledFromRDPHalf, unsigned int DLToRender, unsigned long Position)
{
	if(CalledFromRDPHalf) {
		sprintf(GFXLogMsg, "  [DList called via RDPHALF_1 (0x%08X)]\n", (unsigned int)Position * 4);
		HelperFunc_LogMessage(1, GFXLogMsg);
	} else {
		sprintf(GFXLogMsg, "Display List #%d (0x%08X):\n", DLToRender + 1, (unsigned int)Position * 4);
		HelperFunc_LogMessage(1, GFXLogMsg);
	}

	while (!DListHasEnded) {
		memcpy(&Readout_Current1, &ObjBuffer[Position], 4);
		memcpy(&Readout_Current2, &ObjBuffer[Position + 1], 4);

		memcpy(&Readout_NextGFXCommand1, &ObjBuffer[Position + 2], 4);

		HelperFunc_SplitCurrentVals(true);

		if(CalledFromRDPHalf) HelperFunc_LogMessage(1, " ");

		switch(Readout_CurrentByte1) {
		case F3D_VTX:
			sprintf(CurrentGFXCmd, "F3D_VTX              ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderObject_CMDVertexList();
			break;
		case F3D_TRI1:
			sprintf(CurrentGFXCmd, "F3D_TRI1             ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderObject_CMDDrawTri1();
			break;
		case F3D_TRI4:
			sprintf(CurrentGFXCmd, "F3D_TRI4             ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderObject_CMDDrawTri2();
			break;
		case F3D_TEXTURE:
			sprintf(CurrentGFXCmd, "F3D_TEXTURE          ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderObject_CMDTexture();
			break;
		case G_SETTIMG:
			sprintf(CurrentGFXCmd, "G_SETTIMG            ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderObject_CMDSetTImage();
			break;
		case G_SETTILE:
			sprintf(CurrentGFXCmd, "G_SETTILE            ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderObject_CMDSetTile();
			break;
		case G_LOADBLOCK:
			sprintf(CurrentGFXCmd, "G_LOADBLOCK          ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			HelperFunc_GFXLogCommand(Position);
			break;
		case G_SETTILESIZE:
			sprintf(CurrentGFXCmd, "G_SETTILESIZE        ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderObject_CMDSetTileSize();
			break;
		case G_RDPFULLSYNC:
			sprintf(CurrentGFXCmd, "G_RDPFULLSYNC        ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			break;
		case G_RDPTILESYNC:
			sprintf(CurrentGFXCmd, "G_RDPTILESYNC        ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			break;
		case G_RDPPIPESYNC:
			sprintf(CurrentGFXCmd, "G_RDPPIPESYNC        ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			break;
		case G_RDPLOADSYNC:
			sprintf(CurrentGFXCmd, "G_RDPLOADSYNC        ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			break;
		case F3D_SETGEOMETRYMODE:
			sprintf(CurrentGFXCmd, "F3D_SETGEOMETRYMODE  ");
			sprintf(CurrentGFXCmdNote, "<partially handled>");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderObject_CMDGeometryMode();
			break;
		case F3D_CULLDL:
			sprintf(CurrentGFXCmd, "F3D_CULLDL           ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			HelperFunc_GFXLogCommand(Position);
			break;
		case F3D_SETOTHERMODE_H:
			sprintf(CurrentGFXCmd, "F3D_SETOTHERMODE_H   ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			HelperFunc_GFXLogCommand(Position);
			break;
		case F3D_SETOTHERMODE_L:
			sprintf(CurrentGFXCmd, "F3D_SETOTHERMODE_L   ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			HelperFunc_GFXLogCommand(Position);
			break;
		case G_SETFOGCOLOR:
			sprintf(CurrentGFXCmd, "G_SETFOGCOLOR        ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderObject_CMDSetFogColor();
			break;
		case G_SETPRIMCOLOR:
			sprintf(CurrentGFXCmd, "G_SETPRIMCOLOR       ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderObject_CMDSetPrimColor();
			break;
		case F3D_RDPHALF_1:
			sprintf(CurrentGFXCmd, "F3D_RDPHALF_1        ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderObject_CMDRDPHalf1();
			break;
		case G_LOADTLUT:
			sprintf(CurrentGFXCmd, "G_LOADTLUT           ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderObject_CMDLoadTLUT(Texture.PalDataSource, Texture.PalOffset);
			break;
		case G_SETCOMBINE:
			sprintf(CurrentGFXCmd, "G_SETCOMBINE         ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			HelperFunc_GFXLogCommand(Position);
			break;
		case F3D_DL:
			sprintf(CurrentGFXCmd, "F3D_DL               ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			HelperFunc_GFXLogCommand(Position);
			break;
		case F3D_ENDDL:
			sprintf(CurrentGFXCmd, "F3D_ENDDL            ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			DListHasEnded = true;
			break;
		default:
			sprintf(CurrentGFXCmd, "<unknown>            ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			break;
		}

		if(DListHasEnded) {
			if(CalledFromRDPHalf) {
				HelperFunc_LogMessage(1, "  [Return to original DList]\n");
			} else {
				HelperFunc_LogMessage(1, "\n");
			}
		}

		Position+=2;
	}

	if(CalledFromRDPHalf) DListHasEnded = false;

	SubDLCall = false;

	return 0;
}

int Viewer_RenderObject_CMDVertexList()
{
	glEnable(GL_TEXTURE_2D);
	Renderer_GLTexture = Viewer_LoadTexture();
	glBindTexture(GL_TEXTURE_2D, Renderer_GLTexture);

	unsigned int TempVertListBank = 0;
	unsigned long TempVertListOffset = 0;
	unsigned int TempVertCount = 0;
	unsigned int TempVertListStartEntry = 0;

	TempVertListBank = Readout_CurrentByte5;
	TempVertListOffset = Readout_CurrentByte6 << 16;
	TempVertListOffset = TempVertListOffset + (Readout_CurrentByte7 << 8);
	TempVertListOffset = TempVertListOffset + Readout_CurrentByte8;
	TempVertCount = ((Readout_CurrentByte3 / 2));

	TempVertListStartEntry = TempVertCount - Readout_CurrentByte2 >> 8;

	Viewer_GetVertexList(TempVertListBank, TempVertListOffset, TempVertCount, TempVertListStartEntry);

	return 0;
}

int Viewer_GetVertexList(unsigned int Bank, unsigned long Offset, unsigned int VertCount, unsigned int TempVertListStartEntry)
{
	unsigned long CurrentVert = TempVertListStartEntry;
	unsigned long VertListPosition = 0;

	unsigned long TempVal1 = 0;
	unsigned short TempVal2 = 0;

	unsigned char * VertListTempBuffer;
	VertListTempBuffer = (unsigned char *) malloc ((VertCount + 1) * 0x10);
	memset(VertListTempBuffer, 0x00, sizeof(VertListTempBuffer));

	if(Viewer_SFMemCopy(Bank, Offset, VertListTempBuffer, ((VertCount + 1) * 0x10)) == -1) {
		sprintf(ErrorMsg, "Invalid Vertex data source 0x%02X!", Bank);
		MessageBox(hwnd, ErrorMsg, "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	while(CurrentVert < VertCount + 1) {
		// X + Y
		memcpy(&TempVal1, &VertListTempBuffer[VertListPosition], 4);
		Vertex[CurrentVert].Y = TempVal1 >> 16 << 8;
		Vertex[CurrentVert].Y = Vertex[CurrentVert].Y + (TempVal1 >> 24);
		Vertex[CurrentVert].X = TempVal1 << 8;
		TempVal2 = (TempVal1 >> 8 << 8);
		TempVal2 = TempVal2 >> 8;
		Vertex[CurrentVert].X = Vertex[CurrentVert].X + TempVal2;
		VertListPosition+=4;

		// Z
		memcpy(&TempVal1, &VertListTempBuffer[VertListPosition], 4);
		Vertex[CurrentVert].Z = TempVal1 << 8;
		TempVal2 = (TempVal1 >> 8 << 8);
		TempVal2 = TempVal2 >> 8;
		Vertex[CurrentVert].Z = Vertex[CurrentVert].Z + TempVal2;
		VertListPosition+=4;

		// H + V
		memcpy(&TempVal1, &VertListTempBuffer[VertListPosition], 4);
		Vertex[CurrentVert].V = TempVal1 >> 16 << 8;
		Vertex[CurrentVert].V = Vertex[CurrentVert].V + (TempVal1 >> 24);
		Vertex[CurrentVert].H = TempVal1 << 8;
		TempVal2 = (TempVal1 >> 8 << 8);
		TempVal2 = TempVal2 >> 8;
		Vertex[CurrentVert].H = Vertex[CurrentVert].H + TempVal2;
		VertListPosition+=4;

		// R, G, B, A
		memcpy(&TempVal1, &VertListTempBuffer[VertListPosition], 4);
		Vertex[CurrentVert].A = TempVal1 >> 24;
		Vertex[CurrentVert].B = TempVal1 >> 16;
		Vertex[CurrentVert].G = TempVal1 >> 8;
		Vertex[CurrentVert].R = TempVal1;
		VertListPosition+=4;

		CurrentVert++;
	}

	return 0;
}

int Viewer_RenderObject_CMDDrawTri1()
{
	signed int CurrentX1_1 = Vertex[Readout_CurrentByte6 / 2].X;	// triangle 1, vertex 1, X
	signed int CurrentY1_1 = Vertex[Readout_CurrentByte6 / 2].Y;	// triangle 1, vertex 1, Y
	signed int CurrentZ1_1 = Vertex[Readout_CurrentByte6 / 2].Z;	// triangle 1, vertex 1, Z
	signed int CurrentX1_2 = Vertex[Readout_CurrentByte7 / 2].X;	// triangle 1, vertex 2, X
	signed int CurrentY1_2 = Vertex[Readout_CurrentByte7 / 2].Y;	// triangle 1, vertex 2, Y
	signed int CurrentZ1_2 = Vertex[Readout_CurrentByte7 / 2].Z;	// triangle 1, vertex 2, Z
	signed int CurrentX1_3 = Vertex[Readout_CurrentByte8 / 2].X;	// triangle 1, vertex 3, X
	signed int CurrentY1_3 = Vertex[Readout_CurrentByte8 / 2].Y;	// triangle 1, vertex 3, Y
	signed int CurrentZ1_3 = Vertex[Readout_CurrentByte8 / 2].Z;	// triangle 1, vertex 3, Z

	GLbyte CurrentR1_1 = Vertex[Readout_CurrentByte6 / 2].R;	// triangle 1, vertex 1, R
	GLbyte CurrentG1_1 = Vertex[Readout_CurrentByte6 / 2].G;	// triangle 1, vertex 1, G
	GLbyte CurrentB1_1 = Vertex[Readout_CurrentByte6 / 2].B;	// triangle 1, vertex 1, B
	GLbyte CurrentA1_1 = Vertex[Readout_CurrentByte6 / 2].A;	// triangle 1, vertex 1, A
	GLbyte CurrentR1_2 = Vertex[Readout_CurrentByte7 / 2].R;	// triangle 1, vertex 2, R
	GLbyte CurrentG1_2 = Vertex[Readout_CurrentByte7 / 2].G;	// triangle 1, vertex 2, G
	GLbyte CurrentB1_2 = Vertex[Readout_CurrentByte7 / 2].B;	// triangle 1, vertex 2, B
	GLbyte CurrentA1_2 = Vertex[Readout_CurrentByte7 / 2].A;	// triangle 1, vertex 2, A
	GLbyte CurrentR1_3 = Vertex[Readout_CurrentByte8 / 2].R;	// triangle 1, vertex 3, R
	GLbyte CurrentG1_3 = Vertex[Readout_CurrentByte8 / 2].G;	// triangle 1, vertex 3, G
	GLbyte CurrentB1_3 = Vertex[Readout_CurrentByte8 / 2].B;	// triangle 1, vertex 3, B
	GLbyte CurrentA1_3 = Vertex[Readout_CurrentByte8 / 2].A;	// triangle 1, vertex 3, A

	signed short CurrentH1_1 = Vertex[Readout_CurrentByte6 / 2].H;
	signed short CurrentH1_2 = Vertex[Readout_CurrentByte7 / 2].H;
	signed short CurrentH1_3 = Vertex[Readout_CurrentByte8 / 2].H;
	signed short CurrentV1_1 = Vertex[Readout_CurrentByte6 / 2].V;
	signed short CurrentV1_2 = Vertex[Readout_CurrentByte7 / 2].V;
	signed short CurrentV1_3 = Vertex[Readout_CurrentByte8 / 2].V;

	float TempU = 0;
	float TempV = 0;

	glBegin(GL_TRIANGLES);
		glColor4f(PrimColor[0], PrimColor[1], PrimColor[2], PrimColor[3]);

		TempU = (float) CurrentH1_1 * Texture.S_Scale / 32 / Texture.WidthRender;
		TempV = (float) CurrentV1_1 * Texture.T_Scale / 32 / Texture.HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR1_1, CurrentG1_1, CurrentB1_1, CurrentA1_1); }
		glNormal3f (CurrentR1_1 / 255.0f, CurrentG1_1 / 255.0f, CurrentB1_1 / 255.0f);
		glVertex3d(CurrentX1_1, CurrentY1_1, CurrentZ1_1);

		TempU = (float) CurrentH1_2 * Texture.S_Scale / 32 / Texture.WidthRender;
		TempV = (float) CurrentV1_2 * Texture.T_Scale / 32 / Texture.HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR1_2, CurrentG1_2, CurrentB1_2, CurrentA1_2); }
		glNormal3f (CurrentR1_2 / 255.0f, CurrentG1_2 / 255.0f, CurrentB1_2 / 255.0f);
		glVertex3d(CurrentX1_2, CurrentY1_2, CurrentZ1_2);

		TempU = (float) CurrentH1_3 * Texture.S_Scale / 32 / Texture.WidthRender;
		TempV = (float) CurrentV1_3 * Texture.T_Scale / 32 / Texture.HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR1_3, CurrentG1_3, CurrentB1_3, CurrentA1_3); }
		glNormal3f (CurrentR1_3 / 255.0f, CurrentG1_3 / 255.0f, CurrentB1_3 / 255.0f);
		glVertex3d(CurrentX1_3, CurrentY1_3, CurrentZ1_3);
	glEnd();

	return 0;
}

int Viewer_RenderObject_CMDDrawTri2()
{
	signed int CurrentX1_1 = Vertex[Readout_CurrentByte2 / 2].X;	// triangle 1, vertex 1, X
	signed int CurrentY1_1 = Vertex[Readout_CurrentByte2 / 2].Y;	// triangle 1, vertex 1, Y
	signed int CurrentZ1_1 = Vertex[Readout_CurrentByte2 / 2].Z;	// triangle 1, vertex 1, Z
	signed int CurrentX1_2 = Vertex[Readout_CurrentByte3 / 2].X;	// triangle 1, vertex 2, X
	signed int CurrentY1_2 = Vertex[Readout_CurrentByte3 / 2].Y;	// triangle 1, vertex 2, Y
	signed int CurrentZ1_2 = Vertex[Readout_CurrentByte3 / 2].Z;	// triangle 1, vertex 2, Z
	signed int CurrentX1_3 = Vertex[Readout_CurrentByte4 / 2].X;	// triangle 1, vertex 3, X
	signed int CurrentY1_3 = Vertex[Readout_CurrentByte4 / 2].Y;	// triangle 1, vertex 3, Y
	signed int CurrentZ1_3 = Vertex[Readout_CurrentByte4 / 2].Z;	// triangle 1, vertex 3, Z

	GLbyte CurrentR1_1 = Vertex[Readout_CurrentByte2 / 2].R;	// triangle 1, vertex 1, R
	GLbyte CurrentG1_1 = Vertex[Readout_CurrentByte2 / 2].G;	// triangle 1, vertex 1, G
	GLbyte CurrentB1_1 = Vertex[Readout_CurrentByte2 / 2].B;	// triangle 1, vertex 1, B
	GLbyte CurrentA1_1 = Vertex[Readout_CurrentByte2 / 2].A;	// triangle 1, vertex 1, A
	GLbyte CurrentR1_2 = Vertex[Readout_CurrentByte3 / 2].R;	// triangle 1, vertex 2, R
	GLbyte CurrentG1_2 = Vertex[Readout_CurrentByte3 / 2].G;	// triangle 1, vertex 2, G
	GLbyte CurrentB1_2 = Vertex[Readout_CurrentByte3 / 2].B;	// triangle 1, vertex 2, B
	GLbyte CurrentA1_2 = Vertex[Readout_CurrentByte3 / 2].A;	// triangle 1, vertex 2, A
	GLbyte CurrentR1_3 = Vertex[Readout_CurrentByte4 / 2].R;	// triangle 1, vertex 3, R
	GLbyte CurrentG1_3 = Vertex[Readout_CurrentByte4 / 2].G;	// triangle 1, vertex 3, G
	GLbyte CurrentB1_3 = Vertex[Readout_CurrentByte4 / 2].B;	// triangle 1, vertex 3, B
	GLbyte CurrentA1_3 = Vertex[Readout_CurrentByte4 / 2].A;	// triangle 1, vertex 3, A

	signed short CurrentH1_1 = Vertex[Readout_CurrentByte2 / 2].H;
	signed short CurrentH1_2 = Vertex[Readout_CurrentByte3 / 2].H;
	signed short CurrentH1_3 = Vertex[Readout_CurrentByte4 / 2].H;
	signed short CurrentV1_1 = Vertex[Readout_CurrentByte2 / 2].V;
	signed short CurrentV1_2 = Vertex[Readout_CurrentByte3 / 2].V;
	signed short CurrentV1_3 = Vertex[Readout_CurrentByte4 / 2].V;

	signed int CurrentX2_1 = Vertex[Readout_CurrentByte6 / 2].X;	// triangle 2, vertex 1, X
	signed int CurrentY2_1 = Vertex[Readout_CurrentByte6 / 2].Y;	// triangle 2, vertex 1, Y
	signed int CurrentZ2_1 = Vertex[Readout_CurrentByte6 / 2].Z;	// triangle 2, vertex 1, Z
	signed int CurrentX2_2 = Vertex[Readout_CurrentByte7 / 2].X;	// triangle 2, vertex 2, X
	signed int CurrentY2_2 = Vertex[Readout_CurrentByte7 / 2].Y;	// triangle 2, vertex 2, Y
	signed int CurrentZ2_2 = Vertex[Readout_CurrentByte7 / 2].Z;	// triangle 2, vertex 2, Z
	signed int CurrentX2_3 = Vertex[Readout_CurrentByte8 / 2].X;	// triangle 2, vertex 3, X
	signed int CurrentY2_3 = Vertex[Readout_CurrentByte8 / 2].Y;	// triangle 2, vertex 3, Y
	signed int CurrentZ2_3 = Vertex[Readout_CurrentByte8 / 2].Z;	// triangle 2, vertex 3, Z

	GLbyte CurrentR2_1 = Vertex[Readout_CurrentByte6 / 2].R;	// triangle 2, vertex 1, R
	GLbyte CurrentG2_1 = Vertex[Readout_CurrentByte6 / 2].G;	// triangle 2, vertex 1, G
	GLbyte CurrentB2_1 = Vertex[Readout_CurrentByte6 / 2].B;	// triangle 2, vertex 1, B
	GLbyte CurrentA2_1 = Vertex[Readout_CurrentByte6 / 2].A;	// triangle 2, vertex 1, A
	GLbyte CurrentR2_2 = Vertex[Readout_CurrentByte7 / 2].R;	// triangle 2, vertex 2, R
	GLbyte CurrentG2_2 = Vertex[Readout_CurrentByte7 / 2].G;	// triangle 2, vertex 2, G
	GLbyte CurrentB2_2 = Vertex[Readout_CurrentByte7 / 2].B;	// triangle 2, vertex 2, B
	GLbyte CurrentA2_2 = Vertex[Readout_CurrentByte7 / 2].A;	// triangle 2, vertex 2, A
	GLbyte CurrentR2_3 = Vertex[Readout_CurrentByte8 / 2].R;	// triangle 2, vertex 3, R
	GLbyte CurrentG2_3 = Vertex[Readout_CurrentByte8 / 2].G;	// triangle 2, vertex 3, G
	GLbyte CurrentB2_3 = Vertex[Readout_CurrentByte8 / 2].B;	// triangle 2, vertex 3, B
	GLbyte CurrentA2_3 = Vertex[Readout_CurrentByte8 / 2].A;	// triangle 2, vertex 3, A

	signed short CurrentH2_1 = Vertex[Readout_CurrentByte6 / 2].H;
	signed short CurrentH2_2 = Vertex[Readout_CurrentByte7 / 2].H;
	signed short CurrentH2_3 = Vertex[Readout_CurrentByte8 / 2].H;
	signed short CurrentV2_1 = Vertex[Readout_CurrentByte6 / 2].V;
	signed short CurrentV2_2 = Vertex[Readout_CurrentByte7 / 2].V;
	signed short CurrentV2_3 = Vertex[Readout_CurrentByte8 / 2].V;

	float TempU = 0;
	float TempV = 0;

	glBegin(GL_TRIANGLES);
		glColor4f(PrimColor[0], PrimColor[1], PrimColor[2], PrimColor[3]);

		TempU = (float) CurrentH1_1 * Texture.S_Scale / 32 / Texture.WidthRender;
		TempV = (float) CurrentV1_1 * Texture.T_Scale / 32 / Texture.HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR1_1, CurrentG1_1, CurrentB1_1, CurrentA1_1); }
		glNormal3f (CurrentR1_1 / 255.0f, CurrentG1_1 / 255.0f, CurrentB1_1 / 255.0f);
		glVertex3d(CurrentX1_1, CurrentY1_1, CurrentZ1_1);

		TempU = (float) CurrentH1_2 * Texture.S_Scale / 32 / Texture.WidthRender;
		TempV = (float) CurrentV1_2 * Texture.T_Scale / 32 / Texture.HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR1_2, CurrentG1_2, CurrentB1_2, CurrentA1_2); }
		glNormal3f (CurrentR1_2 / 255.0f, CurrentG1_2 / 255.0f, CurrentB1_2 / 255.0f);
		glVertex3d(CurrentX1_2, CurrentY1_2, CurrentZ1_2);

		TempU = (float) CurrentH1_3 * Texture.S_Scale / 32 / Texture.WidthRender;
		TempV = (float) CurrentV1_3 * Texture.T_Scale / 32 / Texture.HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR1_3, CurrentG1_3, CurrentB1_3, CurrentA1_3); }
		glNormal3f (CurrentR1_3 / 255.0f, CurrentG1_3 / 255.0f, CurrentB1_3 / 255.0f);
		glVertex3d(CurrentX1_3, CurrentY1_3, CurrentZ1_3);

		TempU = (float) CurrentH2_1 * Texture.S_Scale / 32 / Texture.WidthRender;
		TempV = (float) CurrentV2_1 * Texture.T_Scale / 32 / Texture.HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR2_1, CurrentG2_1, CurrentB2_1, CurrentA2_1); }
		glNormal3f (CurrentR2_1 / 255.0f, CurrentG2_1 / 255.0f, CurrentB2_1 / 255.0f);
		glVertex3d(CurrentX2_1, CurrentY2_1, CurrentZ2_1);

		TempU = (float) CurrentH2_2 * Texture.S_Scale / 32 / Texture.WidthRender;
		TempV = (float) CurrentV2_2 * Texture.T_Scale / 32 / Texture.HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR2_2, CurrentG2_2, CurrentB2_2, CurrentA2_2); }
		glNormal3f (CurrentR2_2 / 255.0f, CurrentG2_2 / 255.0f, CurrentB2_2 / 255.0f);
		glVertex3d(CurrentX2_2, CurrentY2_2, CurrentZ2_2);

		TempU = (float) CurrentH2_3 * Texture.S_Scale / 32 / Texture.WidthRender;
		TempV = (float) CurrentV2_3 * Texture.T_Scale / 32 / Texture.HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR2_3, CurrentG2_3, CurrentB2_3, CurrentA2_3); }
		glNormal3f (CurrentR2_3 / 255.0f, CurrentG2_3 / 255.0f, CurrentB2_3 / 255.0f);
		glVertex3d(CurrentX2_3, CurrentY2_3, CurrentZ2_3);
	glEnd();

	return 0;
}

/* Viewer_RenderObject_CMDTEXTURE - F3DEX2_TEXTURE */
int Viewer_RenderObject_CMDTexture()
{
	Texture.S_Scale = ((Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6) + 2;
	Texture.T_Scale = ((Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8) + 2;

	return 0;
}

int Viewer_RenderObject_CMDSetTImage()
{
	switch(Readout_NextGFXCommand1) {
		case 0x000000E8:
			Texture.PalDataSource = Readout_CurrentByte5;

			Texture.PalOffset = Readout_CurrentByte6 << 16;
			Texture.PalOffset = Texture.PalOffset + (Readout_CurrentByte7 << 8);
			Texture.PalOffset = Texture.PalOffset + Readout_CurrentByte8;

			Texture.DataSource = Texture.PalDataSource;
			Texture.Offset = Texture.PalOffset;
			break;
		default:
			Texture.DataSource = Readout_CurrentByte5;

			Texture.Offset = Readout_CurrentByte6 << 16;
			Texture.Offset = Texture.Offset + (Readout_CurrentByte7 << 8);
			Texture.Offset = Texture.Offset + Readout_CurrentByte8;
			break;
	}

	return 0;
}

int Viewer_RenderObject_CMDSetTile()
{
	/* FIX FOR CI TEXTURES - IF BYTES 5-8 ARE 07000000, LEAVE CURRENT PROPERTY SETTINGS ALONE */
	if(Readout_Current2 == 0x00000007) return 0;

	unsigned char TempXParameter = Readout_CurrentByte7 * 0x10;

	switch(Readout_CurrentByte6) {
		case 0x01:
			Texture.Y_Parameter = 1;
			break;
		case 0x09:
			Texture.Y_Parameter = 2;
			break;
		case 0x05:
			Texture.Y_Parameter = 3;
			break;
		default:
			Texture.Y_Parameter = 1;
			break;
	}

	switch(TempXParameter) {
		case 0x00:
			Texture.X_Parameter = 1;
			break;
		case 0x20:
			Texture.X_Parameter = 2;
			break;
		case 0x10:
			Texture.X_Parameter = 3;
			break;
		default:
			Texture.X_Parameter = 1;
			break;
	}

	Texture.LineSize = Readout_CurrentByte3 / 2;
	Texture.Format_N64 = Readout_CurrentByte2;
	Texture.Palette = (Readout_Current1 >> 20) & 0x0F;
	Texture.Format_OGL = GL_RGBA;
	Texture.Format_OGLPixel = GL_RGBA;

	return 0;
}

int Viewer_RenderObject_CMDSetTileSize()
{
	if(!(Readout_Current1 == 0x000000F2)) return 0;

	unsigned int TileSize_Temp1 = (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
	unsigned int TileSize_Temp2 = (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

	unsigned int ULS = (TileSize_Temp1 & 0xFFF000) >> 14;
	unsigned int ULT = (TileSize_Temp1 & 0x000FFF) >> 2;
	unsigned int LRS = (TileSize_Temp2 & 0xFFF000) >> 14;
	unsigned int LRT = (TileSize_Temp2 & 0x000FFF) >> 2;

	Texture.Width  = ((LRS - ULS) + 1);
	Texture.Height = ((LRT - ULT) + 1);

	if(Texture.Width > 256) {
		Texture.WidthRender = Texture.Width - 64;
	} else {
		Texture.WidthRender = Texture.Width;
	}

	if(Texture.Height > 256) {
		Texture.HeightRender = Texture.Height - 64;
	} else {
		Texture.HeightRender = Texture.Height;
	}

	Texture.S_Scale = 1;
	Texture.T_Scale = 1;

	return 0;
}

int Viewer_RenderObject_CMDGeometryMode()
{
	int Convert;
	int Counter, Counter2;
	bool Binary[4];

	/* ---- FOG, LIGHTING, AUTOMATIC TEXTURE MAPPING (SPHERICAL + LINEAR) ---- */
	Convert = Readout_CurrentByte2;
	Counter = 0, Counter2 = 4;
	for(Counter = 0; Counter < 4; Counter++) {
		Binary[Counter] = Convert % 2;
		Convert = Convert / 2;
	}
	if(!Binary[0]) {
		/*disable fog*/ }
		glDisable(GL_FOG);
	if(!Binary[1]) {
		/*disable lighting*/
		glDisable(GL_LIGHTING); glDisable(GL_NORMALIZE); Renderer_EnableLighting = false; }
	if(!Binary[2]) {
		/*disable tex-mapping spherical*/ }
	if(!Binary[3]) {
		/*disable tex-mapping linear*/ }

	Convert = Readout_CurrentByte6;
	Counter = 0, Counter2 = 4;
	for(Counter = 0; Counter < 4; Counter++) {
		Binary[Counter] = Convert % 2;
		Convert = Convert / 2;
	}
	if(Binary[0]) {
		/*enable fog*/ }
//		glEnable(GL_FOG);
	if(Binary[1]) {
		/*enable lighting*/
		glEnable(GL_LIGHTING); glEnable(GL_NORMALIZE); Renderer_EnableLighting = true; }
	if(Binary[2]) {
		/*enable tex-mapping spherical*/ }
	if(Binary[3]) {
		/*disable tex-mapping linear*/ }

	/* ---- FACE CULLING ---- */
	Convert = Readout_CurrentByte3;
	Counter = 0, Counter2 = 4;
	for(Counter = 0; Counter < 4; Counter++) {
		Binary[Counter] = Convert % 2;
		Convert = Convert / 2;
	}
	if(!Binary[0]) {
		/*UNUSED?*/ }
	if(!Binary[1]) {
		/*disable front-face culling*/
		glDisable(GL_CULL_FACE); }
	if(!Binary[2]) {
		/*disable back-face culling*/
		glDisable(GL_CULL_FACE); }
	if(!Binary[3]) {
		/*UNUSED?*/ }

	Convert = Readout_CurrentByte7;
	Counter = 0, Counter2 = 4;
	for(Counter = 0; Counter < 4; Counter++) {
		Binary[Counter] = Convert % 2;
		Convert = Convert / 2;
	}
	if(Binary[0]) {
		/*UNUSED?*/ }
	if(Binary[1]) {
		/*enable front-face culling*/
		glEnable(GL_CULL_FACE); glCullFace(GL_FRONT); }
	if(Binary[2]) {
		/*enable back-face culling*/
		glEnable(GL_CULL_FACE); glCullFace(GL_BACK); }
	if(Binary[3]) {
		/*UNUSED?*/ }

	/* ---- VERTEX COLOR SHADING, Z-BUFFERING ---- */
	Convert = Readout_CurrentByte4;
	Counter = 0, Counter2 = 4;
	for(Counter = 0; Counter < 4; Counter++) {
		Binary[Counter] = Convert % 2;
		Convert = Convert / 2;
	}
	if(!Binary[0]) {
		/*UNUSED?*/ }
	if(!Binary[1]) {
		/*disable vertex color shading*/ }
	if(!Binary[2]) {
		/*disable z-buffering*/
		glDisable(GL_DEPTH_TEST); }
	if(!Binary[3]) {
		/*UNUSED?*/ }

	Convert = Readout_CurrentByte8;
	Counter = 0, Counter2 = 4;
	for(Counter = 0; Counter < 4; Counter++) {
		Binary[Counter] = Convert % 2;
		Convert = Convert / 2;
	}
	if(Binary[0]) {
		/*UNUSED?*/ }
	if(Binary[1]) {
		/*enable vertex color shading*/ }
	if(Binary[2]) {
		/*enable z-buffering*/
		glEnable(GL_DEPTH_TEST); }
	if(Binary[3]) {
		/*UNUSED?*/ }

	return 0;
}

int Viewer_RenderObject_CMDSetFogColor()
{
	FogColor[0] = (Readout_CurrentByte5 / 255.0f);
	FogColor[1] = (Readout_CurrentByte6 / 255.0f);
	FogColor[2] = (Readout_CurrentByte7 / 255.0f);
	FogColor[3] = (Readout_CurrentByte8 / 255.0f);

	glFogfv(GL_FOG_COLOR, FogColor);

	return 0;
}

int Viewer_RenderObject_CMDSetPrimColor()
{
	PrimColor[0] = (Readout_CurrentByte5 / 255.0f);
	PrimColor[1] = (Readout_CurrentByte6 / 255.0f);
	PrimColor[2] = (Readout_CurrentByte7 / 255.0f);
	PrimColor[3] = (Readout_CurrentByte8 / 255.0f);

	return 0;
}

int Viewer_RenderObject_CMDLoadTLUT(unsigned int PaletteSrc, unsigned long PaletteOffset)
{
	/* clear the palette buffer */
	memset(PaletteData, 0x00, sizeof(PaletteData));

	/* calculate palette size, 16 or 256, from parameters */
	unsigned int TempPaletteSize = (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
	unsigned int PaletteSize = ((TempPaletteSize & 0xFFF000) >> 14) + 1;

	/* copy raw palette into buffer */
	if(Viewer_SFMemCopy(PaletteSrc, PaletteOffset, PaletteData, PaletteSize * 2) == -1) {
		sprintf(ErrorMsg, "Invalid palette data source 0x%02X!", PaletteSrc);
		MessageBox(hwnd, ErrorMsg, "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	/* initialize variables for palette conversion */
	unsigned int CurrentPaletteEntry = 0;
	unsigned int SourcePaletteData = 0;
	unsigned int InPalettePosition = 0;

	unsigned int PalLoop = 0;

	unsigned int RExtract = 0;
	unsigned int GExtract = 0;
	unsigned int BExtract = 0;
	unsigned int AExtract = 0;

	for(PalLoop = 0; PalLoop < PaletteSize; PalLoop++) {
		/* get raw rgba5551 data */
		SourcePaletteData = (PaletteData[InPalettePosition] * 0x100) + PaletteData[InPalettePosition + 1];

		/* extract r, g, b and a elements */
		RExtract = (SourcePaletteData & 0xF800);
		RExtract >>= 8;
		GExtract = (SourcePaletteData & 0x07C0);
		GExtract <<= 5;
		GExtract >>= 8;
		BExtract = (SourcePaletteData & 0x003E);
		BExtract <<= 18;
		BExtract >>= 16;

		if((SourcePaletteData & 0x0001)) { AExtract = 0xFF; } else { AExtract = 0x00; }

		/* set the current ci palette index to the rgba values from above */
		Palette[CurrentPaletteEntry].R = RExtract;
		Palette[CurrentPaletteEntry].G = GExtract;
		Palette[CurrentPaletteEntry].B = BExtract;
		Palette[CurrentPaletteEntry].A = AExtract;

		/* go on */
		CurrentPaletteEntry++;
		InPalettePosition += 2;
	}

	return 0;
}

int Viewer_RenderObject_CMDRDPHalf1()
{
	if(Readout_CurrentByte5 == 0x03) {
		unsigned long TempOffset;

		TempOffset = Readout_CurrentByte6 << 16;
		TempOffset = TempOffset + (Readout_CurrentByte7 << 8);
		TempOffset = TempOffset + Readout_CurrentByte8;

		SubDLCall = true;
		Viewer_RenderObject_DListParser(true, 0, TempOffset / 4);
	}

	return 0;
}

GLuint Viewer_LoadTexture()
{
	if((Readout_NextGFXCommand1 == 0x00000003) || (Readout_NextGFXCommand1 == 0x000000E1)) return 0;

	GLuint GLTexture;

	int i, j = 0;

	unsigned long TextureBufferSize = 0x8000;

	bool UnhandledTextureSource = false;

	TextureData_OGL = (unsigned char *) malloc (TextureBufferSize);
	TextureData_N64 = (unsigned char *) malloc (TextureBufferSize);

	memset(TextureData_OGL, 0x00, TextureBufferSize);
	memset(TextureData_N64, 0x00, TextureBufferSize);

	/* create solid red texture for unsupported stuff, such as kakariko windows */
	unsigned char * EmptyTexture_Red;
	unsigned char * EmptyTexture_Green;
	EmptyTexture_Red = (unsigned char *) malloc (TextureBufferSize);
	EmptyTexture_Green = (unsigned char *) malloc (TextureBufferSize);
	for(i = 0; i < TextureBufferSize; i+=4) {
		EmptyTexture_Red[i]			= 0xFF;
		EmptyTexture_Red[i + 1]		= 0x00;
		EmptyTexture_Red[i + 2]		= 0x00;
		EmptyTexture_Red[i + 3]		= 0xFF;

		EmptyTexture_Green[i]		= 0x00;
		EmptyTexture_Green[i + 1]	= 0xFF;
		EmptyTexture_Green[i + 2]	= 0x00;
		EmptyTexture_Green[i + 3]	= 0xFF;
	}

	if(Viewer_SFMemCopy(Texture.DataSource, Texture.Offset, TextureData_N64, TextureBufferSize) == -1) {
		UnhandledTextureSource = true;
		sprintf(ErrorMsg, "Unhandled texture source 0x%02X|%06X!", Texture.DataSource, Texture.Offset);
		MessageBox(hwnd, ErrorMsg, "Error", MB_OK | MB_ICONERROR);
		Texture.Format_OGL = GL_RGBA;
		Texture.Format_OGLPixel = GL_RGBA;
		memcpy(TextureData_OGL, EmptyTexture_Red, TextureBufferSize);
	}

	if(!UnhandledTextureSource) {
		switch(Texture.Format_N64) {
		/* RGBA FORMAT */
		case 0x00:
		case 0x08:
		case 0x10:
			{
			unsigned int LoadRGBA_RGBA5551 = 0;

			unsigned int LoadRGBA_RExtract = 0;
			unsigned int LoadRGBA_GExtract = 0;
			unsigned int LoadRGBA_BExtract = 0;
			unsigned int LoadRGBA_AExtract = 0;

			unsigned int LoadRGBA_InTexturePosition_N64 = 0;
			unsigned int LoadRGBA_InTexturePosition_OGL = 0;

			for(j = 0; j < Texture.Height; j++) {
				for(i = 0; i < Texture.Width; i++) {
					LoadRGBA_RGBA5551 = (TextureData_N64[LoadRGBA_InTexturePosition_N64] * 0x100) + TextureData_N64[LoadRGBA_InTexturePosition_N64 + 1];

					LoadRGBA_RExtract = (LoadRGBA_RGBA5551 & 0xF800);
					LoadRGBA_RExtract >>= 8;
					LoadRGBA_GExtract = (LoadRGBA_RGBA5551 & 0x07C0);
					LoadRGBA_GExtract <<= 5;
					LoadRGBA_GExtract >>= 8;
					LoadRGBA_BExtract = (LoadRGBA_RGBA5551 & 0x003E);
					LoadRGBA_BExtract <<= 18;
					LoadRGBA_BExtract >>= 16;

					if((LoadRGBA_RGBA5551 & 0x0001)) {
						LoadRGBA_AExtract = 0xFF;
					} else {
						LoadRGBA_AExtract = 0x00;
					}

					TextureData_OGL[LoadRGBA_InTexturePosition_OGL]     = LoadRGBA_RExtract;
					TextureData_OGL[LoadRGBA_InTexturePosition_OGL + 1] = LoadRGBA_GExtract;
					TextureData_OGL[LoadRGBA_InTexturePosition_OGL + 2] = LoadRGBA_BExtract;
					TextureData_OGL[LoadRGBA_InTexturePosition_OGL + 3] = LoadRGBA_AExtract;

					LoadRGBA_InTexturePosition_N64 += 2;
					LoadRGBA_InTexturePosition_OGL += 4;
				}
				LoadRGBA_InTexturePosition_N64 += Texture.LineSize * 4 - Texture.Width;
			}

			break;
			}
		/* CI FORMAT */
		case 0x40:
		case 0x50:
			{
			unsigned int LoadCI_CIData1 = 0;
			unsigned int LoadCI_CIData2 = 0;

			unsigned int LoadCI_InTexturePosition_N64 = 0;
			unsigned int LoadCI_InTexturePosition_OGL = 0;

			for(j = 0; j < Texture.Height; j++) {
				for(i = 0; i < Texture.Width / 2; i++) {
					LoadCI_CIData1 = (TextureData_N64[LoadCI_InTexturePosition_N64] & 0xF0) >> 4;
					LoadCI_CIData2 = TextureData_N64[LoadCI_InTexturePosition_N64] & 0x0F;

					TextureData_OGL[LoadCI_InTexturePosition_OGL]     = Palette[LoadCI_CIData1].R;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 1] = Palette[LoadCI_CIData1].G;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 2] = Palette[LoadCI_CIData1].B;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 3] = Palette[LoadCI_CIData1].A;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 4] = Palette[LoadCI_CIData2].R;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 5] = Palette[LoadCI_CIData2].G;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 6] = Palette[LoadCI_CIData2].B;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 7] = Palette[LoadCI_CIData2].A;

					LoadCI_InTexturePosition_N64 += 1;
					LoadCI_InTexturePosition_OGL += 8;
				}
				LoadCI_InTexturePosition_N64 += Texture.LineSize * 8 - (Texture.Width / 2);
			}

			break;
			}
		case 0x48:
			{
			unsigned int LoadCI_CIData = 0;

			unsigned int LoadCI_InTexturePosition_N64 = 0;
			unsigned int LoadCI_InTexturePosition_OGL = 0;

			for(j = 0; j < Texture.Height; j++) {
				for(i = 0; i < Texture.Width; i++) {
					LoadCI_CIData = TextureData_N64[LoadCI_InTexturePosition_N64];

					TextureData_OGL[LoadCI_InTexturePosition_OGL]     = Palette[LoadCI_CIData].R;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 1] = Palette[LoadCI_CIData].G;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 2] = Palette[LoadCI_CIData].B;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 3] = Palette[LoadCI_CIData].A;

					LoadCI_InTexturePosition_N64 += 1;
					LoadCI_InTexturePosition_OGL += 4;
				}
				LoadCI_InTexturePosition_N64 += Texture.LineSize * 8 - Texture.Width;
			}

			break;
			}
		/* IA FORMAT */
		case 0x60:
			{
			unsigned int LoadIA_IAData = 0;

			unsigned int LoadIA_IExtract1 = 0;
			unsigned int LoadIA_AExtract1 = 0;
			unsigned int LoadIA_IExtract2 = 0;
			unsigned int LoadIA_AExtract2 = 0;

			unsigned int LoadIA_InTexturePosition_N64 = 0;
			unsigned int LoadIA_InTexturePosition_OGL = 0;

			for(j = 0; j < Texture.Height; j++) {
				for(i = 0; i < Texture.Width / 2; i++) {
					LoadIA_IAData = (TextureData_N64[LoadIA_InTexturePosition_N64] & 0xF0) >> 4;
					LoadIA_IExtract1 = (LoadIA_IAData & 0x0E) << 4;
					if((LoadIA_IAData & 0x01)) { LoadIA_AExtract1 = 0xFF; } else { LoadIA_AExtract1 = 0x00; }

					LoadIA_IAData = (TextureData_N64[LoadIA_InTexturePosition_N64] & 0x0F);
					LoadIA_IExtract2 = (LoadIA_IAData & 0x0E) << 4;
					if((LoadIA_IAData & 0x01)) { LoadIA_AExtract2 = 0xFF; } else { LoadIA_AExtract2 = 0x00; }

					TextureData_OGL[LoadIA_InTexturePosition_OGL]     = LoadIA_IExtract1;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 1] = LoadIA_IExtract1;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 2] = LoadIA_IExtract1;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 3] = LoadIA_AExtract1;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 4] = LoadIA_IExtract2;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 5] = LoadIA_IExtract2;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 6] = LoadIA_IExtract2;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 7] = LoadIA_AExtract2;

					LoadIA_InTexturePosition_N64 += 1;
					LoadIA_InTexturePosition_OGL += 8;
				}
				LoadIA_InTexturePosition_N64 += Texture.LineSize * 8 - (Texture.Width / 2);
			}

			break;
			}
		case 0x68:
			{
			unsigned int LoadIA_IAData = 0;

			unsigned int LoadIA_IExtract = 0;
			unsigned int LoadIA_AExtract = 0;

			unsigned int LoadIA_InTexturePosition_N64 = 0;
			unsigned int LoadIA_InTexturePosition_OGL = 0;

			for(j = 0; j < Texture.Height; j++) {
				for(i = 0; i < Texture.Width; i++) {
					LoadIA_IAData = TextureData_N64[LoadIA_InTexturePosition_N64];
					LoadIA_IExtract = (LoadIA_IAData & 0xFE);
					if((LoadIA_IAData & 0x01)) { LoadIA_AExtract = 0xFF; } else { LoadIA_AExtract = 0x00; }

					TextureData_OGL[LoadIA_InTexturePosition_OGL]     = LoadIA_IExtract;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 1] = LoadIA_IExtract;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 2] = LoadIA_IExtract;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 3] = LoadIA_AExtract;

					LoadIA_InTexturePosition_N64 += 1;
					LoadIA_InTexturePosition_OGL += 4;
				}
				LoadIA_InTexturePosition_N64 += Texture.LineSize * 8 - Texture.Width;
			}

			break;
			}
		case 0x70:
			{
			unsigned int LoadIA_IAData = 0;

			unsigned int LoadIA_IExtract = 0;
			unsigned int LoadIA_AExtract = 0;

			unsigned int LoadIA_InTexturePosition_N64 = 0;
			unsigned int LoadIA_InTexturePosition_OGL = 0;

			for(j = 0; j < Texture.Height; j++) {
				for(i = 0; i < Texture.Width; i++) {
					LoadIA_IAData = TextureData_N64[LoadIA_InTexturePosition_N64];
					LoadIA_IExtract = LoadIA_IAData;

					LoadIA_IAData = TextureData_N64[LoadIA_InTexturePosition_N64 + 1];
					LoadIA_AExtract = LoadIA_IAData;

					TextureData_OGL[LoadIA_InTexturePosition_OGL]     = LoadIA_IExtract;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 1] = LoadIA_IExtract;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 2] = LoadIA_IExtract;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 3] = LoadIA_AExtract;

					LoadIA_InTexturePosition_N64 += 2;
					LoadIA_InTexturePosition_OGL += 4;
				}
				LoadIA_InTexturePosition_N64 += Texture.LineSize * 4 - Texture.Width;
			}

			break;
			}
		/* I FORMAT */
		case 0x80:
		case 0x90:
			{
			/* 4bit - spot00 pathways, castle town walls, treeline near kokiri entrance, besitu blue patterned walls */
			unsigned int LoadI_IData = 0;

			unsigned int LoadI_IExtract1 = 0;
			unsigned int LoadI_IExtract2 = 0;

			unsigned int LoadI_InTexturePosition_N64 = 0;
			unsigned int LoadI_InTexturePosition_OGL = 0;

			for(j = 0; j < Texture.Height; j++) {
				for(i = 0; i < Texture.Width / 2; i++) {
					LoadI_IData = (TextureData_N64[LoadI_InTexturePosition_N64] & 0xF0) >> 4;
					LoadI_IExtract1 = (LoadI_IData & 0x0F) << 4;

					LoadI_IData = (TextureData_N64[LoadI_InTexturePosition_N64] & 0x0F);
					LoadI_IExtract2 = (LoadI_IData & 0x0F) << 4;

					TextureData_OGL[LoadI_InTexturePosition_OGL]     = LoadI_IExtract1;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 1] = LoadI_IExtract1;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 2] = LoadI_IExtract1;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 3] = (PrimColor[3] * 255);

					TextureData_OGL[LoadI_InTexturePosition_OGL + 4] = LoadI_IExtract2;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 5] = LoadI_IExtract2;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 6] = LoadI_IExtract2;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 7] = (PrimColor[3] * 255);

					LoadI_InTexturePosition_N64 += 1;
					LoadI_InTexturePosition_OGL += 8;
				}
				LoadI_InTexturePosition_N64 += Texture.LineSize * 8 - (Texture.Width / 2);
			}

			break;
			}
		case 0x88:
			{
			/* 8bit - Bmori_0 l/r side walls */
			unsigned int LoadI_IData = 0;

			unsigned int LoadI_InTexturePosition_N64 = 0;
			unsigned int LoadI_InTexturePosition_OGL = 0;

			for(j = 0; j < Texture.Height; j++) {
				for(i = 0; i < Texture.Width; i++) {
					LoadI_IData = TextureData_N64[LoadI_InTexturePosition_N64];

					TextureData_OGL[LoadI_InTexturePosition_OGL]     = LoadI_IData;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 1] = LoadI_IData;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 2] = LoadI_IData;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 3] = (PrimColor[3] * 255);

					LoadI_InTexturePosition_N64 += 1;
					LoadI_InTexturePosition_OGL += 4;
				}
				LoadI_InTexturePosition_N64 += Texture.LineSize * 8 - Texture.Width;
			}

			break;
			}
		/* FALLBACK - gives us an empty texture */
		default:
			{
			sprintf(ErrorMsg, "Unhandled Texture Type 0x%02X!", Texture.Format_N64);
			MessageBox(hwnd, ErrorMsg, "Error", MB_OK | MB_ICONERROR);
			Texture.Format_OGL = GL_RGBA;
			Texture.Format_OGLPixel = GL_RGBA;
			memcpy(TextureData_OGL, EmptyTexture_Green, TextureBufferSize);
			break;
			}
		}
	}

	glGenTextures(1, &GLTexture);
	glBindTexture(GL_TEXTURE_2D, GLTexture);

	switch(Texture.Y_Parameter) {
		case 1:  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); break;
		case 2:  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); break;
		case 3:  if(GLExtension_TextureMirror) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT_ARB); break;
		default: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); break;
	}

	switch(Texture.X_Parameter) {
		case 1:  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); break;
		case 2:  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); break;
		case 3:  if(GLExtension_TextureMirror) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT_ARB); break;
		default: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); break;
	}

	if(GLExtension_AnisoFilter) {
		float AnisoMax;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &AnisoMax);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, AnisoMax);
	} else {
		//
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Renderer_FilteringMode_Min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Renderer_FilteringMode_Mag);

	gluBuild2DMipmaps(GL_TEXTURE_2D, Texture.Format_OGL, Texture.WidthRender, Texture.HeightRender, Texture.Format_OGLPixel, GL_UNSIGNED_BYTE, TextureData_OGL);

	free(TextureData_N64);
	free(TextureData_OGL);

	free(EmptyTexture_Red);
	free(EmptyTexture_Green);

	sprintf(ErrorMsg, "TEXTURE:\n \
	Height %d, HeightRender %d, Width %d, WidthRender %d\n \
	DataSource %x, PalDataSource %x, Offset %x, PalOffset %x\n \
	Format_N64 %x, Format_OGL %x, Format_OGLPixel %x\n \
	Y_Parameter %d, X_Parameter %d, S_Scale %d, T_Scale %d\n \
	LineSize %d, Palette %d\n",
						Texture.Height, Texture.HeightRender, Texture.Width, Texture.WidthRender,
						Texture.DataSource, Texture.PalDataSource, (unsigned int)Texture.Offset, (unsigned int)Texture.PalOffset,
						Texture.Format_N64, Texture.Format_OGL, Texture.Format_OGLPixel,
						Texture.Y_Parameter, Texture.X_Parameter, Texture.S_Scale, Texture.T_Scale,
						Texture.LineSize, Texture.Palette);

	HelperFunc_LogMessage(2, ErrorMsg);

	return GLTexture;
}

/*	------------------------------------------------------------ */
