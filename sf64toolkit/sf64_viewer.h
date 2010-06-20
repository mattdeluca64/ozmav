extern int sv_EnableViewer(unsigned char * Ptr);
extern int sv_Init();
extern void sv_LoadToSegment(unsigned char Segment, unsigned char * Buffer, unsigned int Offset, unsigned int Size);
extern void sv_ClearSegment(unsigned char Segment);
extern void sv_ClearAllSegments();
extern void sv_ClearStructures(bool Full);
extern bool sv_CheckAddressValidity(unsigned int Address);
extern void sv_ExecuteDisplayLists();
extern int sv_DeInit();
