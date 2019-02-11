#include "TimeStamp.h"

ULONGLONG CurrentTimeStamp()
{
	SYSTEMTIME stBase;
	FILETIME ftBase;
	ULARGE_INTEGER uliBase;

	ZeroMemory(&stBase, sizeof(SYSTEMTIME));
	stBase.wYear = 1970;
	stBase.wMonth = 1;
	stBase.wDay = 1;

	SystemTimeToFileTime(&stBase, &ftBase);

	uliBase.LowPart = ftBase.dwLowDateTime;
	uliBase.HighPart = ftBase.dwHighDateTime;

	SYSTEMTIME st;
	FILETIME ft;
	FILETIME ft1;
	ULARGE_INTEGER uli;
	memset(&st, 0, sizeof(SYSTEMTIME));
	GetLocalTime(&st);
	SystemTimeToFileTime(&st, &ft);
	LocalFileTimeToFileTime(&ft, &ft1);
	uli.LowPart = ft1.dwLowDateTime;
	uli.HighPart = ft1.dwHighDateTime;
	return (uli.QuadPart - uliBase.QuadPart) / 10000;
}

//ULONGLONG CurrentTimeStamp()
//{
//	SYSTEMTIME stBase;
//	FILETIME ftBase;
//	ULARGE_INTEGER uliBase;
//
//	ZeroMemory(&stBase, sizeof(SYSTEMTIME));
//	stBase.wYear = 1970;
//	stBase.wMonth = 1;
//	stBase.wDay = 1;
//
//	SystemTimeToFileTime(&stBase, &ftBase);
//
//	uliBase.LowPart = ftBase.dwLowDateTime;
//	uliBase.HighPart = ftBase.dwHighDateTime;
//
//	SYSTEMTIME st;
//	FILETIME ft;
//	ULARGE_INTEGER uli;
//	memset(&st, 0, sizeof(SYSTEMTIME));
//	GetLocalTime(&st);
//	SystemTimeToFileTime(&st, &ft);
//	uli.LowPart = ft.dwLowDateTime;
//	uli.HighPart = ft.dwHighDateTime;
//
//	return (uli.QuadPart - uliBase.QuadPart) / 10000;
//}