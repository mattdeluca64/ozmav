/*	------------------------------------------------------------
	OZMAV - OpenGL Zelda Map Viewer

	Written in October/November 2008 by xdaniel & contributors
	http://ozmav.googlecode.com/
	------------------------------------------------------------
    helpers.c - misc. helper functions (log file handling, etc.)
	------------------------------------------------------------ */

#include "globals.h"

/*	------------------------------------------------------------ */

int Viewer_ZMemCopy(unsigned int SourceBank, unsigned long SourceOffset, unsigned char * Target, unsigned long Size)
{
	int ReturnValue = 0;

	switch(SourceBank) {
	case 0x02:
		/* data comes from scene file */
		memcpy(Target, &ZSceneBuffer[SourceOffset / 4], Size);
		break;
	case 0x03:
		/* data comes from map file */
		memcpy(Target, &ZMapBuffer[SourceOffset / 4], Size);
		break;
	case 0x04:
		/* data comes from gameplay_keep */
		memcpy(Target, &GameplayKeepBuffer[SourceOffset / 4], Size);
		break;
	case 0x05:
		/* data comes from gameplay_dangeon_keep */
		/* (gameplay_field_keep is not handled yet, always loading in file specified in Filename_GameplayFDKeep */
		memcpy(Target, &GameplayFDKeepBuffer[SourceOffset / 4], Size);
		break;
	default:
		/* fallback if source is not handled / no valid source was found */
		memset(Target, 0xFF, Size);
		ReturnValue = -1;
		break;
	}

	return ReturnValue;
}

/*	------------------------------------------------------------ */

/* HELPERFUNC_SPLITCURRENTVALS - TAKES THE 4 BYTES OF THE LONG VARIABLE(S) USED FOR DATA FETCHING AND SPLITS THEM APART */
void HelperFunc_SplitCurrentVals(bool SplitDual)
{
	Readout_CurrentByte1 = Readout_Current1 << 24;
	Readout_CurrentByte1 = Readout_CurrentByte1 >> 24;
	Readout_CurrentByte2 = Readout_Current1 << 16;
	Readout_CurrentByte2 = Readout_CurrentByte2 >> 24;
	Readout_CurrentByte3 = Readout_Current1 << 8;
	Readout_CurrentByte3 = Readout_CurrentByte3 >> 24;
	Readout_CurrentByte4 = Readout_Current1;
	Readout_CurrentByte4 = Readout_CurrentByte4 >> 24;

	if(SplitDual) {
		Readout_CurrentByte5 = Readout_Current2 << 24;
		Readout_CurrentByte5 = Readout_CurrentByte5 >> 24;
		Readout_CurrentByte6 = Readout_Current2 << 16;
		Readout_CurrentByte6 = Readout_CurrentByte6 >> 24;
		Readout_CurrentByte7 = Readout_Current2 << 8;
		Readout_CurrentByte7 = Readout_CurrentByte7 >> 24;
		Readout_CurrentByte8 = Readout_Current2;
		Readout_CurrentByte8 = Readout_CurrentByte8 >> 24;
	}
}

/* HELPERFUNC_LOGMESSAGE - WRITES GIVEN STRING INTO PREVIOUSLY OPENED LOG FILE */
int HelperFunc_LogMessage(int LogType, char Message[])
{
	switch(LogType) {
	case 1:
		fprintf(FileGFXLog, Message);
		break;
	case 2:
		fprintf(FileSystemLog, Message);
		break;
	}
	return 0;
}

/* HELPERFUNC_GFXLOGCOMMAND - WRITES INFORMATION ABOUT CURRENTLY PROCESSED COMMAND INTO LOG FILE */
int HelperFunc_GFXLogCommand(unsigned int Position)
{
	sprintf(GFXLogMsg, "  0x%08X:\t%s\t\t[%02X%02X%02X%02X %02X%02X%02X%02X] %s\n",
		Position * 4, CurrentGFXCmd,
		Readout_CurrentByte1, Readout_CurrentByte2, Readout_CurrentByte3, Readout_CurrentByte4,
		Readout_CurrentByte5, Readout_CurrentByte6, Readout_CurrentByte7, Readout_CurrentByte8,
		CurrentGFXCmdNote);
	HelperFunc_LogMessage(1, GFXLogMsg);

	return 0;
}

/* HELPERFUNC_CALCULATEFPS - CALCULATE THE VIEWER'S CURRENT FPS */
int HelperFunc_CalculateFPS()
{
	if(GetTickCount() - Renderer_LastFPS >= 1000)
	{
		Renderer_LastFPS = GetTickCount();
		Renderer_FPS = Renderer_FrameNo;
		Renderer_FrameNo = 0;
	}
	Renderer_FrameNo++;

	sprintf(Renderer_FPSMessage, "%d FPS", Renderer_FPS);
	SendMessage(hstatus, SB_SETTEXT, 0, (LPARAM)Renderer_FPSMessage);

	return 0;
}

/*	------------------------------------------------------------ */
