extern bool sf_Init();
extern void sf_LoadROM(unsigned char * Ptr);
extern int sf_DoLoadROM();
extern int sf_ReadDMATable();
extern void sf_ListDMATable(unsigned char * Ptr);
extern void sf_ExtractFiles(unsigned char * Ptr);
extern void sf_CreateExpandedROM(unsigned char * Ptr);
extern void sf_CreateFreshROM(unsigned char * Ptr);
extern int sf_DecompressMIO0(unsigned int SrcOffset);
