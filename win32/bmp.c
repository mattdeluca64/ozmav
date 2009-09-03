#include "..\globals.h"

bool SaveBMP(BYTE* Buffer, int width, int height, long paddedsize, char* bmpfile)
{
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER info;

	memset (&bmfh, 0, sizeof(BITMAPFILEHEADER));
	memset (&info, 0, sizeof(BITMAPINFOHEADER));

	bmfh.bfType = 0x4d42;
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paddedsize;
	bmfh.bfOffBits = 0x36;

	info.biSize = sizeof(BITMAPINFOHEADER);
	info.biWidth = width;
	info.biHeight = height;
	info.biPlanes = 1;
	info.biBitCount = 24;
	info.biCompression = BI_RGB;
	info.biSizeImage = 0;
	info.biXPelsPerMeter = 0x0ec4;
	info.biYPelsPerMeter = 0x0ec4;
	info.biClrUsed = 0;
	info.biClrImportant = 0;

	HANDLE file = CreateFile(bmpfile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == NULL) {
		CloseHandle(file);
		return false;
	}

	unsigned long bwritten;
	if (WriteFile(file, &bmfh, sizeof(BITMAPFILEHEADER), &bwritten, NULL) == false) {
		CloseHandle(file);
		return false;
	}

	if (WriteFile(file, &info, sizeof(BITMAPINFOHEADER), &bwritten, NULL) == false) {
		CloseHandle(file);
		return false;
	}

	if (WriteFile(file, Buffer, paddedsize, &bwritten, NULL) == false) {
		CloseHandle(file);
		return false;
	}

	CloseHandle(file);

	return true;
}

BYTE* ConvertToBMP(BYTE* Buffer, int width, int height, long* newsize)
{

	if ((NULL == Buffer) || (width == 0) || (height == 0))
		return NULL;

	int padding = 0;
	int scanlinebytes = width * 3;
	while ((scanlinebytes + padding) % 4 != 0)
		padding++;
	int psw = scanlinebytes + padding;

	*newsize = height * psw;

	BYTE* newbuf = (unsigned char *) malloc(*newsize);

	memset (newbuf, 0, *newsize);

	long bufpos = height * width * 4;
	long newpos = 0;
	int y = 0, x = 0;
	for(y = 0; y < height; y++) {
		for (x = 0; x < 3 * width; x+=3) {
			bufpos-=4;
			newbuf[newpos++] = Buffer[bufpos + 2];
			newbuf[newpos++] = Buffer[bufpos + 1];
			newbuf[newpos++] = Buffer[bufpos];
		}
	}

	return newbuf;
}
