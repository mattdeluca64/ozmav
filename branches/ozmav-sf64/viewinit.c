/*	------------------------------------------------------------
	OZMAV SF64
	 - Branch of 'OpenGL Zelda Map Viewer' for Starfox 64

	Written in October/November 2008 by xdaniel & contributors
	http://ozmav.googlecode.com/
	------------------------------------------------------------
	viewinit.c - DList loader
	------------------------------------------------------------ */

#include "globals.h"

/*	------------------------------------------------------------ */

int Viewer_GetDisplayLists(unsigned long Fsize)
{
	unsigned int DListScanPosition = 0;
	DListInfo_CurrentCount = -1;
	unsigned long TempOffset = 0;

	while ((DListScanPosition < Fsize / 4)) {
		memcpy(&Readout_Current1, &ObjBuffer[DListScanPosition], 4);
		memcpy(&Readout_Current2, &ObjBuffer[DListScanPosition + 1], 4);

		HelperFunc_SplitCurrentVals(true);

		/* fetching entry points via SETTIMG since they tend to be the first command in a DList */
		if ((Readout_CurrentByte1 == G_SETTIMG) && (Readout_CurrentByte2 == 0x10)) {
			if((Readout_CurrentByte3 == 0x00) && (Readout_CurrentByte4 == 0x00)) {
				DListInfo_CurrentCount++;
				DLists[DListInfo_CurrentCount] = (DListScanPosition * 4);
			}
		}

		DListScanPosition += 2;
	}

	if(DListInfo_CurrentCount == -1) {
		MessageBox(hwnd, "No Entry Points found!", "Error", MB_OK | MB_ICONERROR);
	}

	return 0;
}

/*	------------------------------------------------------------ */
