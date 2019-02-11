#include "CaptureScreen.h"

#include <tchar.h>
#include <Strsafe.h>

HBITMAP ScreenCapture(LPSTR filename, WORD bitCount, const LPRECT lpRect, double dfamplification)
{
	HBITMAP hBitmap;
	HDC hScreenDC = CreateDCA("DISPLAY", NULL, NULL, NULL); //通过指定DISPLAY来获取一个显示设备上下文环境
	HDC hmemDC = CreateCompatibleDC(hScreenDC);          //该函数创建一个与指定设备兼容的内存设备上下文环境（DC）
	int ScreenWidth = GetDeviceCaps(hScreenDC, HORZRES); //获取指定设备的性能参数（此处获取屏幕宽度）
	int ScreenHeight = GetDeviceCaps(hScreenDC, VERTRES); //获取指定设备的性能参数（此处获取屏幕高度）
	HBITMAP hOldBM;
	PVOID lpData;   //内存分配成功返回的指向所分配内存块的首地址指针
	int startX;     //截图x位置
	int startY;     //截图y位置
	int width;      //截图宽度
	int height;     //截图高度
	long BitmapSize;
	DWORD BitsOffset;
	DWORD ImageSize;
	DWORD FileSize;
	BITMAPINFOHEADER bmInfo; //BITMAPINFOHEADER结构所包含的成员表明了图像的尺寸、原始设备的颜色格式、以及数据压缩方案
	BITMAPFILEHEADER bmFileHeader;
	HANDLE bmFile, hpal, holdpal = NULL;;
	DWORD dwWritten;
	if (lpRect == NULL)
	{
		startX = startY = 0;
		width = ScreenWidth;
		height = ScreenHeight;
	}
	else
	{
		startX = lpRect->left;
		startY = lpRect->top;
		width = lpRect->right - lpRect->left;
		height = lpRect->bottom - lpRect->top;
	}

	double xScale = 1;
	double yScale = 1;
	DEVMODE curDevMode;
	memset(&curDevMode, 0, sizeof(curDevMode));
	curDevMode.dmSize = sizeof(DEVMODE);
	BOOL bEnumRet = ::EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &curDevMode);
	if (bEnumRet)
	{
		xScale = curDevMode.dmPelsWidth / (double)ScreenWidth;
		yScale = curDevMode.dmPelsHeight / (double)ScreenHeight;
	}

	int width2 = width * xScale;
	int height2 = height * yScale;
	int width3 = width2 * dfamplification;
	int height3 = height2 * dfamplification;
	//创建一张长width，宽height的画布，用于后面绘制图形
	hBitmap = CreateCompatibleBitmap(hScreenDC, width3, height3);
	//该函数选择一对象到指定的设备上下文环境中，该新对象替换先前的相同类型的对象。
	hOldBM = (HBITMAP)SelectObject(hmemDC, hBitmap);

	//该函数对指定的源设备环境区域中的像素进行位块（bit_block）转换，以传送到目标设备环境。
	StretchBlt(hmemDC, 0, 0, width3, height3, hScreenDC, startX * xScale, startY * yScale, width2, height2, SRCCOPY | CAPTUREBLT);
	//BitBlt(hmemDC, 0, 0, width2, height2, hScreenDC, startX * xScale, startY * yScale, SRCCOPY | CAPTUREBLT);

	char szBuf[512] = {0};
	sprintf(szBuf, "xScale=%f yScale=%f screenWidth=%d screenHeight=%d DevModeWidth=%d DevModeHeight=%d width=%d height=%d destWidth=%d destHeight=%d\n",
		xScale, yScale, ScreenWidth, ScreenHeight, curDevMode.dmPelsWidth, curDevMode.dmPelsHeight, width, height, width2, height2);
	OutputDebugStringA(szBuf);

	hBitmap = (HBITMAP)SelectObject(hmemDC, hOldBM);
	if (filename == NULL)
	{
		DeleteDC(hScreenDC);
		DeleteDC(hmemDC);
		return hBitmap;
	}
	BitmapSize = ((((width3 * 32) + 32) / 32) * 4) * height3;
	//用来在指定的堆上分配内存，并且分配后的内存不可移动(HEAP_NO_SERIALIZE 不使用连续存取)
	lpData = HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, BitmapSize);
	ZeroMemory(lpData, BitmapSize);
	ZeroMemory(&bmInfo, sizeof(BITMAPINFOHEADER));
	bmInfo.biSize = sizeof(BITMAPINFOHEADER); //位图信息结构长度 ,必须为40
	bmInfo.biWidth = width3;                 //图像宽度 单位是像素
	bmInfo.biHeight = height3;               //图像高度 单位是像素
	bmInfo.biPlanes = 1;                    //必须为1
	bmInfo.biBitCount = bitCount;           //设置图像的位数。比如8位，16位，32位位数越高分辨率越高
	bmInfo.biCompression = BI_RGB;          //位图是否压缩 BI_RGB为不压缩
	ZeroMemory(&bmFileHeader, sizeof(BITMAPFILEHEADER));
	BitsOffset = sizeof(BITMAPFILEHEADER) + bmInfo.biSize;
	ImageSize = ((((bmInfo.biWidth * bmInfo.biBitCount) + 31) / 32) * 4) * bmInfo.biHeight;
	FileSize = BitsOffset + ImageSize;
	bmFileHeader.bfType = 0x4d42; //'B'+('M'<<8);
	bmFileHeader.bfSize = FileSize;
	bmFileHeader.bfOffBits = BitsOffset;
	hpal = GetStockObject(DEFAULT_PALETTE);
	if (hpal)
	{
		holdpal = SelectPalette(hmemDC, (HPALETTE)hpal, false);
		RealizePalette(hmemDC);
	}
	GetDIBits(hmemDC, hBitmap, 0, bmInfo.biHeight, lpData, (BITMAPINFO *)&bmInfo, DIB_RGB_COLORS);
	if (holdpal)
	{
		SelectPalette(hmemDC, (HPALETTE)holdpal, true);
		RealizePalette(hmemDC);
	}

	bmFile = CreateFileA(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (bmFile == INVALID_HANDLE_VALUE)
	{
		OutputDebugString(_T("Create File Error!\n"));
	}
	WriteFile(bmFile, &bmFileHeader, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	WriteFile(bmFile, &bmInfo, sizeof(BITMAPINFOHEADER), &dwWritten, NULL);
	WriteFile(bmFile, lpData, ImageSize, &dwWritten, NULL);
	CloseHandle(bmFile);
	HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, lpData);
	::ReleaseDC(0, hScreenDC);
	DeleteDC(hmemDC);
	return hBitmap;
}