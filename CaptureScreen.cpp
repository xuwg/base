#include "CaptureScreen.h"

#include <tchar.h>
#include <Strsafe.h>

HBITMAP ScreenCapture(LPSTR filename, WORD bitCount, const LPRECT lpRect, double dfamplification)
{
	HBITMAP hBitmap;
	HDC hScreenDC = CreateDCA("DISPLAY", NULL, NULL, NULL); //ͨ��ָ��DISPLAY����ȡһ����ʾ�豸�����Ļ���
	HDC hmemDC = CreateCompatibleDC(hScreenDC);          //�ú�������һ����ָ���豸���ݵ��ڴ��豸�����Ļ�����DC��
	int ScreenWidth = GetDeviceCaps(hScreenDC, HORZRES); //��ȡָ���豸�����ܲ������˴���ȡ��Ļ��ȣ�
	int ScreenHeight = GetDeviceCaps(hScreenDC, VERTRES); //��ȡָ���豸�����ܲ������˴���ȡ��Ļ�߶ȣ�
	HBITMAP hOldBM;
	PVOID lpData;   //�ڴ����ɹ����ص�ָ���������ڴ����׵�ַָ��
	int startX;     //��ͼxλ��
	int startY;     //��ͼyλ��
	int width;      //��ͼ���
	int height;     //��ͼ�߶�
	long BitmapSize;
	DWORD BitsOffset;
	DWORD ImageSize;
	DWORD FileSize;
	BITMAPINFOHEADER bmInfo; //BITMAPINFOHEADER�ṹ�������ĳ�Ա������ͼ��ĳߴ硢ԭʼ�豸����ɫ��ʽ���Լ�����ѹ������
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
	//����һ�ų�width����height�Ļ��������ں������ͼ��
	hBitmap = CreateCompatibleBitmap(hScreenDC, width3, height3);
	//�ú���ѡ��һ����ָ�����豸�����Ļ����У����¶����滻��ǰ����ͬ���͵Ķ���
	hOldBM = (HBITMAP)SelectObject(hmemDC, hBitmap);

	//�ú�����ָ����Դ�豸���������е����ؽ���λ�飨bit_block��ת�����Դ��͵�Ŀ���豸������
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
	//������ָ���Ķ��Ϸ����ڴ棬���ҷ������ڴ治���ƶ�(HEAP_NO_SERIALIZE ��ʹ��������ȡ)
	lpData = HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, BitmapSize);
	ZeroMemory(lpData, BitmapSize);
	ZeroMemory(&bmInfo, sizeof(BITMAPINFOHEADER));
	bmInfo.biSize = sizeof(BITMAPINFOHEADER); //λͼ��Ϣ�ṹ���� ,����Ϊ40
	bmInfo.biWidth = width3;                 //ͼ���� ��λ������
	bmInfo.biHeight = height3;               //ͼ��߶� ��λ������
	bmInfo.biPlanes = 1;                    //����Ϊ1
	bmInfo.biBitCount = bitCount;           //����ͼ���λ��������8λ��16λ��32λλ��Խ�߷ֱ���Խ��
	bmInfo.biCompression = BI_RGB;          //λͼ�Ƿ�ѹ�� BI_RGBΪ��ѹ��
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