#ifndef ProcessUtil_h__
#define ProcessUtil_h__

#include <Windows.h>

BOOL GetProcessName(DWORD processID, OUT LPWSTR lpszProcessName);

#endif // ProcessUtil_h__
