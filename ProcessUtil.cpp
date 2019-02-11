#include "ProcessUtil.h"

#include <Tlhelp32.h>
#include <Strsafe.h>

BOOL GetProcessName(DWORD processID, OUT LPWSTR lpszProcessName)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32W pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32FirstW(hSnapshot, &pe))
	{
		CloseHandle(hSnapshot);
		return FALSE;
	}
	while (Process32NextW(hSnapshot, &pe))
	{
		if (pe.th32ProcessID == processID)
		{
			wcscpy(lpszProcessName, pe.szExeFile);
			CloseHandle(hSnapshot);
			return true;
		}
	}
	CloseHandle(hSnapshot);
	return FALSE;
}

