#include "StdAfx.h"
#include "D_Environment.h"


std::wstring D_Environment::GetAppDataDir(bool create)
{
  wchar_t buf[MAX_PATH] ={0};
  if (SHGetSpecialFolderPathW(NULL, buf, CSIDL_APPDATA, create))
  {
    return buf;
  }
  return L"";
}

std::wstring D_Environment::GetDesktopDir(bool create)
{
	wchar_t buf[MAX_PATH] ={0};
	if (SHGetSpecialFolderPathW(NULL, buf, CSIDL_DESKTOP, create))
	{
		return buf;
	}
	return L"";
}
std::wstring D_Environment::GetCommonAppDataDir(bool create)
{
  wchar_t buf[MAX_PATH] ={0};
  if (SHGetSpecialFolderPathW(NULL, buf, CSIDL_COMMON_APPDATA, create))
  {
    return buf;
  }
  return L"";
}

std::wstring D_Environment::GetExePath()
{
  wchar_t buf[MAX_PATH] = {0};
  if (GetModuleFileNameW(NULL, buf, MAX_PATH) == 0) 
    return L"";
  return buf;
}

std::wstring D_Environment::GetExeDir()
{
  wchar_t buf[MAX_PATH] = {0};
  if (GetModuleFileNameW(NULL, buf, MAX_PATH) == 0) 
    return L"";

  std::wstring exePath = buf;
  return exePath.substr(0, exePath.find_last_of(L'\\'));
}
std::wstring  D_Environment::GetProductDir()
{
	BOOL bRun = FALSE;
	HKEY hKey = HKEY_LOCAL_MACHINE;
	DWORD dwType = REG_SZ;
	DWORD dwCont = MAX_PATH;
	wchar_t buf[1024]={0};
	wchar_t wszSubKey[]=DUL_BOX_REG_NAMEW;

	HKEY hSoftKey = NULL;
	wstring value = L"";
	DWORD style;
	DWORD len = 1024;

	long rc = RegOpenKeyExW(hKey, wszSubKey,NULL,  KEY_ALL_ACCESS|KEY_WOW64_64KEY , &hSoftKey);
	if (rc != ERROR_SUCCESS)
	{
		return  L""; 
	}

	rc = RegQueryValueExW(hSoftKey, L"Path", NULL, &style, (LPBYTE)buf, &len);
	if (rc != ERROR_SUCCESS)
	{
		RegCloseKey(hSoftKey);
		return  L"";
	}

	value = buf;
	RegCloseKey(hSoftKey);
	return value;

}