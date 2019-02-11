// D_COMMON.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "D_Common.h"
#include <winioctl.h>
#include <windows.h>
#include <time.h>
#include <intrin.h>
#include "socks.h"
#include "debugUtil.h"
#include "encodeDecode.h"




#define IN6ADDRSZ 16 
#define INT16SZ 2
#define INET6_ADDRSTRLEN 65
#define INET_ADDRSTRLEN  22
#define random(x) (rand()%x)


HDC GetPrinterDC(char* pPrinterName)
{
	HDC			hPrinterDC	= NULL;
	HANDLE		hPrinter	= NULL;
	LPDEVMODEA	pDevMode	= NULL;
	DWORD		dwSize = 0;

	OpenPrinterA(pPrinterName, &hPrinter, NULL);
	if( hPrinter )
	{
		dwSize		= DocumentPropertiesA(NULL, hPrinter, pPrinterName,NULL,NULL, 0);
		pDevMode	= (LPDEVMODEA)HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, dwSize );
		if( pDevMode )
		{
			DocumentPropertiesA(NULL, hPrinter, pPrinterName,pDevMode,NULL, DM_OUT_BUFFER);
			hPrinterDC = CreateDCA(NULL,pPrinterName,NULL,pDevMode);
			HeapFree( GetProcessHeap(), 0, pDevMode );
		}
		ClosePrinter(hPrinter);
	}
	return hPrinterDC;
}

BOOL GetJobs(HANDLE hPrinter,
	JOB_INFO_2 **ppJobInfo,
	int *pcJobs,           
	DWORD *pStatus)        
{
	DWORD        cByteNeeded, nReturned, cByteUsed;
	JOB_INFO_2          *pJobStorage = NULL;
	PRINTER_INFO_2       *pPrinterInfo = NULL;

	if (!GetPrinter(hPrinter, 2, NULL, 0, &cByteNeeded))
	{
		DWORD dwErrorCode = ::GetLastError();
		if (dwErrorCode != ERROR_INSUFFICIENT_BUFFER)
			return FALSE;
	}
	pPrinterInfo = (PRINTER_INFO_2 *)malloc(cByteNeeded);
	if (!(pPrinterInfo))
		return FALSE;

	if (!GetPrinter(hPrinter,
		2,
		(LPBYTE)pPrinterInfo,
		cByteNeeded,
		&cByteUsed))
	{

		free(pPrinterInfo);
		pPrinterInfo = NULL;
		return FALSE;
	}

	if (!EnumJobs(hPrinter,
		0,
		pPrinterInfo->cJobs,
		2,
		NULL,
		0,
		(LPDWORD)&cByteNeeded,
		(LPDWORD)&nReturned))
	{
		if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		{
			free(pPrinterInfo);
			pPrinterInfo = NULL;
			return FALSE;
		}
	}
	pJobStorage = (JOB_INFO_2 *)malloc(cByteNeeded);
	if (!pJobStorage)
	{

		free(pPrinterInfo);
		pPrinterInfo = NULL;
		return FALSE;
	}
	ZeroMemory(pJobStorage, cByteNeeded);

	if (!EnumJobs(hPrinter,
		0,
		pPrinterInfo->cJobs,
		2,
		(LPBYTE)pJobStorage,
		cByteNeeded,
		(LPDWORD)&cByteUsed,
		(LPDWORD)&nReturned))
	{
		free(pPrinterInfo);
		free(pJobStorage);
		pJobStorage = NULL;
		pPrinterInfo = NULL;
		return FALSE;
	}

	*pcJobs = nReturned;
	*pStatus = pPrinterInfo->Status;
	*ppJobInfo = pJobStorage;
	free(pPrinterInfo);
	return TRUE;
}

void FileTimeToTime_t( FILETIME ft, time_t *t )
{
	LONGLONG ll;

	ULARGE_INTEGER	ui;
	ui.LowPart	= ft.dwLowDateTime;
	ui.HighPart	= ft.dwHighDateTime;

	ll	= ft.dwHighDateTime << 32 + ft.dwLowDateTime;

	*t	= ((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);
}


 VOID DelPrintedJosTimeOut(char* pPrinterName, DWORD dwTimeSecond)
 {
	 int         cJobs = 0,i = 0;
	 HANDLE      hPrinter = NULL;
	 JOB_INFO_2  *pJobs = NULL;
	 DWORD       dwPrinterStatus = 0;
	 do 
	 {
		 if (pPrinterName == NULL)
		 {
			 break;
		 }
		 if (!OpenPrinterA(pPrinterName, &hPrinter, NULL) || hPrinter == NULL)
		 {
			break;
		 }
		 //获得打印机的所有打印任务
		 if (!GetJobs(hPrinter, &pJobs, &cJobs, &dwPrinterStatus))
		 {
			 break;
		 }
		 //没有任务不管
		 if (cJobs == 0)
		 {
			 break;
		 }
		 for (i = 0; i < cJobs; ++i)
		 {
			 if((pJobs[i].Status  & JOB_STATUS_PRINTED) ||
				 (pJobs[i].Status  & JOB_STATUS_COMPLETE))
			 {
				 time_t t1 = 0;
				 time_t t2 = 0;
				 SYSTEMTIME systemtime={0};
				 FILETIME filetime = {0};
				 FILETIME localtime = {0};
				 GetSystemTime(&systemtime);
				 SystemTimeToFileTime (&systemtime, &localtime);
				 FileTimeToTime_t(localtime, &t2);
				 SystemTimeToFileTime (&pJobs[i].Submitted, &filetime);
				 FileTimeToTime_t(filetime, &t1);
				 //差值超过超时秒
				 if (t2-t1 > dwTimeSecond)
				 {
					 SetJob(hPrinter, pJobs[i].JobId, 0, NULL, JOB_CONTROL_DELETE);
				 }
				 
			 }
		 }
	 } while (FALSE);
	 if (pJobs != NULL)
	 {
		 free( pJobs );
		 pJobs = NULL;
	 }
	 if (hPrinter != NULL)
	 {
		 ClosePrinter(hPrinter);
		 hPrinter= NULL;
	 }
 }

DWORD GetPrinterPaperWidth(LPSTR pPrinterName)
{
	if (pPrinterName == NULL)
	{
		return 0;
	}

	HDC			hPrinterDC	= NULL;
	HANDLE		hPrinter	= NULL;
	LPDEVMODEA	pDevMode	= NULL;
	DWORD		dwSize = 0;

	OpenPrinterA(pPrinterName, &hPrinter, NULL);
	if( hPrinter )
	{
		dwSize		= DocumentPropertiesA(NULL, hPrinter, pPrinterName,NULL,NULL, 0);
		pDevMode	= (LPDEVMODEA)HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, dwSize );
		if( pDevMode )
		{
			DocumentPropertiesA(NULL, hPrinter, pPrinterName,pDevMode,NULL, DM_OUT_BUFFER);
			dwSize = pDevMode->dmPaperWidth;
			HeapFree( GetProcessHeap(), 0, pDevMode );
		}
		ClosePrinter(hPrinter);
	}
	return dwSize;

}


 BOOL StartServerByName(LPCSTR lpServerName)
 {
	 BOOL bRun = FALSE;
	 SC_HANDLE   schService;  
	 SC_HANDLE   schSCManager;  
	 SERVICE_STATUS ssStatus;
	 if (lpServerName == NULL)
	 {
		 return FALSE;
	 }
	 schSCManager = ::OpenSCManager(  
		 NULL,                   // machine (NULL == local)  
		 NULL,                   // database (NULL == default)  
		 SC_MANAGER_ALL_ACCESS   // access required  
		 );  
	 if ( schSCManager )  
	 {  
		 schService = ::OpenServiceA(schSCManager,lpServerName,SERVICE_ALL_ACCESS);  
		 if (schService)  
		 {  
			 // 起动服务
			 if ( ::StartService(schService,NULL,NULL))  
			 {  
				 Sleep( 1000 );  
				 while( ::QueryServiceStatus( schService, &ssStatus ) )  
				 {  
					 if ( ssStatus.dwCurrentState == SERVICE_START_PENDING )  
					 {  
						 Sleep( 500 );  
					 }  
					 else  
						 break;  
				 }  
				 if ( ssStatus.dwCurrentState == SERVICE_RUNNING ) 
				 {
					  bRun = TRUE;
				 }
			 }  
			 else
			 {
				 ::QueryServiceStatus( schService, &ssStatus );
				 if ( ssStatus.dwCurrentState == SERVICE_RUNNING ) 
				 {
					bRun = TRUE;
				 }
			 }
			 ::CloseServiceHandle(schService);  
		 }  
		 ::CloseServiceHandle(schSCManager);  
	 }  
	 return bRun;
 }

//根据句柄获得兑现名字
BOOL GetFileNameFromHandle(HANDLE hFile, LPCSTR lpFilePath, DWORD dwBufCount)
{  
	BOOL bRun  = FALSE;
	const int ObjectNameInformation = 1;  // enum OBJECT_INFORMATION_CLASS; 
	PMYUNICODE_STRING pObjectName  = NULL;
	WCHAR  szTemp[MAX_PATH] = {0};
	WCHAR  szObjectPath[MAX_PATH] = {0};
	ULONG  dwResult = 0; 
	HMODULE hNtDLL = NULL;

	do 
	{
		//加载动态库
		hNtDLL = LoadLibraryW(L"ntdll.dll");   
		if (hNtDLL == NULL)       
		{
			break;
		}  
		//获取ZwQueryObject函数指针
		ZWQUERYOBJECT ZwQueryObject = (ZWQUERYOBJECT)GetProcAddress(hNtDLL, "ZwQueryObject"); 
		if (ZwQueryObject == NULL) 
		{ 
			break; 
		} 

		//获取对象名称
		pObjectName = (PMYUNICODE_STRING)szTemp;
		if (ZwQueryObject(hFile, ObjectNameInformation, pObjectName, sizeof(szTemp)-1 , &dwResult) != 0)
		{ 
			break;
		}  

		wcsncpy_s(szObjectPath, pObjectName->Buffer, pObjectName->Length/sizeof(WCHAR));
		dwResult = WideCharToMultiByte(CP_ACP, 0, szObjectPath, -1, NULL, 0, NULL, NULL);
		if (dwResult > dwBufCount)
		{
			break;
		}
		dwResult = WideCharToMultiByte(CP_ACP, 0, szObjectPath, -1, (LPSTR)lpFilePath, dwBufCount, NULL, NULL);
		bRun  = TRUE;
	} while (FALSE);
	if (hNtDLL)
	{
		FreeLibrary(hNtDLL);
		hNtDLL = NULL;
	}
	return bRun;  
} 

BOOL GetCurrentProcessName(LPSTR lpProcessName, DWORD dwBufCount)
{
	BOOL bRun = FALSE;
	CHAR szBuf[MAX_PATH] = {0};
	do 
	{
		if (lpProcessName == NULL)
		{
			break;	
		}
		GetModuleFileNameA(0, szBuf, MAX_PATH);
		CHAR* tmp = (CHAR*)strrchr(szBuf, '\\');
		if(!tmp )
		{
			break;
		}
		tmp++;
		if (strlen(tmp) > dwBufCount)
		{
			break;
		}
		strncpy_s(lpProcessName,dwBufCount, tmp, strlen(tmp));
		bRun = TRUE;
	} while (FALSE);
	return bRun;
}


//获取当前目录
BOOL GetModelDir(LPSTR lpPath, UINT uiBufSize)
{
	BOOL bRun = FALSE;
	char szDir[MAX_PATH] = {0};
	char szModePath[MAX_PATH] = {0};
	do 
	{
		if (lpPath == NULL)
		{
			break;
		}
		if(0 != GetModuleFileNameA(NULL, szModePath, MAX_PATH))
		{
			char* pTemp = strrchr( szModePath, '\\');
			if ( pTemp )
			{
				*pTemp = 0;
			}
			strncpy_s(lpPath, uiBufSize, szModePath, uiBufSize);
			strcat_s(lpPath,uiBufSize,"\\");
		}
		bRun = TRUE;
	} while (FALSE);
	return bRun;
}

//函数名：CreateDirectoryPath
//功能：创建一条路径
//参数：_pszPath要创建的路径
//返回值：成功，TRUE 失败，FALSE
BOOL CreateDirectoryPath(LPCSTR _pszPath)  
{  
	char szOrig[MAX_PATH] = {0};  

	if (!_pszPath || !*_pszPath)  
	{  
		return FALSE;  
	}  

	DWORD dwAttrib = GetFileAttributesA(_pszPath);  
	if (dwAttrib != -1 && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY))  
	{  
		return TRUE;  
	}  

	CopyMemory(szOrig,_pszPath,strlen(_pszPath));
	//strcpy(szOrig,  _pszPath);  

	LPSTR pszPath = szOrig;  
	if (*szOrig == '\\')  
	{  
		pszPath = szOrig + 1;  
	}  

	while (*pszPath)  
	{  
		if (*pszPath == '\\')  
		{  
			*pszPath = NULL;  
			CreateDirectoryA(szOrig, NULL);  
			*pszPath = '\\';  
		}  
		pszPath ++;  
	}  

	CreateDirectoryA(szOrig, NULL);  
	dwAttrib = GetFileAttributesA(_pszPath);  
	return (dwAttrib != -1 && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));  
}

 BOOL CreateDirectoryPathW(LPCWSTR _pwzPath)
 {
	 WCHAR wszOrig[MAX_PATH] = {0}; 

	 if (!_pwzPath || !*_pwzPath)  
	 {  
		 return FALSE;  
	 }  

	 DWORD dwAttrib = GetFileAttributesW(_pwzPath);  
	 if (dwAttrib != -1 && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY))  
	 {  
		 return TRUE;  
	 }  

	 CopyMemory(wszOrig,_pwzPath, wcslen(_pwzPath)*sizeof(WCHAR));

	 LPWSTR pwzPath = wszOrig;  
	 if (*wszOrig == L'\\')  
	 {  
		 pwzPath = wszOrig + 1;  
	 }  

	 while (*pwzPath)  
	 {  
		 if (*pwzPath == L'\\')  
		 {  
			 *pwzPath = NULL;  
			 CreateDirectoryW(wszOrig, NULL);  
			 *pwzPath = L'\\';  
		 }  
		 pwzPath ++;  
	 }  

	 CreateDirectoryW(wszOrig, NULL);  
	 dwAttrib = GetFileAttributesW(_pwzPath);  
	 return (dwAttrib != -1 && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));  
 }

BOOL RemoveDir(const char* pszDirectoryPath)
{
	//是目录
	if (GetFileAttributesA(pszDirectoryPath) & FILE_ATTRIBUTE_DIRECTORY)
	{
		BOOL bRet = TRUE;
		WIN32_FIND_DATAA findFileData;
		char tempFileFind[500] = {0}; 
		sprintf_s(tempFileFind,"%s\\*.*",pszDirectoryPath); 
		//打开文件
		HANDLE hFind = ::FindFirstFileA(tempFileFind, &findFileData);
		if (INVALID_HANDLE_VALUE == hFind)
		{
			return FALSE;
		}
		while(bRet) 
		{ 
			//如果是目录
			if(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
			{ 
				//., ..不处理
				if (findFileData.cFileName[0] != '.')
				{
					char tempDir[500] = {0}; 
					sprintf_s(tempDir,"%s\\%s",pszDirectoryPath,findFileData.cFileName); 
					RemoveDir(tempDir); 
				}
			}
			else //文件直接删除
			{ 
				char tempFileName[500] = {0}; 
				sprintf_s(tempFileName,"%s\\%s",pszDirectoryPath,findFileData.cFileName); 
				if(!DeleteFileA(tempFileName))
				{
					D_Log::WriteLog(D_Log::LOG_NORMAL,"[RemoveDir]：删除目录下文件失败 %s Error=%d",tempFileName, GetLastError());
					break;
				}
			}  
			bRet = ::FindNextFileA(hFind, &findFileData);
		} 
		::FindClose(hFind);
		hFind = INVALID_HANDLE_VALUE;
		if(!RemoveDirectoryA(pszDirectoryPath)) 
		{ 
			D_Log::WriteLog(D_Log::LOG_NORMAL,"[RemoveDir]:删除目录失败 %s Error=%d",pszDirectoryPath, GetLastError());
			return FALSE; 
		} 
	}
	else //是文件
	{
		DeleteFileA(pszDirectoryPath); 
	}
	return TRUE;
}

BOOL CopyFileMust(LPCSTR lpSrcFiles, LPCSTR LpDestPath)
{
	USES_CONVERSION;

	BOOL bRun = FALSE;
	char szDestPath[MAX_PATH] = {0}; //目标路径
	char szExpand[MAX_PATH/2] = {0};   //扩展名
	char szFileName[MAX_PATH/2] = {0}; //真实名字
	char szTemp[MAX_PATH] = {0};
	char szRemovePath[MAX_PATH] = {0}; //删除文件所在目录
	//读取产品安装目录 注册表相关
	HKEY hKey = NULL;
	DWORD dwType = REG_SZ;
	DWORD dwCont = MAX_PATH;
	char szSubKey[]=DUL_BOX_REG_NAME;
	do 
	{
		//检查数据有效性和源路径是否存在
		if (lpSrcFiles == NULL || LpDestPath == NULL || !PathFileExistsA(lpSrcFiles))
		{
			D_Log::WriteLog(D_Log::LOG_NORMAL,"[CopyFileMust]:源文件路径不合法或不存在");
			break;
		}
		//检查目的路径的合法性
		LPCSTR pTemp = strrchr( LpDestPath, '\\');
		if (!pTemp )
		{
			D_Log::WriteLog(D_Log::LOG_NORMAL,"[CopyFileMust]:目标路径不是有效的路径格式：%s",LpDestPath);
			break;
		}
		D_Log::WriteLog(D_Log::LOG_NORMAL,"[CopyFileMust]:将要替换的文件：%s %s",lpSrcFiles,LpDestPath);
		strncpy_s(szDestPath,LpDestPath, (pTemp-LpDestPath)/sizeof(TCHAR));
		//创建目标路径
		if (!CreateDirectoryPath(szDestPath))
		{
			D_Log::WriteLog(D_Log::LOG_NORMAL,"[CopyFileMust]:创建目的路径失败：%s",szDestPath);
			break;
		}
		//检测目标文件是否存在
		if (PathFileExistsA(LpDestPath))
		{
			D_Log::WriteLog(D_Log::LOG_NORMAL,"[CopyFileMust]:目的路径下已经存在了该文件：%s",LpDestPath);
			//获得扩展名
			LPCSTR pExd = strrchr(LpDestPath, '.');
			if (!pExd )
			{
				break;
			}
			LPCSTR pName = strrchr(LpDestPath, '//');
			if (!pName )
			{
				pName = strrchr(LpDestPath, '\\');
				if (!pName)
				{
					break;
				}
			}
			//总字符长度-前面的字符长度
			int iLen = strlen(LpDestPath) - (pExd-LpDestPath);
			//获得扩展名
			strncpy_s(szExpand, pExd, iLen);
			//获得文件名(去掉目录标示)
			strncpy_s(szFileName, pName+1, pExd-pName-1);
			//组合重命名名字
			strncpy_s(szTemp, szDestPath, MAX_PATH);
			strcat_s(szTemp, "\\");
			strcat_s(szTemp, szFileName);
			strcat_s(szTemp, "_old");
			strcat_s(szTemp, szExpand);
			//存在改名删除
			if(!MoveFileA(LpDestPath, szTemp))
			{
				D_Log::WriteLog(D_Log::LOG_NORMAL,"[CopyFileMust]:将源文件改名失败：%s %s",LpDestPath, szTemp);
				break;
			}
			//读取注册表获取系统安装目录(没有的话，就在当前目录下创建临时文件夹)
			if(::RegOpenKeyExA(HKEY_LOCAL_MACHINE ,szSubKey, NULL, KEY_READ|KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS)
			{
				RegQueryValueExA(hKey, "Path", NULL, &dwType, (LPBYTE)szRemovePath, &dwCont);
				::RegCloseKey(hKey);
				hKey = NULL;
			}
			if (0 != strcmp(szRemovePath, ""))
			{
				strcat_s(szRemovePath, "\\");
			}
			strcat_s(szRemovePath, T2A(UPDATA_TEMP));
			strcat_s(szRemovePath, "\\");
			strcat_s(szRemovePath, T2A(REMOVE_DIR));
			CreateDirectoryPath(szRemovePath);
			strcat_s(szRemovePath, "\\");
			strcat_s(szRemovePath, szFileName);
			strcat_s(szRemovePath, szExpand);
			if(!MoveFileA(szTemp, szRemovePath))
			{
				D_Log::WriteLog(D_Log::LOG_NORMAL,"[CopyFileMust]:将临时文件移动到将要删除的目录失败：%s %s",szTemp, szRemovePath);
				break;
			}
		}
		bRun = CopyFileA(lpSrcFiles, LpDestPath, FALSE);
	} while (false);
	if (!bRun)
	{
		D_Log::WriteLog(D_Log::LOG_NORMAL,"[CopyFileMust]:拷贝最终的文件失败：%s %s Error=%d",lpSrcFiles, LpDestPath, GetLastError());
	}
	return bRun;
}

BOOL RemoveUpdataTempDir()
{
	USES_CONVERSION;

	char szModePath[MAX_PATH] = {0};
	GetModelDir(szModePath, MAX_PATH);
	strcat_s(szModePath, T2A(UPDATA_TEMP));
	strcat_s(szModePath, "\\");
	strcat_s(szModePath, T2A(REMOVE_DIR));
	return RemoveDir(szModePath);
}


BOOL MoveFileWithFlag(LPCSTR lpFilePath, LPCSTR lpVersion)
{
	USES_CONVERSION;

	BOOL bRun  = FALSE;
	char szRealPath[MAX_PATH] = {0};   //将要拷贝的真实路径
	char szExpand[MAX_PATH/2] = {0};   //扩展名
	char szFileName[MAX_PATH/2] = {0}; //真实名字
	char szModePath[MAX_PATH] = {0};   //中间BUF
	HKEY hKey = NULL;
	DWORD dwType = REG_SZ;
	DWORD dwCont = MAX_PATH;
	char szSubKey[]=DUL_BOX_REG_NAME;
	do 
	{
		if (lpFilePath == NULL)
		{
			break;
		}
		//分解文件名
		LPCSTR pExd = strrchr(lpFilePath, '.');
		if (!pExd )
		{
			break;
		}
		LPCSTR pName = strrchr(lpFilePath, '//');
		if (!pName )
		{
			pName = strrchr(lpFilePath, '\\');
			if (!pName)
			{
				break;
			}
		}
		//总字符长度-前面的字符长度
		int iLen = strlen(lpFilePath) - (pExd-lpFilePath);
		//获得扩展名
		strncpy_s(szExpand, pExd, iLen);
		//获得文件名(去掉目录标示)
		strncpy_s(szFileName, pName+1, pExd-pName-1);
		//去掉版本号
		if (lpVersion != NULL)
		{
			size_t sIndex = 0;
			string strName = szFileName;
			sIndex = strName.rfind(lpVersion);
			if (sIndex != std::string::npos)
			{
				ZeroMemory(szFileName, MAX_PATH/2);
				strncpy_s(szFileName, strName.c_str(), sIndex-1); //-1因为还有一个下划线要去掉
			}

		}
		char tfalg = *(szFileName+strlen(szFileName)-1);
		//去掉标识
		*(szFileName+strlen(szFileName)-1) = 0;
		switch (tfalg)
		{
		case '1':  //系统system32目录
			{
				if(0 != GetSystemDirectoryA(szModePath, MAX_PATH))
				{
					strncpy_s(szRealPath, szModePath, MAX_PATH);
				}
				else
				{
					break;
				}
			}
			break;
		case '0':  //软件安装目录
			{
				//读取注册表获取系统安装目录(没有的话，就在当前目录下创建临时文件夹)
				if(::RegOpenKeyExA(HKEY_LOCAL_MACHINE ,szSubKey, NULL, KEY_READ|KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS)
				{
					RegQueryValueExA(hKey, "Path", NULL, &dwType, (LPBYTE)szModePath, &dwCont);
					::RegCloseKey(hKey);
					hKey = NULL;
				}
				if (0 != strcmp(szModePath, ""))
				{
					if(0 != GetModuleFileNameA(NULL, szModePath, MAX_PATH))
					{

						char* pTemp = strrchr( szModePath, '\\');
						if ( pTemp )
						{
							*pTemp = 0;
						}
						strncpy_s(szRealPath, szModePath, MAX_PATH);
					}
					else
					{
						break;
					}
				}
			}
			break;
			//缺省的也拷贝到到LOSTDIR_TEMP目录下
		default:
			{
				//读取注册表获取系统安装目录(没有的话，就在当前目录下寻找临时文件夹)
				if(::RegOpenKeyExA(HKEY_LOCAL_MACHINE ,szSubKey, NULL, KEY_READ|KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS)
				{
					RegQueryValueExA(hKey, "Path", NULL, &dwType, (LPBYTE)szModePath, &dwCont);
					::RegCloseKey(hKey);
					hKey = NULL;
				}
				if (0 != strcmp(szModePath, ""))
				{
					if(0 != GetModuleFileNameA(NULL, szModePath, MAX_PATH))
					{

						char* pTemp = strrchr( szModePath, '\\');
						if ( pTemp )
						{
							*pTemp = 0;
						}
						strncpy_s(szRealPath, szModePath, MAX_PATH);
					}
					else
					{
						break;
					}
				}
				strcat_s(szRealPath, "\\");
				strcat_s(szRealPath, T2A(UPDATA_TEMP));
				strcat_s(szRealPath, "\\");
				strcat_s(szRealPath, T2A(LOSTDIR_TEMP));
			}
			break;
		}
		strcat_s(szRealPath, "\\");
		strcat_s(szRealPath, szFileName);
		strcat_s(szRealPath, szExpand);
		//拷贝文件
		bRun = CopyFileMust(lpFilePath, szRealPath);
		if(bRun <= 0)
		{
			D_Log::WriteLog(D_Log::LOG_NORMAL,"[CopyFileMust]:失败，%s--->%s Error = %d",lpFilePath, szRealPath, GetLastError());
			break;
		}
		//删除源文件
		bRun = DeleteFileA(lpFilePath);
	} while (false);
	return bRun;
}


BOOL MoveFileWithDirFlag(LPCSTR lpFilePath)
{
	USES_CONVERSION;

	BOOL bRun  = FALSE;
	char szRealPath[MAX_PATH] = {0};   //将要拷贝的真实路径
	char szFileName[MAX_PATH/2] = {0}; //真实名字
	char szModePath[MAX_PATH] = {0};   //中间BUF
	char szTempDir[MAX_PATH] = {0};    //中间缓存目录名
	//读取产品注册表相关
	HKEY hKey = NULL;
	DWORD dwType = REG_SZ;
	DWORD dwCont = MAX_PATH;
	char szSubKey[]=DUL_BOX_REG_NAME;
	do 
	{
		if (lpFilePath == NULL)
		{
			break;
		}


		//获取可执行文件名
		LPCSTR pName = strrchr(lpFilePath, '//');
		if (!pName )
		{
			pName = strrchr(lpFilePath, '\\');
			if (!pName)
			{
				break;
			}
		}
		//总字符长度-前面的字符长度
		int iLen = strlen(lpFilePath) - (pName-lpFilePath);
		//获得文件名
		strncpy_s(szFileName, pName+1, iLen-1);


		//获取前面的目录
		strncpy_s(szModePath, lpFilePath, strlen(lpFilePath)-iLen);
		LPCSTR pTemp = strrchr(szModePath, '\\');
		if (!pTemp )
		{
			break;
		}
		strncpy_s(szTempDir, pTemp+1, strlen(szModePath)-(pTemp-szModePath)-1);

		D_Log::WriteLog(D_Log::LOG_NORMAL,"[MoveFileWithDirFlag]:获取所要移动的文件目录标识 %s",szTempDir);

		if (strcmp(szTempDir, SYSDIR_TEMP) == 0) //系统system32
		{
			if(0 != GetSystemDirectoryA(szModePath, MAX_PATH))
			{
				strncpy_s(szRealPath, szModePath, MAX_PATH);
			}
			else
			{
				break;
			}
		}
		else if(strcmp(szTempDir, MODULE_TEMP) == 0) //产品安装目录
		{
			
			//读取注册表获取系统安装目录(没有的话，就在当前目录下寻找临时文件夹)
			if(::RegOpenKeyExA(HKEY_LOCAL_MACHINE ,szSubKey, NULL, KEY_READ|KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS)
			{
				RegQueryValueExA(hKey, "Path", NULL, &dwType, (LPBYTE)szRealPath, &dwCont);
				::RegCloseKey(hKey);
				hKey = NULL;
			}
			//如果读取注册表失败
			if (0 == strcmp(szRealPath, ""))
			{
				//释放到当前目录
				if(0 != GetModuleFileNameA(NULL, szModePath, MAX_PATH))
				{

					char* pTemp = strrchr( szModePath, '\\');
					if ( pTemp )
					{
						*pTemp = 0;
					}
					strncpy_s(szRealPath, szModePath, MAX_PATH);
					D_Log::WriteLog(D_Log::LOG_NORMAL,"[MoveFileWithDirFlag]:读取注册表失败，获取当前模块路径：%s",szRealPath);
				}
				else
				{
					break;
				}
			}
		}
		else if (strcmp(szTempDir, DIRVER_TEMP) == 0)
		{
			if(0 != GetSystemDirectoryA(szModePath, MAX_PATH))
			{
				strncpy_s(szRealPath, szModePath, MAX_PATH);
				strcat_s(szRealPath, "\\drivers");
			}
			else
			{
				break;
			}
		}
		else
		{
			GetModelDir(szRealPath, MAX_PATH);
			strcat_s(szRealPath, "\\");
			strcat_s(szRealPath, T2A(LOSTDIR_TEMP));
		}
	
		strcat_s(szRealPath, "\\");
		strcat_s(szRealPath, szFileName);
		D_Log::WriteLog(D_Log::LOG_NORMAL,"[MoveFileWithDirFlag]:拷贝文件的目录路径组合完毕:%s",szRealPath);
		//拷贝文件
		if(!CopyFileMust(lpFilePath, szRealPath))
		{
			D_Log::WriteLog(D_Log::LOG_NORMAL,"[CopyFileMust]:替换文件失败");
			break;
		}
		//删除源文件
		bRun = DeleteFileA(lpFilePath);
	} while (false);
	return bRun;
}

BOOL MoveDirWithDirFlag(LPCSTR lpDirPath)
{
	BOOL bRun = TRUE;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	if (lpDirPath == NULL)
	{
		return FALSE;
	}
	//是目录
	if (GetFileAttributesA(lpDirPath) & FILE_ATTRIBUTE_DIRECTORY)
	{
		BOOL bRet = TRUE;
		WIN32_FIND_DATAA findFileData;
		char tempFileFind[500] = {0}; 
		sprintf_s(tempFileFind,"%s\\*.*",lpDirPath); 
		//打开文件
		hFind = ::FindFirstFileA(tempFileFind, &findFileData);
		if (INVALID_HANDLE_VALUE == hFind)
		{
			return FALSE;
		}
		while(bRet) 
		{ 
			//如果是目录
			if(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
			{ 
				//., ..不处理
				if (findFileData.cFileName[0] != '.')
				{
					char tempDir[500] = {0}; 
					sprintf_s(tempDir,"%s\\%s",lpDirPath,findFileData.cFileName); 
					if(!MoveDirWithDirFlag(tempDir))
					{
						bRun  = FALSE;
						break;
					}
				}
			}
			else //文件直接移动
			{ 
				char tempFileName[500] = {0}; 
				sprintf_s(tempFileName,"%s\\%s",lpDirPath,findFileData.cFileName); 
				if(!MoveFileWithDirFlag(tempFileName))
				{
					bRun  = FALSE;
					break;
				}
			}  
			bRet = ::FindNextFileA(hFind, &findFileData);
		}
	}
	else //是文件
	{
		bRun = MoveFileWithDirFlag(lpDirPath); 
	}
	::FindClose(hFind);
	hFind = INVALID_HANDLE_VALUE;
	return bRun;
}


int String2Bytes(unsigned char* szSrc, unsigned char* pDst, int nDstMaxLen)
{	
	int iLen = strlen((char *)szSrc);
	int i = 0;

	if(szSrc == NULL)	
	{	
		return 0;
	}	
	if (iLen <= 0 || iLen%2 != 0 || pDst == NULL || nDstMaxLen < iLen/2)
	{	
		return 0;
	}	


	_strupr_s((char *)szSrc,iLen+1);	
	for (i=0; i<iLen/2; i++)	
	{		
		int iVal = 0;
		unsigned char *pSrcTemp = szSrc + i*2;	
		sscanf_s((char *)pSrcTemp, "%02x", &iVal);	
		pDst[i] = (unsigned char)iVal;
	}
	return iLen;
} 

WORD GetMonthDay(WORD wYear, WORD wMonth)
{
	int iDayYear = 0;
	int iDayMonth = 0;
	//先判断一年有多少天
	if(wYear%4==0 && wYear%100 != 0 || wYear%400 == 0)
	{
		iDayYear=366;
	}
	else
	{
		iDayYear=365;
	}

	switch(wMonth)
	{
	case(1):
	case(3):
	case(5):
	case(7):
	case(8):
	case(10):
	case(12):
		{
			iDayMonth=31;
		}
		break;
	case(4):
	case(6):
	case(9):
	case(11):
		{
			iDayMonth=30;
		}
		break;
	case(2):
		{
			if(iDayYear == 366) 
			{
				iDayMonth=29;
			}
			else 
			{
				iDayMonth=28;
			}
		}
		break;
	default:
		break;
	}
	return iDayMonth;
}

void GetMondy(SYSTEMTIME *pST)
{
	SYSTEMTIME &st = *pST;

	switch (st.wDayOfWeek)
	{
	case 0:
		st.wDayOfWeek = 7;
	case 1: //周一
	case 2: //周二
	case 3: //周三
	case 4: //周四
	case 5: //周五
	case 6: //周六
		if(st.wDay <= st.wDayOfWeek-1)
		{			
			if(st.wMonth == 1)
			{
				st.wYear -= 1;
				st.wMonth = 12;
			}
			else
			{
				st.wMonth -= 1;				
			}			
			st.wDay = GetMonthDay(st.wYear, st.wMonth) - (st.wDayOfWeek-st.wDay-1);
		}
		else
		{
			st.wDay -= ((st.wDayOfWeek-1));
		}
		break;
	}	
	st.wHour = 0;
	st.wMinute = 0;
	st.wSecond = 0;
}

void GetNextDay(SYSTEMTIME *pST)
{
	SYSTEMTIME &st = *pST;
	WORD wDay = GetMonthDay(st.wYear, st.wMonth);
	if(st.wDay == wDay)
	{
		if(st.wMonth == 12)
		{
			st.wYear += 1;
			st.wMonth = 1;
		}
		else
		{
			st.wMonth += 1;
		}
		st.wDay = 1;
	}
	else
	{
		st.wDay += 1;		
	}
	st.wHour = 0;
	st.wMinute = 0;
	st.wSecond = 0;
}

BOOL GetProductDirA(char* pDir, DWORD dwSize)
{
	BOOL bRun = FALSE;
	HKEY hKey = NULL;
	DWORD dwType = REG_SZ;
	DWORD dwCont = MAX_PATH;
	char szSubKey[]=DUL_BOX_REG_NAME;
	do 
	{
		if (pDir == NULL || dwSize == 0)
		{
			break;
		}
		dwCont = dwSize;
		//获取安装目录
		if(::RegOpenKeyExA(HKEY_LOCAL_MACHINE ,szSubKey, NULL, KEY_READ|KEY_WOW64_64KEY, &hKey) != ERROR_SUCCESS)
		{
			OutputDebugString(_T("RegOpenKeyExA is  fail"));		
			MzOutputDebugStringA("[PortHook]:GetProductDirA  RegOpenKeyExA 失败 Error Code:%d\n",GetLastError());
			break;
		}
		if(::RegQueryValueExA(hKey, "Path", NULL, &dwType, (LPBYTE)pDir, &dwCont) != ERROR_SUCCESS)
		{
			OutputDebugString(_T("::RegQueryValueExA is  fail"));
			MzOutputDebugStringA("[PortHook]:GetProductDirA RegQueryValueExA 失败 Error Code:%d\n",GetLastError());
			break;
		}
		bRun = TRUE;
	} while (FALSE);
	if (hKey != NULL)
	{
		::RegCloseKey(hKey);
	}
	return bRun;
}

BOOL GetProductDirW(wchar_t* pwDir, DWORD dwSize)
{
	BOOL bRun = FALSE;
	HKEY hKey = NULL;
	DWORD dwType = REG_SZ;
	DWORD dwCont = MAX_PATH;
	wchar_t wszSubKey[]=DUL_BOX_REG_NAMEW;
	do 
	{
		if (pwDir == NULL || dwSize == 0)
		{
			break;
		}
		dwCont = dwSize;
		//获取安装目录
		if(::RegOpenKeyExW(HKEY_LOCAL_MACHINE ,wszSubKey, NULL, KEY_READ|KEY_WOW64_64KEY, &hKey) != ERROR_SUCCESS)
		{
			break;
		}
		if(::RegQueryValueExW(hKey, L"Path", NULL, &dwType, (LPBYTE)pwDir, &dwCont) != ERROR_SUCCESS)
		{
			break;
		}
		bRun = TRUE;
	} while (FALSE);
	if (hKey != NULL)
	{
		::RegCloseKey(hKey);
	}
	return bRun;
 }
// 
// VOID D_Log::WriteLog(D_Log::LOG_NORMAL,LPCSTR pMsg, ... )
// {	
// 	FILE* pFile = NULL;
// 	va_list marker=NULL;
// 	SYSTEMTIME systemTime;
// 	char szLogFigPaht[MAX_PATH] = {0};
// 	char szFileName[MAX_PATH] = {0};
// 	char* lpMsg = NULL;
// 	int iCount = 1;
// 	int iRun = 0;
// 	//循环申请内存，防止大小不够
// 	do 
// 	{
// 		lpMsg = new char[iCount*MAX_PATH];
// 		if (lpMsg)
// 		{	
// 			ZeroMemory(lpMsg, iCount*MAX_PATH);
// 			//格式化日志信息
// 			va_start(marker ,pMsg);  
// 			iRun = _vsnprintf(lpMsg, iCount*MAX_PATH, pMsg,marker);
// 			va_end(marker);
// 		}
// 		if (iRun < 0)
// 		{
// 			delete[] lpMsg;
// 			lpMsg = NULL;
// 			iCount++;
// 		}
// 		else
// 		{
// 			break;
// 		}
// 	} while (TRUE);
// 	int iError = 0;
// 	GetProductDirA(szLogFigPaht, MAX_PATH);
// 	if (0 == strcmp(szLogFigPaht, ""))
// 	{
// 		//获取失败，就获取当前目录
// 		GetModelDir(szLogFigPaht, MAX_PATH);
// 	}
// 	else
// 	{
// 		strcat_s(szLogFigPaht, "\\");
// 	}
// 	strcat_s(szLogFigPaht, LOG_FILE_PARH);
// 	CreateDirectoryPath(szLogFigPaht);
// 	//获得系统时间
// 	::GetLocalTime(&systemTime);
// 	//获得当前文件名字
// 	sprintf_s(szFileName, "%d-%02d-%02d.log", systemTime.wYear, systemTime.wMonth, systemTime.wDay);
// 	//文件完成路径
// 	sprintf_s(szLogFigPaht, "%s\\%s", szLogFigPaht, szFileName);
// 	//打开文件
// 	fopen_s(&pFile,szLogFigPaht, "a+");
// 	if (pFile != NULL && lpMsg != NULL)
// 	{
// 		//写文件头
// 		sprintf_s(szFileName, "%02d-%02d-%02d", systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
// 		fprintf(pFile,"【%s】:",szFileName);
// 		fprintf(pFile, "%s",lpMsg);
// 		fclose(pFile);
// 	}
// 
// 	if (lpMsg != NULL)
// 	{
// 		delete[] lpMsg;
// 		lpMsg = NULL;
// 	}
// }

VOID FilterFile(vector<string>& vecIn)
{
	vector<string> vectorTemp;
	vector<string>::iterator itor;
	for (itor = vecIn.begin(); itor != vecIn.end(); ++ itor)
	{
		size_t dwIndex = (*itor).find(".log");
		if (dwIndex == string::npos)
		{
			continue;
		}
		vectorTemp.push_back(*itor);

	}
	vecIn.swap(vectorTemp);

}

VOID SaveLogInThirtyDays()
{
	char szLogDir[MAX_PATH] = {0};
	char szLogFigPaht[MAX_PATH] = {0};
	char szFileName[MAX_PATH] = {0};
	SYSTEMTIME systemTime;
	//删除那天的年月日
	WORD wYear = 0;
	WORD wDay = 0;
	WORD wMonth = 0;
	WORD wFlag  = 0;
	//获取日志路径
	if(GetProductDirA(szLogDir, MAX_PATH))
	{
		strcat_s(szLogDir, "\\");
	}
	strcat_s(szLogDir, LOG_FILE_PARH);

	vector<string> vecAllFileInfo;
	DirectoryList(szLogDir, vecAllFileInfo);
	// 过滤掉非 *.log 文件
	FilterFile(vecAllFileInfo);
	
	//获得系统时间
	::GetLocalTime(&systemTime);
	//找到需要删除 具体某一天之前的所有日志

	do 
	{
		if ( systemTime.wDay >= 30)
		{
			// 这天在本月内的第一天
			wDay =systemTime.wDay-29;
			wYear = systemTime.wYear;
			wMonth = systemTime.wMonth;
			break;
		}
		if (systemTime.wMonth == 1) //这个月是一月
		{
			wMonth  = 12;
			wYear = systemTime.wYear-1;
			wDay = systemTime.wDay + 2;
			break;
		}else{
			wMonth = systemTime.wMonth-1;
			wYear = systemTime.wYear;

			//算出上个月有多少天
			WORD nDay = GetMonthDay(wYear, wMonth);
			//算出上周日是几号
			wDay = nDay+systemTime.wDay-29;
		}

	} while (FALSE);
	
	vector<string>::iterator itor;
	for (itor = vecAllFileInfo.begin(); itor!= vecAllFileInfo.end(); ++itor)
	{
		SYSTEMTIME systemTimeOneDay;
		char driver[512] = "", path[512] = "", filename[512] = "", ext[512] = "";
		_splitpath((*itor).c_str(), driver, path, filename, ext);

		bool bIsSuc = StringConvertDateFormat(string(filename),systemTimeOneDay.wYear, systemTimeOneDay.wMonth,systemTimeOneDay.wDay);
		if (!bIsSuc)
		{
			continue;
		}
		SYSTEMTIME systemTimeRightPar;
		systemTimeRightPar.wYear = wYear;
		systemTimeRightPar.wMonth = wMonth;
		systemTimeRightPar.wDay = wDay;
		bIsSuc = bIsLeftDateLessThanRightDate(systemTimeOneDay, systemTimeRightPar );
		if (bIsSuc)
		{
			DeleteFileA((*itor).c_str());
		}
	}
	
}

// VOID SaveHandoverFileInThirtyDays()
// {
// 	char szHandoverDir[MAX_PATH]={0};
// 	SYSTEMTIME systemTime;
// 	//删除那天的年月日
// 	WORD wYear = 0;
// 	WORD wDay = 0;
// 	WORD wMonth = 0;
// 	WORD wFlag  = 0;
// 	SYSTEMTIME systemTimeRightPar;

// 	vector<string> vecAllFileInfo;
// 	vector<string>::iterator itor;
	

// 	//获取文件保存目录路径
// 	if(GetProductDirA(szHandoverDir, MAX_PATH))
// 	{
// 		strcat(szHandoverDir, "\\");
// 	}
// 	strcat(szHandoverDir, D_BOX_HANDOVERS_FILEDIRA);

// 	//获取目录下的文件
// 	DirectoryList(szHandoverDir, vecAllFileInfo);

// 	//获得系统时间
// 	::GetLocalTime(&systemTime);
// 	//找到需要删除 具体某一天之前的所有文件
// 	do 
// 	{
// 		if ( systemTime.wDay >= 30)
// 		{
// 			// 这天在本月内的第一天
// 			wDay =systemTime.wDay-29;
// 			wYear = systemTime.wYear;
// 			wMonth = systemTime.wMonth;
// 			break;
// 		}
// 		if (systemTime.wMonth == 1) //这个月是一月
// 		{
// 			wMonth  = 12;
// 			wYear = systemTime.wYear-1;
// 			wDay = systemTime.wDay + 2;
// 			break;
// 		}else{
// 			wMonth = systemTime.wMonth-1;
// 			wYear = systemTime.wYear;

// 			//算出上个月有多少天
// 			WORD nDay = GetMonthDay(wYear, wMonth);
// 			//算出上周日是几号
// 			wDay = nDay+systemTime.wDay-29;
// 		}
// 	} while (FALSE);

// 	//格式化这个时间点为SYSTEMTIME结构体
// 	systemTimeRightPar.wYear = wYear;
// 	systemTimeRightPar.wMonth = wMonth;
// 	systemTimeRightPar.wDay = wDay;

// 	for (itor = vecAllFileInfo.begin(); itor!= vecAllFileInfo.end(); ++itor)
// 	{
// 		//获取文件的创建时间
// 		HANDLE hFile = CreateFileA((*itor).c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
// 		if (hFile != INVALID_HANDLE_VALUE)
// 		{
// 			SYSTEMTIME systemTimeOneDay;
// 			FILETIME ftCreate, ftModify, ftAccess;
// 			if (GetFileTime(hFile, &ftCreate, &ftAccess, &ftModify))
// 			{
// 				FileTimeToSystemTime(&ftCreate, &systemTimeOneDay);
// 				//判断文件是不是超过保存时间区间之前创建的
// 				if (bIsLeftDateLessThanRightDate(systemTimeOneDay, systemTimeRightPar ))
// 				{
// 					CloseHandle(hFile);
// 					DeleteFileA((*itor).c_str()); //删除文件
// 				}
// 			}
// 		}
// 	}
// }

BOOL bIsLeftDateLessThanRightDate(SYSTEMTIME& lp, SYSTEMTIME& rp)
{
	if (lp.wYear < rp.wYear)
	{
		return TRUE;
	}
	if (lp.wYear == rp.wYear && lp.wMonth < rp.wMonth)
	{
		return TRUE;
	}
	if (lp.wYear == rp.wYear && lp.wMonth == rp.wMonth && lp.wDay <rp.wDay)
	{
		return TRUE;
	}
	return FALSE;
}

VOID DeletLogWithTime()
{
	char szLogDir[MAX_PATH] = {0};
	char szLogFigPaht[MAX_PATH] = {0};
	char szFileName[MAX_PATH] = {0};
	SYSTEMTIME systemTime;
	//删除那天的年月日
	WORD wYear = 0;
	WORD wDay = 0;
	WORD wMonth = 0;
	WORD wFlag  = 0;
	//获取日志路径
	if (GetProductDirA(szLogDir, MAX_PATH))
	{
		_tcscat_s(szLogDir, _T("\\"));
	}
	strcat_s(szLogDir, LOG_FILE_PARH);
	//获得系统时间
	::GetLocalTime(&systemTime);
	//循环计算日期



	for (WORD i = 0; i<365; i++)
	{
		// 获得几天前
		if (systemTime.wDayOfWeek == 0)
		{
			wFlag= 30 + i;
		}
		else
		{
			wFlag = systemTime.wDayOfWeek+ i;
		}

		if (systemTime.wDay > wFlag) //这天在本月内
		{
			wDay  = systemTime.wDay - wFlag;
			wMonth = systemTime.wMonth;
			wYear = systemTime.wYear;
		}
		else //这天在上个月
		{
			if (systemTime.wMonth == 1) //这个月是一月
			{
				wMonth  = 12;
				wYear = systemTime.wYear-1;
			}
			else //上个月不是一月
			{

				wMonth = systemTime.wMonth-1;
				wYear = systemTime.wYear;

			}
			//算出上个月有多少天
			WORD nDay = GetMonthDay(wYear, wMonth);
			//算出上周日是几号
			wDay = nDay-(wFlag-systemTime.wDay);
		}	
		//文件完成路径
		sprintf_s(szFileName, "%d-%02d-%02d.log", wYear, wMonth, wDay);
		sprintf_s(szLogFigPaht, "%s\\%s", szLogDir, szFileName);
		//删除那天的日志
		DeleteFileA(szLogFigPaht);
	}
}



static const char * My_inet_ntop_v6 (const u_char *src, char *dst, size_t size)
{
  /*
   * Note that int32_t and int16_t need only be "at least" large enough
   * to contain a value of the specified size.  On some systems, like
   * Crays, there is no such thing as an integer variable with 16 bits.
   * Keep this in mind if you think this function should have been coded
   * to use pointer overlays.  All the world's not a VAX.
   */
  char  tmp [INET6_ADDRSTRLEN+1];
  char *tp;
  struct {
    long base;
    long len;
  } best, cur;
  u_long words [IN6ADDRSZ / INT16SZ];
  int    i;
 
  /* Preprocess:
   *  Copy the input (bytewise) array into a wordwise array.
   *  Find the longest run of 0x00's in src[] for :: shorthanding.
   */
  memset (words, 0, sizeof(words));
  for (i = 0; i < IN6ADDRSZ; i++)
      words[i/2] |= (src[i] << ((1 - (i % 2)) << 3));
 
  best.base = -1;
  cur.base  = -1;
  for (i = 0; i < (IN6ADDRSZ / INT16SZ); i++)
  {
    if (words[i] == 0)
    {
      if (cur.base == -1)
           cur.base = i, cur.len = 1;
      else cur.len++;
    }
    else if (cur.base != -1)
    {
      if (best.base == -1 || cur.len > best.len)
         best = cur;
      cur.base = -1;
    }
  }
  if ((cur.base != -1) && (best.base == -1 || cur.len > best.len))
     best = cur;
  if (best.base != -1 && best.len < 2)
     best.base = -1;
 
  /* Format the result.
   */
  tp = tmp;
  for (i = 0; i < (IN6ADDRSZ / INT16SZ); i++)
  {
    /* Are we inside the best run of 0x00's?
     */
    if (best.base != -1 && i >= best.base && i < (best.base + best.len))
    {
      if (i == best.base)
         *tp++ = ':';
      continue;
    }
 
    /* Are we following an initial run of 0x00s or any real hex?
     */
    if (i != 0)
       *tp++ = ':';
 
    /* Is this address an encapsulated IPv4?
     */
    if (i == 6 && best.base == 0 &&
        (best.len == 6 || (best.len == 5 && words[5] == 0xffff)))
    {
      if (!My_inet_ntop_v4(src+12, tp, sizeof(tmp) - (tp - tmp)))
      {
        errno = ENOSPC;
        return (NULL);
      }
      tp += strlen(tp);
      break;
    }
    tp += sprintf (tp, "%lX", words[i]);
  }
 
  /* Was it a trailing run of 0x00's?
   */
  if (best.base != -1 && (best.base + best.len) == (IN6ADDRSZ / INT16SZ))
     *tp++ = ':';
  *tp++ = '\0';
 
  /* Check for overflow, copy, and we're done.
   */
  if ((size_t)(tp - tmp) > size)
  {
    errno = ENOSPC;
    return (NULL);
  }
  return strcpy (dst, tmp);
  return (NULL);
}


 const char * My_inet_ntop_v4(const void *src, char *dst, size_t size)
{
	const char digits[] = "0123456789";
	int i;
	struct in_addr *addr = (struct in_addr *)src;
	u_long a = ntohl(addr->s_addr);
	const char *orig_dst = dst;

	if (size < INET_ADDRSTRLEN) {
		errno = ENOSPC;
		return NULL;
	}
	for (i = 0; i < 4; ++i) {
		int n = (a >> (24 - i * 8)) & 0xFF;
		int non_zerop = 0;

		if (non_zerop || n / 100 > 0) {
			*dst++ = digits[n / 100];
			n %= 100;
			non_zerop = 1;
		}
		if (non_zerop || n / 10 > 0) {
			*dst++ = digits[n / 10];
			n %= 10;
			non_zerop = 1;
		}
		*dst++ = digits[n];
		if (i != 3)
			*dst++ = '.';
	}
	*dst++ = '\0';
	return orig_dst;
}


const char * My_inet_ntop(int af, const void *src, char *dst, size_t size)
{
	switch (af) {
	case AF_INET :
		return My_inet_ntop_v4 (src, dst, size);
#ifdef INET6
	case AF_INET6:
		return My_inet_ntop_v6 ((const u_char*)src, dst, size);
#endif
	default :
		errno = EAFNOSUPPORT;
		return NULL;
	}
}


string ZBase64::Encode(const unsigned char* Data,int DataByte)
{
	//编码表
	const char EncodeTable[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	//返回值
	string strEncode;
	unsigned char Tmp[4]={0};
	int LineLength=0;
	for(int i=0;i<(int)(DataByte / 3);i++)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		Tmp[3] = *Data++;
		strEncode+= EncodeTable[Tmp[1] >> 2];
		strEncode+= EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];
		strEncode+= EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];
		strEncode+= EncodeTable[Tmp[3] & 0x3F];
		if(LineLength+=4,LineLength==76) {strEncode+="\r\n";LineLength=0;}
	}
	//对剩余数据进行编码
	int Mod=DataByte % 3;
	if(Mod==1)
	{
		Tmp[1] = *Data++;
		strEncode+= EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode+= EncodeTable[((Tmp[1] & 0x03) << 4)];
		strEncode+= "==";
	}
	else if(Mod==2)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		strEncode+= EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode+= EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];
		strEncode+= EncodeTable[((Tmp[2] & 0x0F) << 2)];
		strEncode+= "=";
	}

	return strEncode;
}

string ZBase64::Decode(const char* Data,int DataByte,int& OutByte)
{
	//解码表
	const char DecodeTable[] =
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		62, // '+'
		0, 0, 0,
		63, // '/'
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
		0, 0, 0, 0, 0, 0, 0,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
		13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
		0, 0, 0, 0, 0, 0,
		26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
		39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
	};
	//返回值
	string strDecode;
	int nValue;
	int i= 0;
	while (i < DataByte)
	{
		if (*Data != '\r' && *Data!='\n')
		{
			nValue = DecodeTable[*Data++] << 18;
			nValue += DecodeTable[*Data++] << 12;
			strDecode+=(nValue & 0x00FF0000) >> 16;
			OutByte++;
			if (*Data != '=')
			{
				nValue += DecodeTable[*Data++] << 6;
				strDecode+=(nValue & 0x0000FF00) >> 8;
				OutByte++;
				if (*Data != '=')
				{
					nValue += DecodeTable[*Data++];
					strDecode+=nValue & 0x000000FF;
					OutByte++;
				}
			}
			i += 4;
		}
		else// 回车换行,跳过
		{
			Data++;
			i++;
		}
	}
	return strDecode;
}


bool AdjustProcessTokenPrivlege()
{
	LUID luidTmp;
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		//printf("AdjustProcessTokenPrivlege OpenProcessToken failed!\n");
		return false;
	}
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luidTmp))
	{
		//printf("AdjustProcessTokenPrivlege LookupPrivilegeValue failed!\n");
		CloseHandle(hToken);
		return false;
	}
	tkp.PrivilegeCount =1;
	tkp.Privileges[0].Luid = luidTmp;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
	{
		//printf("AdjustProcessTokenPrivlege AdjustTokenPrivileges failed!\n");
		CloseHandle(hToken);
		return false;
	}
	return true;
}


/*****************************************
根据进程名获得进程句柄
RUN:进程句柄，失败返回INVALID_HANDLE_VALUE
*****************************************/
HANDLE GetprocessHandByName(PWCHAR lpProcessname)
{
	WCHAR szMsg[MAX_PATH] = {0};
	BOOL report = TRUE;
	HANDLE hModule = INVALID_HANDLE_VALUE;
	PROCESSENTRY32W* pinfo = NULL;
	DWORD dwExplorerId = 0;
	pinfo=new PROCESSENTRY32W;
	wstring str1;
	wstring str2;
	if (NULL == pinfo)
	{
		return NULL;
	}
	pinfo->dwSize=sizeof(PROCESSENTRY32W);
	hModule=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if (hModule == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}
	report=Process32FirstW(hModule,pinfo);
	while (report)
	{
		str1 = pinfo->szExeFile;
		str2 = lpProcessname;
		//全部转换成大写
		transform(str1.begin(), str1.end(), str1.begin(), toupper);
		transform(str2.begin(), str2.end(), str2.begin(), toupper);

		//swprintf(szMsg, L"EnumProcess name = %s pid =%d\n", pinfo->szExeFile, pinfo->th32ProcessID);
		//OutputDebugStringW(szMsg);

		if((str1.compare(str2) == 0))
		{
			dwExplorerId = pinfo->th32ProcessID;
			//OutputDebugStringW(L"★查找到了该进程★");
			break;
		}
		report=Process32NextW(hModule, pinfo); 
	}
	delete pinfo;
	CloseHandle(hModule);
	hModule = NULL;
	if (dwExplorerId != 0)
	{
		hModule = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwExplorerId);
		/*
		if(NULL == hModule)
		{
			swprintf(szMsg, L"ProcessIsExplorer - OpenProcess Failed , Error Code Is %d , dwProcessId Is %d !\n", GetLastError(), dwExplorerId);
			OutputDebugStringW(szMsg);
		}
		*/
	}
	return hModule;
}


PDWORD GetAllProcessIdByName(PWCHAR lpProcessname, PDWORD pdwCount)
{
	PDWORD pProcessId = NULL;
	HANDLE hModule = INVALID_HANDLE_VALUE;
	PROCESSENTRY32W* pinfo = NULL;
	DWORD dwExplorerId = 0;
	BOOL report = TRUE;
	wstring str1;
	wstring str2;
	vector<DWORD> vProcessId;

	HANDLE hProcess = NULL;

	do 
	{
		if (lpProcessname == NULL ||
			pdwCount == NULL)
		{
			break;
		}
		
		pinfo=new PROCESSENTRY32W;
		if (NULL == pinfo)
		{
			break;
		}
		
		pinfo->dwSize=sizeof(PROCESSENTRY32W);
		hModule=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
		report=Process32FirstW(hModule,pinfo);
		while (report)
		{
			str1 = pinfo->szExeFile;
			str2 = lpProcessname;
			//全部转换成大写
			transform(str1.begin(), str1.end(), str1.begin(), toupper);
			transform(str2.begin(), str2.end(), str2.begin(), toupper);
		
			if((str1.compare(str2) == 0))
			{
				vProcessId.push_back(pinfo->th32ProcessID);  //保存进程ID
			}
			report=Process32NextW(hModule, pinfo); 
		}

		if (vProcessId.size() <= 0)
		{
			break;
		}
		//如果进程ID存在，分配内存保存
		*pdwCount = vProcessId.size();
		pProcessId = new DWORD[vProcessId.size()];
		if (pProcessId == NULL)
		{
			break;
		}

		ZeroMemory((char*)pProcessId, sizeof(DWORD)*vProcessId.size());
		PDWORD pIdIndex = pProcessId;
		vector<DWORD>::iterator beg = vProcessId.begin();
		vector<DWORD>::iterator ed = vProcessId.end();
		while (beg != ed)
		{
			*pIdIndex = *beg;
			pIdIndex++;
			beg++;
		}

		
	} while (false);
	if (pinfo != NULL)
	{
		delete pinfo;
	}
	if (hModule != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hModule);
		hModule = INVALID_HANDLE_VALUE;
	}
	return pProcessId;
}

PDWORD GetAllProcessIdByPath(PWCHAR lpPath, PDWORD pdwCount)
{
	PDWORD pProcessId = NULL;
	HANDLE hModule = INVALID_HANDLE_VALUE;
	PROCESSENTRY32W* pinfo = NULL;
	DWORD dwExplorerId = 0;
	BOOL report = TRUE;
	wstring str1;
	wstring str2;
	vector<DWORD> vProcessId;

	HANDLE hProcess = NULL;

	do 
	{
		if (lpPath == NULL ||
			pdwCount == NULL)
		{
			break;
		}

		pinfo=new PROCESSENTRY32W;
		if (NULL == pinfo)
		{
			break;
		}
		str2 = lpPath;
		transform(str2.begin(), str2.end(), str2.begin(), toupper);

		pinfo->dwSize=sizeof(PROCESSENTRY32W);
		hModule=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
		report=Process32FirstW(hModule,pinfo);
		while (report)
		{
			//尝试打开进程，获取进程完成路径(系统进程会打开失败，这个无所谓)
			hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,pinfo->th32ProcessID);
			if (hProcess != NULL)
			{
				//获取可执行文件所在目录
				WCHAR wszProcessPath[MAX_PATH+1]={0};
				GetModuleFileNameExW(hProcess,NULL,wszProcessPath,MAX_PATH+1);

				CloseHandle(hProcess);
				hProcess = NULL;

				//获取目录
				WCHAR* tmp = (WCHAR*)wcsrchr(wszProcessPath, L'\\');
				if(tmp != NULL )
				{
					*tmp = NULL;
				}
				//路径转换到大写
				str1 = wszProcessPath;
				transform(str1.begin(), str1.end(), str1.begin(), toupper);
				
				//比较路径是否匹配
				if((str1.compare(str2) == 0))
				{
					vProcessId.push_back(pinfo->th32ProcessID);  //保存进程ID
				}	
			}
			report=Process32NextW(hModule, pinfo); 
		}

		if (vProcessId.size() <= 0)
		{
			break;
		}
		//如果进程ID存在，分配内存保存
		*pdwCount = vProcessId.size();
		pProcessId = new DWORD[vProcessId.size()];
		if (pProcessId == NULL)
		{
			break;
		}

		ZeroMemory((char*)pProcessId, sizeof(DWORD)*vProcessId.size());
		PDWORD pIdIndex = pProcessId;
		vector<DWORD>::iterator beg = vProcessId.begin();
		vector<DWORD>::iterator ed = vProcessId.end();
		while (beg != ed)
		{
			*pIdIndex = *beg;
			pIdIndex++;
			beg++;
		}

	} while (false);
	if (pinfo != NULL)
	{
		delete pinfo;
	}
	if (hModule != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hModule);
		hModule = INVALID_HANDLE_VALUE;
	}
	return pProcessId;
}


BOOL IsLoadModule(HANDLE hProcess, LPCSTR lpModulePath)
{
	BOOL bRUn = FALSE;
	DWORD dwNeeded = 0;            //需要多少字节
	DWORD dwModulesCount = 0;      //需要多少个模块句柄（dwNeeded/sizeof(HMODULE)）
	HMODULE* phMods = NULL;        //真正的模块句柄缓冲区
	char szModeName[MAX_PATH] = {0};
	do 
	{
		if (hProcess == INVALID_HANDLE_VALUE || lpModulePath == NULL)
		{
			break;
		}
		//首先获取以下需要多少空间
		if(!EnumProcessModules(hProcess, NULL, 0, &dwNeeded))
		{
			break;
		}
		dwModulesCount = dwNeeded/sizeof(HMODULE);
		phMods = new HMODULE[dwModulesCount+1];

		if (phMods == NULL)
		{
			break;
		}
		ZeroMemory((void*)phMods, sizeof(HMODULE)*(dwModulesCount+1));


		//再次枚举获得模块
		if(!EnumProcessModules(hProcess, phMods, sizeof(HMODULE)*(dwModulesCount+1), &dwNeeded))
		{
			break;
		}

		//循环检测模块
		for (int i = 0; i<dwModulesCount; i++)
		{
			ZeroMemory(szModeName, MAX_PATH);
			if(0 != GetModuleFileNameExA(hProcess,*(phMods+i),szModeName,sizeof(TCHAR)*MAX_PATH))
			{
				if(strcmp(szModeName, lpModulePath) == 0)
				{
					bRUn = TRUE;
					break;
				}

			}
		}
	} while (FALSE);
	if (phMods != NULL)
	{
		delete[] phMods;
		phMods = NULL;
	}
	return bRUn;

}



BOOL WhaterProcess(LPCSTR pprocessNmae)
{
	USES_CONVERSION;

	BOOL bRun = FALSE;
	HANDLE hProcess=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	PROCESSENTRY32* pinfo = NULL;
	pinfo=new PROCESSENTRY32;
	if (NULL == pinfo)
	{
		CloseHandle(hProcess);
		return FALSE;
	}

	pinfo->dwSize=sizeof(PROCESSENTRY32);
	BOOL report = TRUE;	
	report=Process32First(hProcess,pinfo);
	while(report)
	{
		if (stricmp(pprocessNmae, T2A(pinfo->szExeFile)) == 0)
		{
			bRun = TRUE;
			break;
		}
		report=Process32Next(hProcess, pinfo); 
	}
	delete pinfo;
	pinfo = NULL;
	CloseHandle(hProcess);
	return bRun;
}

 DWORD GetProcessIdByName(LPCSTR pprocessNmae)
 {
	 USES_CONVERSION;

	 DWORD dwRun = 0;
	 HANDLE hProcess=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	 PROCESSENTRY32* pinfo = NULL;
	 pinfo=new PROCESSENTRY32;
	 if (NULL == pinfo)
	 {
		 CloseHandle(hProcess);
		 return dwRun;
	 }

	 pinfo->dwSize=sizeof(PROCESSENTRY32);
	 BOOL report = TRUE;	
	 report=Process32First(hProcess,pinfo);
	 while(report)
	 {
		 if (stricmp(pprocessNmae, T2A(pinfo->szExeFile)) == 0)
		 {
			 dwRun = pinfo->th32ProcessID;
			 break;
		 }
		 report=Process32Next(hProcess, pinfo); 
	 }
	 delete pinfo;
	 pinfo = NULL;
	 CloseHandle(hProcess);
	 return dwRun;
 }

 BOOL GetRandName(LPSTR pRandName, DWORD dwBufLen)
 {
	 BOOL bRun = FALSE;
	 int iCount = 0;
	 char Name[10]={0};
	 do 
	 {
		 if (pRandName == NULL ||
			 dwBufLen < 10)
		 {
			 break;
		 }
		 srand((int)time(0));
		 while (TRUE)
		 {
			 iCount = random(10);
			 if (iCount >=2 )
			 {
				 break;
			 }
		 }
		 for(int x=0;x<iCount;x++)
		 {
			 if (random(26) / 5 > 3)
			 {
				 Name[x] = 'a'+random(26);
			 }
			 else
			 {
				 Name[x] = 'A'+random(26);
			 }
		 }
		 StringCbPrintf(pRandName, dwBufLen, "%s.exe",Name);
		 bRun = TRUE;
	 } while (false);
	 return bRun;
 }

//  //获取哆啦宝注册表里面保存的随机命名的名字
//  BOOL GetDlbRegRandName(char* pRandName, DWORD dwSize)
//  {
// 	 BOOL bRun = FALSE;
// 	 HKEY hKey = NULL;
// 	 DWORD dwType = REG_SZ;
// 	 DWORD dwCont = MAX_PATH;
// 	 char szSubKey[]=DUL_BOX_REG_NAME;
// 	 do 
// 	 {
// 		 if (pRandName == NULL || dwSize == 0)
// 		 {
// 			 break;
// 		 }
// 		 dwCont = dwSize;
// 		 //获取安装目录
// 		 if(::RegOpenKeyExA(HKEY_LOCAL_MACHINE ,szSubKey, NULL, KEY_READ|KEY_WOW64_64KEY, &hKey) != ERROR_SUCCESS)
// 		 {
// 			 break;
// 		 }
// 		 if(::RegQueryValueExA(hKey, D_RANDNAME_KEY, NULL, &dwType, (LPBYTE)pRandName, &dwCont) != ERROR_SUCCESS)
// 		 {
// 			 break;
// 		 }
// 		 bRun = TRUE;
// 	 } while (FALSE);
// 	 if (hKey != NULL)
// 	 {
// 		 ::RegCloseKey(hKey);
// 	 }
// 	 return bRun;
//  }

//  //将随机启动重命名名字写入注册表
//  BOOL SetAutoRun(char* pRandName, DWORD dwCont)
//  {
// 	 BOOL bRun = FALSE;
// 	 HKEY hKey = NULL;
// 	 DWORD dwType = REG_SZ;
// 	 do
// 	 {
// 		 //获取系统开机自启动
// 		 if (::RegOpenKeyExA(HKEY_LOCAL_MACHINE , DUL_BOX_REG_NAME, NULL, KEY_SET_VALUE | KEY_WOW64_64KEY, &hKey) != ERROR_SUCCESS)
// 		 {
// 			 break;
// 		 }
// 		 //写入注册表
// 		 if (RegSetValueExA(hKey, D_RANDNAME_KEY, 0, dwType, (PBYTE)pRandName, dwCont) != ERROR_SUCCESS)
// 		 {
// 			 break;
// 		 }
// 		 bRun = TRUE;
// 	 }
// 	 while (FALSE);
// 	 if (hKey != NULL)
// 	 {
// 		 ::RegCloseKey(hKey);
// 		 hKey = NULL;
// 	 }
// 	 return bRun;
//  }


BOOL KillProcess(LPCSTR pprocessNmae)
{
	BOOL bRun = TRUE;
	TCHAR tszProcessName[MAX_PATH]={0};
#ifdef _UNICODE
	wsprintfW(tszProcessName, L"%S", pprocessNmae);
#else
	wsprintfA(tszProcessName, "%s", pprocessNmae);
#endif
	HANDLE hProcessInfo=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	PROCESSENTRY32* pinfo = NULL;
	pinfo=new PROCESSENTRY32;
	TCHAR szBuf[MAX_PATH]={0};
	if (NULL == pinfo)
	{
		CloseHandle(hProcessInfo);
		return FALSE;
	}
	pinfo->dwSize=sizeof(PROCESSENTRY32);
	BOOL report = TRUE;	
	report=Process32First(hProcessInfo,pinfo);
	while(report)
	{
		if (_tcsicmp(tszProcessName, pinfo->szExeFile) == 0)
		{
			//名字相同，杀死该进程
			HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE,FALSE,pinfo->th32ProcessID); 
			if (hProcess != NULL)
			{
				if(!::TerminateProcess(hProcess,0))
				{
					_stprintf(szBuf, _T("强杀TerminateProcess失败：PID：%d Error：%d"),pinfo->th32ProcessID, GetLastError());
					OutputDebugString(szBuf);
					bRun = FALSE;                       //有一个没杀死就判断失败，一个都不能有
				}
				CloseHandle(hProcess); 
				hProcess = NULL;
			}
			else
			{
				_stprintf(szBuf, _T("强杀OpenProcesss失败：PID：%d Error：%d"),pinfo->th32ProcessID, GetLastError());
				OutputDebugString(szBuf);
				bRun = FALSE;   
			}
			
		}
		report=Process32Next(hProcessInfo, pinfo); 
	}
	if (bRun)
	{
		OutputDebugString(_T("-----强杀成功-----"));
		OutputDebugStringA(pprocessNmae);
		OutputDebugString(_T("----------------"));
	}
	else
	{
		OutputDebugString(_T("-----强杀失败-----"));
		OutputDebugStringA(pprocessNmae);
		OutputDebugString(_T("----------------"));
	}
	delete pinfo;
	pinfo = NULL;
	CloseHandle(hProcessInfo);
	return bRun;
}


//int CreateUserProcess(LPTSTR filename, STARTUPINFO &siStartInfo, PROCESS_INFORMATION &piProcInfo)
//{
//	DWORD dwSessionId;
//	DWORD dwProcessId;
//
//	HANDLE hProcessSnap = NULL;
//	HANDLE hProcess = NULL;
//	HANDLE hPToken = NULL;
//	HANDLE hUserTokenDup = NULL;
//
//	dwSessionId = WTSGetActiveConsoleSessionId();
//	PROCESSENTRY32 pe32 = {0};
//	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//
//	if (hProcessSnap == (HANDLE)-1)
//	{
//		return FALSE;
//	}
//	pe32.dwSize = sizeof(PROCESSENTRY32);
//	if (Process32First(hProcessSnap, &pe32))
//	{
//		do
//		{
//			if(! stricmp(pe32.szExeFile,"EXPLORER.EXE"))
//			{
//				DWORD winlogonSessId = 0;
//				BOOL bReturn = ProcessIdToSessionId(pe32.th32ProcessID, &winlogonSessId); 
//				DWORD aa = GetLastError();
//				//DEBUG_MSG("ProcessIdToSessionId's errorcode is:%d",aa);
//				if (bReturn && winlogonSessId == dwSessionId)
//				{
//					dwProcessId = pe32.th32ProcessID;
//					//DEBUG_MSG("dwSessionId is:%d",winlogonSessId);
//					break;
//				}
//			}
//		}
//		while (Process32Next(hProcessSnap, &pe32));	
//	}
//	hProcess = OpenProcess(MAXIMUM_ALLOWED, false, dwProcessId);
//	if (NULL != hProcess)
//	{
//		CloseHandle(hProcessSnap);
//	}
//
//	if (!OpenProcessToken(hProcess, TOKEN_DUPLICATE, &hPToken))
//	{
//		CloseHandle(hProcessSnap);
//		CloseHandle(hProcess);
//		return false;
//	}
//
//	
//	char pszObjName[MAX_PATH] ={0};
//	//GetModuleFileName(NULL, pszObjName, MAX_PATH);
//	//PathRemoveFileSpec(pszObjName);
//	GetWindowsDirectory(pszObjName,MAX_PATH);
//	strcat(pszObjName,"\\");
//	strcat(pszObjName,"explorer.exe");
//	SE_OBJECT_TYPE ObjectType = SE_FILE_OBJECT;
//	PACL pOldDACL = NULL;
//	PSECURITY_DESCRIPTOR pSD = NULL;
//
//
//	DWORD dwRes = GetNamedSecurityInfo(pszObjName, 
//		ObjectType, 
//		DACL_SECURITY_INFORMATION, 
//		NULL,
//		NULL, 
//		&pOldDACL,
//		NULL, 
//		&pSD);
//
//	SECURITY_ATTRIBUTES sa;
//	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
//	sa.lpSecurityDescriptor = pSD;
//	sa.bInheritHandle = TRUE;
//	
//
//	if (!DuplicateTokenEx(
//		hPToken, 
//		TOKEN_ALL_ACCESS, 
//		NULL, 
//		SecurityIdentification, 
//		TokenPrimary, 
//		&hUserTokenDup))
//	{
//		DWORD bb = GetLastError();
//		//DEBUG_MSG("DuplicateTokenEx's ErrorCode is:%d",bb);
//		CloseHandle(hProcessSnap);
//		CloseHandle(hProcess);
//		CloseHandle(hPToken);
//		return false;
//	}
//
//	siStartInfo.lpDesktop = "winsta0\\default";
//	int dwCreationFlags = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE;
//
//	BOOL bResult = CreateProcessAsUser(
//		hUserTokenDup,        // client's access token
//		NULL,                   // file to execute
//		filename,        // command line
//		&sa,                 // pointer to process SECURITY_ATTRIBUTES
//		&sa,                 // pointer to thread SECURITY_ATTRIBUTES
//		FALSE,                  // handles are not inheritable
//		dwCreationFlags,        // creation flags
//		0,            // pointer to new environment block 
//		NULL,                   // name of current directory 
//		&siStartInfo,                 // pointer to STARTUPINFO structure
//		&piProcInfo            // receives information about new process
//		);
//	DWORD cc = GetLastError();
//	//DEBUG_MSG("CreateProcessAsUser's ErrorCode is:%d",cc);
//	CloseHandle(hProcessSnap);
//	CloseHandle(hProcess);
//	CloseHandle(hPToken);
//	CloseHandle(hUserTokenDup);
//	return bResult;
//}

__int64 SecondsBase1970(const char *szSysTime)
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
	sscanf(szSysTime, "%d-%02d-%02d %02d:%02d:%02d", &st.wYear, &st.wMonth, &st.wDay, &st.wHour, &st.wMinute, &st.wSecond);
	SystemTimeToFileTime(&st, &ft);
	LocalFileTimeToFileTime(&ft, &ft1);
	uli.LowPart = ft1.dwLowDateTime;
	uli.HighPart = ft1.dwHighDateTime;

	return (uli.QuadPart - uliBase.QuadPart) / 10000 ;
}

void TimeFromBase1970(double dDiff, char *szSysTime)
{
	SYSTEMTIME stBase;
	FILETIME ftBase;
	ULARGE_INTEGER uliBase;
	__int64 diff = (__int64)(dDiff);

	ZeroMemory(&stBase, sizeof(SYSTEMTIME));
	stBase.wYear = 1970;
	stBase.wMonth = 1;
	stBase.wDay = 1;

	SystemTimeToFileTime(&stBase, &ftBase);

	uliBase.LowPart = ftBase.dwLowDateTime;
	uliBase.HighPart = ftBase.dwHighDateTime;

	uliBase.QuadPart += diff * 10000;

	FILETIME ft;
	SYSTEMTIME st;
	ft.dwLowDateTime = uliBase.LowPart;
	ft.dwHighDateTime = uliBase.HighPart;


	FileTimeToLocalFileTime(&ft, &ft);
	FileTimeToSystemTime(&ft, &st);

	sprintf(szSysTime, "%d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
}

VOID SafeFreeMem(  VOID* pszMem )
{
	if (pszMem != NULL)
	{
		free(pszMem);
		pszMem = NULL;
	}
	return;
}


string BuildSignStringCommon( const char * szSecretKey, vector<string> &strVec, string &strFirst, char *szTimeStamp )
{
	std::sort(strVec.begin(), strVec.end());
	for (auto iter = strVec.begin(); iter != strVec.end(); ++iter)
	{
		strFirst += *iter;
		strFirst += "&";
	}

	strFirst.erase(strFirst.end()-1);

	string strSecond = strFirst + "&secretKey=";
	strSecond += szSecretKey;
	strSecond += "&timestamp=";

	const int nSize= 64;
	ZeroMemory(szTimeStamp, nSize);
	sprintf_s(szTimeStamp, nSize, "%I64u", CurrentTimeStampCommon());
	strSecond += szTimeStamp; 

	return strSecond;
}

ULONGLONG CurrentTimeStampCommon()
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
	ULARGE_INTEGER uli;
	memset(&st, 0, sizeof(SYSTEMTIME));
	GetLocalTime(&st);
	SystemTimeToFileTime(&st, &ft);
	uli.LowPart = ft.dwLowDateTime;
	uli.HighPart = ft.dwHighDateTime;

	return (uli.QuadPart - uliBase.QuadPart) / 10000;
}


BOOL  DirectoryList(LPCSTR Path, vector<string>& vectorStr)
{
	WIN32_FIND_DATAA FindData;
	HANDLE hError;
	int FileCount = 0;
	char FilePathName[MAX_PATH];
	// 构造路径
	char FullPathName[MAX_PATH];
	strcpy(FilePathName, Path);
	strcat(FilePathName, "\\*.*");
	hError = FindFirstFileA(FilePathName, &FindData);
	if (hError == INVALID_HANDLE_VALUE)
	{
		printf("搜索失败!");
		return 0;
	}
	while(::FindNextFileA(hError, &FindData))
	{
		// 过虑.和..
		if (strcmp(FindData.cFileName, ".") == 0 
			|| strcmp(FindData.cFileName, "..") == 0 )
		{
			continue;
		}

		// 构造完整路径
		char szPathBuf[MAX_PATH] = {0};	
		strcpy_s(szPathBuf, Path);
		int nLen = strlen(Path);
		if(nLen > 0)
		{
			if(Path[nLen-1] != ('\\'))
			{
				strcat_s(szPathBuf, ("\\"));
			}
		}
		
		wsprintfA(FullPathName, "%s%s", szPathBuf, FindData.cFileName);

		FileCount++;
		// 输出本级的文件
		//printf("\n%d  %s  ", FileCount, FullPathName);

		vectorStr.push_back(string(FullPathName));

		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//printf("<Dir>");
			DirectoryList(FullPathName, vectorStr);
		}
	}
	FindClose(hError);
	return 0;
}

BOOL StringConvertDateFormat(string& strInDate,OUT WORD&  dwYear, OUT WORD& dwMonth, OUT WORD & dwDay)
{
	string strDate =strInDate;
	if (strDate.length()<=0)
	{
		return FALSE;
	}
	int dwIndex = strDate.find_first_of('-');
	if (dwIndex == string::npos)
	{
		return FALSE;
	}

	int nNumIndex = strDate.find_first_of("0123456789");
	if (nNumIndex == string::npos)
	{
		return FALSE;
	}

	string strYear = strDate.substr(nNumIndex, dwIndex - nNumIndex);
	strDate = strDate.substr(dwIndex+1);
	dwIndex = strDate.find_first_of('-');
	if (dwIndex == string::npos)
	{
		return FALSE;
	}
	string strMonth = strDate.substr(0,dwIndex);
	strDate = strDate.substr(dwIndex+1);
	string strDay = strDate;
	dwYear = atoi(strYear.c_str());
	if (dwYear == 0)
	{
		return FALSE;
	}
	dwMonth = atoi(strMonth.c_str());
	if (0 == dwMonth)
	{
		return FALSE;
	}
	dwDay = atoi(strDay.c_str());
	if (0 == dwDay)
	{
		return FALSE;
	}
	return TRUE;
}

//得到当前桌面路径
BOOL GetDesktopPath(char *pszDesktopPath)
{
	USES_CONVERSION;

	LPITEMIDLIST  ppidl = NULL;

	if (SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &ppidl) == S_OK)
	{
		BOOL flag =   SHGetPathFromIDList(ppidl, A2T(pszDesktopPath));
		CoTaskMemFree(ppidl);
		return flag;
	}

	return FALSE;
}    
//得到快速启动栏的路径
BOOL GetIEQuickLaunchPath(char *pszIEQueickLaunchPath)
{    
	USES_CONVERSION;

	LPITEMIDLIST  ppidl;                   

	if (SHGetSpecialFolderLocation(NULL, CSIDL_APPDATA, &ppidl) == S_OK)
	{
		BOOL flag =   SHGetPathFromIDList(ppidl, A2T(pszIEQueickLaunchPath));
		strcat(pszIEQueickLaunchPath, "\\Microsoft\\Internet Explorer\\Quick Launch");
		CoTaskMemFree(ppidl);
		return flag;
	}

	return FALSE;
}

//得到所有用户桌面路径
BOOL GetAllUsersDesktopPath(char *pszDesktopPath)
{
	USES_CONVERSION;

	LPITEMIDLIST  ppidl = NULL;

	if (SHGetSpecialFolderLocation(NULL, CSIDL_COMMON_DESKTOPDIRECTORY, &ppidl) == S_OK)
	{
		BOOL flag =   SHGetPathFromIDList(ppidl, A2T(pszDesktopPath));
		CoTaskMemFree(ppidl);
		return flag;
	}

	return FALSE;
} 
/*
函数功能：对指定文件在指定的目录下创建其快捷方式
函数参数：
lpszFileName    指定文件，为NULL表示当前进程的EXE文件。
lpszLnkFileDir  指定目录，不能为NULL。
lpszLnkFileName 快捷方式名称，为NULL表示EXE文件名。
wHotkey         为0表示不设置快捷键
pszDescription  备注
iShowCmd        运行方式，默认为常规窗口
*/
BOOL CreateFileShortcut(
	LPSTR lpszFileName, 
	LPCSTR lpszLnkFileDir, 
	LPCSTR lpszLnkFileName, 
	LPSTR lpszWorkDir, 
	WORD wHotkey, 
	LPSTR lpszDescription, 
	int iShowCmd /*= SW_SHOWNORMAL*/)
{
	USES_CONVERSION;

	if (lpszLnkFileDir == NULL)
		return FALSE;

	HRESULT hr;
	IShellLink     *pLink;  //IShellLink对象指针
	IPersistFile   *ppf; //IPersisFil对象指针

	//创建IShellLink对象
	hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pLink);
	if (FAILED(hr))
		return FALSE;

	//从IShellLink对象中获取IPersistFile接口
	hr = pLink->QueryInterface(IID_IPersistFile, (void**)&ppf);
	if (FAILED(hr))
	{
		pLink->Release();
		return FALSE;
	}

	//目标
	if (lpszFileName == NULL)
		pLink->SetPath(A2T(_pgmptr));
	else
		pLink->SetPath(A2T(lpszFileName));

	//工作目录
	if (lpszWorkDir != NULL)
		pLink->SetWorkingDirectory(A2T(lpszWorkDir));

	//快捷键
	if (wHotkey != 0)
		pLink->SetHotkey(wHotkey);

	//备注
	if (lpszDescription != NULL)
		pLink->SetDescription(A2T(lpszDescription));

	//显示方式
	pLink->SetShowCmd(iShowCmd);


	//快捷方式的路径 + 名称
	char szBuffer[MAX_PATH];
	if (lpszLnkFileName != NULL) //指定了快捷方式的名称
		sprintf(szBuffer, "%s\\%s", lpszLnkFileDir, lpszLnkFileName);
	else  
	{
		//没有指定名称，就从取指定文件的文件名作为快捷方式名称。
		const char *pstr;
		if (lpszFileName != NULL)
			pstr = strrchr(lpszFileName, '\\');
		else
			pstr = strrchr(_pgmptr, '\\');

		if (pstr == NULL)
		{    
			ppf->Release();
			pLink->Release();
			return FALSE;
		}
		//注意后缀名要从.exe改为.lnk
		sprintf(szBuffer, "%s\\%s", lpszLnkFileDir, pstr);
		int nLen = strlen(szBuffer);
		szBuffer[nLen - 3] = 'l';
		szBuffer[nLen - 2] = 'n';
		szBuffer[nLen - 1] = 'k';
	}
	//保存快捷方式到指定目录下
	WCHAR  wsz[MAX_PATH];  //定义Unicode字符串
	MultiByteToWideChar(CP_ACP, 0, szBuffer, -1, wsz, MAX_PATH);

	hr = ppf->Save(wsz, TRUE);

	ppf->Release();
	pLink->Release();
	return SUCCEEDED(hr);
}

// string GetDLBVersion()
// {
// 	USES_CONVERSION;

// 	//获取配置文件路径
// 	char szModePath[MAX_PATH] = {0};
// 	char szConfigPath[MAX_PATH] = { 0 };
// 	if(GetProductDirA(szModePath, MAX_PATH))
// 	{
// 		memcpy(szConfigPath,szModePath, MAX_PATH);
// 		strcat(szConfigPath, ("\\"));
// 	}else
// 	{
// 		return "";
// 	}
// 	strcat(szConfigPath, T2A(D_CONFIG_INI));

// 	TCHAR szVersion[MAX_PATH] = { 0 };
// 	GetPrivateProfileString(D_CONFIG_HOSTINFO, D_HOSTINFO_VERSION,_T("3.1.1.5"),szVersion, MAX_PATH, A2T(szConfigPath));
// 	return string(T2A(szVersion));

// }


bool GetStringValW(HKEY hKey, const std::wstring &subkey, const std::wstring &name, std::wstring &value)
{
	HKEY hSoftKey = NULL;
	value = L"";
	DWORD style;
	DWORD len = 1024;
	wchar_t buf[512];

	/// see remark section of RegQueryValueEx(MSDN), buffer should be zero-ed 
	memset(buf, 0, 1024); 

	long rc = RegOpenKeyExW(hKey, subkey.c_str(), 0, KEY_READ, &hSoftKey);
	if (rc != ERROR_SUCCESS)
	{
		return false; 
	}

	rc = RegQueryValueExW(hSoftKey, name.c_str(), NULL, &style, (LPBYTE)buf, &len);
	if (rc != ERROR_SUCCESS)
	{
		RegCloseKey(hSoftKey);
		return false;
	}

	value = buf;
	RegCloseKey(hSoftKey);
	return true;
}

bool GetStringValA(HKEY hKey, const std::string &subkey, const std::string &name, std::string &value)
{
	HKEY hSoftKey = NULL;
	DWORD style;
	DWORD len = 1024;
	char buf[1024];

	/// see remark section of RegQueryValueEx(MSDN), buffer should be zero-ed 
	memset(buf, 0, 1024);

	long rc = RegOpenKeyExA(hKey, subkey.c_str(), 0, KEY_READ, &hSoftKey);
	if (rc != ERROR_SUCCESS)
	{
		return false; 
	}

	rc = RegQueryValueExA(hSoftKey, name.c_str(), NULL, &style, (LPBYTE)buf, &len);
	if (rc != ERROR_SUCCESS)
	{
		RegCloseKey(hSoftKey);
		return false;
	}

	value = buf;
	RegCloseKey(hSoftKey);
	return true;
}

BOOL X64OS()
{
	BOOL bRet = FALSE;
	SYSTEM_INFO si;
	typedef VOID(__stdcall*GETNATIVESYSTEMINFO)(LPSYSTEM_INFO lpSystemInfo);
	GETNATIVESYSTEMINFO fnGetNativeSystemInfo = NULL;
	fnGetNativeSystemInfo=(GETNATIVESYSTEMINFO)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),"GetNativeSystemInfo");
	if (fnGetNativeSystemInfo != NULL)
	{
		fnGetNativeSystemInfo(&si);

		if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||   
			si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )
		{ 
			bRet=TRUE;
		}
	}
	return bRet; 
}

#define	TRUST_BUFFER_SIZE			48

BOOL	EmfIsNumber(CHAR Ch)
{
	BOOL	bRtn = FALSE;

	if( Ch >= '0' && Ch <= '9' )
	{
		bRtn = TRUE;
	}

	return bRtn;
}

BOOL EmfIsLetter(CHAR Ch)
{
	BOOL	bRtn = FALSE;

	if( Ch >= 'a' && Ch <= 'z')
	{
		bRtn = TRUE;
	}
	else if( Ch >= 'A' && Ch <= 'Z')
	{
		bRtn = TRUE;
	}

	return bRtn;
}

BOOL	EmfIsNumberOrLetter(CHAR Ch)
{
	BOOL	bRtn = FALSE;

	bRtn = EmfIsNumber(Ch);
	if( !bRtn )
	{
		bRtn = EmfIsLetter(Ch);
	}

	return bRtn;
}


void 	MzHexStrToBytes(char* pHexString,char* pOutput)
{
	int		uLength = strlen((char *)pHexString);
	UINT	uIndex = 0;
	char	temp = 0;
	int 	i=0;

	if( !pHexString || !pOutput || 0 != uLength%2 )
	{
		return ;
	}

	for(i=0;i<uLength;i++)
	{
		if( pHexString[i] >= '0' && pHexString[i] <= '9')
		{
			temp = pHexString[i] - '0';
		}
		else if( pHexString[i] >= 'a' && pHexString[i] <= 'f')
		{
			temp = pHexString[i] - 'a' + 10;
		}
		else if( pHexString[i] >= 'A' && pHexString[i] <= 'F')
		{
			temp = pHexString[i] - 'A' + 10;
		}
		else
		{
			break;
		}
		// 一个字节代表的是2位字符，分高位和地位，高位要乘以16
		if( 0 == i%2 )
		{
			pOutput[uIndex] = temp * 16;
		}
		else
		{
			pOutput[uIndex++] += temp;
		}
	}
	if( strlen(pOutput) != uLength/2)
	{
		memset(pOutput,0,strlen((char *)pOutput));
	}
}



VOID	MzGetDeskNoByContent(char* pData,size_t uDataSize,char* szDeskNo)
{
	BOOL	bStartMoney = FALSE;
	PCHAR	pTemp = NULL;
	size_t	uIndex = 0;
	size_t	i = 0;

	if( !pData || !szDeskNo || !uDataSize )
	{
		return ;
	}
	// 初始化临时指针
	pTemp = pData;

	for(i=0;i<uDataSize;i++)
	{
		if( !bStartMoney && EmfIsNumberOrLetter(pTemp[i]) )
		{
			// 如果是第一次找到数字，则表示金额截取开始
			bStartMoney = TRUE;
			szDeskNo[uIndex++] = pTemp[i];		// 将金额数字拷贝到缓存
		}
		else if( bStartMoney && EmfIsNumberOrLetter(pTemp[i]) )
		{
			szDeskNo[uIndex++] = pTemp[i];		// 将金额数字拷贝到缓存
		}
		else if( bStartMoney && !EmfIsNumberOrLetter(pTemp[i]) )
		{
			break;
		}
	}
}

VOID	MzGetMoneyValueByContent(char* pData,size_t uDataSize,char* pMoneyValue)
{
	BOOL	bStartMoney = FALSE;
	char*	pTemp = NULL;
	size_t	uIndex = 0;
	size_t	i = 0;

	if( !pData || !uDataSize || !pMoneyValue )
	{
		return ;
	}
	// 初始化临时指针
	pTemp = pData;
	memset(pMoneyValue,0,TRUST_BUFFER_SIZE);

	for(i=0;i<uDataSize;i++)
	{
		if( !bStartMoney && EmfIsNumber(pTemp[i]) )
		{
			// 如果是第一次找到数字，则表示金额截取开始
			bStartMoney = TRUE;
			pMoneyValue[uIndex++] = pTemp[i];		// 将金额数字拷贝到缓存
		}
		else if( bStartMoney && EmfIsNumber(pTemp[i]) )
		{
			pMoneyValue[uIndex++] = pTemp[i];		// 将金额数字拷贝到缓存
		}
		else if( bStartMoney && pTemp[i] == '.' )
		{
			pMoneyValue[uIndex++] = pTemp[i];		// 将金额小数点拷贝到缓存
		}// 1000以上的金额会出现逗号，例如 2,129
		else if( bStartMoney && ( pTemp[i] == ' ' ||  pTemp[i] == ',' ||  pTemp[i] == '，') )	// 朋友圈的文件金额数字中间会出现空格，中英文逗号都要处理。
		{
			continue;
		}
		else if( bStartMoney && (pTemp[i] != '.' && !EmfIsNumber(pTemp[i])) )
		{
			break;
		}
	}
}

BOOL	RawFindQrEmbeKey(char* pData,size_t uDataSize,char* szQrKey)
{
	size_t	uIndex = 0;
	BOOL	bFind = FALSE;

	if( !pData || !uDataSize || !szQrKey )
	{
		return FALSE;
	}

	for(uIndex=0;uIndex<uDataSize;uIndex++)
	{
		if( 0 == memcmp(pData+uIndex,szQrKey,strlen(szQrKey) ) )
		{
			bFind = TRUE;
			break;
		}
	}

	return bFind;
}


VOID	RawGetMoneyValue(char* pData,size_t uDataSize,char* szMoneyKey,char* szMoneyValue)
{
	size_t	uIndex = 0;

	if( !pData || !uDataSize || !szMoneyKey || !szMoneyValue )
	{
		return ;
	}

	for(uIndex=0;uIndex<uDataSize;uIndex++)
	{
		if( 0 == memcmp(pData+uIndex,szMoneyKey,strlen(szMoneyKey) ) )
		{
			MzGetMoneyValueByContent(pData+uIndex,uDataSize-uIndex,szMoneyValue);
			break;
		}
	}
}


VOID RawGetDeskNo(char* pData,size_t uDataSize,char* szDeskNoKey, char *szDeskNoTailKey,char* szDeskNo)
{
	size_t	uIndex = 0;

	if( !pData || !uDataSize || !szDeskNoKey || !szDeskNo )
	{
		return ;
	}

	int nKeyLen = strlen(szDeskNoKey);
	for(uIndex=0;uIndex<uDataSize;uIndex++)
	{
		if( 0 == memcmp(pData+uIndex, szDeskNoKey, nKeyLen) )
		{
			//桌号可能出现汉字，大董桌号出现（外卖1 201文津）这样的情况	
			char *p = pData + uIndex + nKeyLen;
			int nLen = strlen(szDeskNoTailKey);
			int i = 0;
			int nLeft = uDataSize-uIndex-nKeyLen-1;
			while(i < TRUST_BUFFER_SIZE && i < nLeft && 0 != memcmp(p+i, szDeskNoTailKey, nLen))
			{
				szDeskNo[i] = p[i];
				++i;
			}			

			// 			MzGetDeskNoByContent(pData+uIndex+nKeyLen+i, uDataSize-uIndex-nKeyLen-i, szDeskNo+i);
			break;
		}
	}
}


BOOL	MzFindCutCommand(char* pData,size_t uDataSize,char* pCutCommand,size_t* nPosition)
{
	size_t	uIndex = 0;
	BOOL	bFind = FALSE;
	unsigned char	*pTemp = NULL;
	char	szCutCmd1[8] = "1D53";
	char	szCutCmd2[8] = "1D56";

	if( !pData || !uDataSize  )
	{
		return FALSE;
	}
	// 先将当前数据块转换为字符串
	pTemp = (unsigned char*)MzHeapAlloc(uDataSize*2+128);
	if( !pTemp )
	{
		return FALSE;
	}
	// 转换
	MzStringToHex((unsigned char*)pData,uDataSize,pTemp,uDataSize*2+128);
	// 从转换后的字符串指令中查找切纸指令
	if( strlen(pCutCommand) )
	{
		for(uIndex=0;uIndex<uDataSize*2;uIndex++)
		{
			if( 0 == memcmp(pTemp+uIndex,pCutCommand,strlen(pCutCommand)) )
			{
				bFind = TRUE;
				// 返回当前文件中的偏移
				*nPosition = uIndex/2;
				break;
			}
		}
	}
	else
	{
		// 查找默认指令1
		for(uIndex=0;uIndex<uDataSize*2;uIndex++)
		{
			if( 0 == memcmp(pTemp+uIndex,szCutCmd1,strlen(szCutCmd1)) )
			{
				bFind = TRUE;
				// 返回当前文件中的偏移
				*nPosition = uIndex/2;
				break;
			}
		}
		// 如果默认指令1也没找到，则找默认指令2
		if( !bFind )
		{
			for(uIndex=0;uIndex<uDataSize*2;uIndex++)
			{
				if( 0 == memcmp(pTemp+uIndex,szCutCmd2,strlen(szCutCmd2)) )
				{
					bFind = TRUE;
					// 返回当前文件中的偏移
					*nPosition = uIndex/2;
					break;
				}
			}
		}
	}
	if( bFind )
	{
		MzOutputDebugStringA("[APL]:在长度%d数据中找到切纸指令%s\n",uDataSize,pCutCommand);
	}
	else
	{
		MzOutputDebugStringA("[APL]:在长度%d数据中查找不到切纸指令%s\n",uDataSize,pCutCommand);
	}
	MzHeapFree(pTemp);
	return bFind;
}


/*size_t	MzFindCutCommand(char* pData,size_t uDataSize,char* pCutCommand)
{
	size_t	uIndex = 0;

	if( !pData || !uDataSize || !pCutCommand )
	{
		return 0;
	}
	// 从尾部开始查找切纸指令
	for(uIndex=uDataSize-strlen(pCutCommand);uIndex>0;uIndex--)
	{
		if( 0 == memcmp(pData+uIndex,pCutCommand,strlen(pCutCommand)) )
		{
			break;
		}
	}
	return uIndex;
}*/



void	MzGetTextPos(char* pText,char* szPos)
{
	size_t	uIndex = 0;

	if( !pText || !szPos )
	{
		return ;
	}
		// 设置汉子模式
	//szPos[uIndex++] = 0x1C;
	//szPos[uIndex++] = 0x26;

	StringCbCatA(szPos,TRUST_BUFFER_SIZE,pText);
	uIndex = strlen(szPos);
	// 取消汉子模式
	// 必须添加，就打印乱码了
	szPos[uIndex++] = 0x0A;
	
	//szPos[uIndex++] = 0x1C;
	//szPos[uIndex++] = 0x2E;
}



void	MzAddPosData(char* pRawFile,char* pCutCommand,char* pData,size_t uDataSize)
{
	HANDLE	hFile		= INVALID_HANDLE_VALUE;
	DWORD	dwRtnSize	= 0;
	char	*pTemp = NULL;
	char	szTailData[MAX_PATH] = {0};
	size_t	uIndex = 0;
	size_t	uFileSize = 0;
	size_t	uPostion = 0;
	BOOL	bRtn = FALSE;

	if( !pRawFile || !pData || !uDataSize )
	{
		return ;
	}

	hFile = CreateFileA(pRawFile,GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ, NULL,OPEN_ALWAYS, 0, NULL);
	if( INVALID_HANDLE_VALUE == hFile )
	{
		MzOutputDebugStringA("[PortHook]:EmfAddEsc 打开文件[%ws]失败 Error Code:%d\n",pRawFile,GetLastError());
		return ;
	}

	do
	{
		uFileSize = GetFileSize(hFile,NULL);
		if( !uFileSize )
		{
			break ;
		}
		pTemp = (char*)HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, uFileSize+1 );
		if( !pTemp )
		{
			break;
		}
		bRtn = ReadFile(hFile,pTemp,uFileSize,&dwRtnSize,NULL);
		if( !bRtn )
		{
			break;
		}

		bRtn = MzFindCutCommand(pTemp,dwRtnSize,pCutCommand,&uPostion);
		if( bRtn )
		{
			// 移动到切纸指令之前，写入需要添加的POS数据
			SetFilePointer(hFile,uPostion,NULL,FILE_BEGIN);
			// 读取切纸指令及其后面的数据
			ReadFile(hFile,szTailData,uFileSize-uPostion,&dwRtnSize,NULL);
			// 再次移动到切纸指令前，加入数据
			SetFilePointer(hFile,uPostion,NULL,FILE_BEGIN);
			// 插入数据
			WriteFile(hFile,pData,uDataSize,&dwRtnSize,NULL);
			// 将覆盖的数据写回去
			WriteFile(hFile,szTailData,uFileSize-uPostion,&dwRtnSize,NULL);
		}
	}while(0);
	// 释放资源
	if( INVALID_HANDLE_VALUE != hFile  )
	{
		CloseHandle(hFile);
	}
	if( pTemp )
	{
		MzHeapFree(pTemp);
	}
}




long MyRand(int n)  
{  
	//static int inited=0;  
	long X,k=1;  
	int i;  
	unsigned int nSeed = CurrentTimeStampCommon()%100000;
	//	long n=CurrentTimeStampCommon()%100000;
	srand( nSeed ); 
	//srand((unsigned)time(0));  
	// 	if(!inited)  
	// 	{  
	// 		srand((unsigned)time(0));  
	// 		inited=1;  
	// 	}  
	if(n==1)  
		return rand()%10;  
	else if(n==2)  
		return (rand()%9+1)*10+rand()%10;  
	for(i=0;i<n/2+1;i++)  
		k*=10;  
	for(i=n/2+1;i<n-1;i++)  
		k*=10;  
	X=(rand()%9+1)*k+((long)(rand()%k)*(rand()%k))%k;  
	if(X%2==0)//如果产生的随机数是偶数，那么+1使之成为奇数。偶数肯定不是素数。  
		X=X+1;  
	return X;  
}  

string&   replace_all(string&   str,const   string&   old_value,const   string&   new_value)     
{     
	while(true)   {     
		string::size_type   pos(0);     
		if(   (pos=str.find(old_value))!=string::npos   )     
			str.replace(pos,old_value.length(),new_value);     
		else   break;     
	}     
	return   str;     
}     


BOOL StringToWString(const std::string &str,std::wstring &wstr)
{    
	int nLen = (int)str.length();    
	wstr.resize(nLen,L' ');

	int nResult = MultiByteToWideChar(CP_ACP,0,(LPCSTR)str.c_str(),nLen,(LPWSTR)wstr.c_str(),nLen);

	if (nResult == 0)
	{
		return FALSE;
	}

	return TRUE;
}

void Wchar_tToString(std::string& szDst, wchar_t *wchar)
{
	wchar_t * wText = wchar;
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP,NULL,wText,-1,NULL,0,NULL,FALSE);// WideCharToMultiByte的运用
	char *psText;  // psText为char*的临时数组，作为赋值给std::string的中间变量
	psText = new char[dwNum];
	WideCharToMultiByte (CP_OEMCP,NULL,wText,-1,psText,dwNum,NULL,FALSE);// WideCharToMultiByte的再次运用
	szDst = psText;// std::string赋值
	delete []psText;// psText的清除
}


//把WORD数组调整字节序为little-endian，并滤除字符串结尾的空格。
void ToLittleEndian(PUSHORT pWords, int nFirstIndex, int nLastIndex, LPSTR pBuf)
{
	int index;
	LPSTR pDest = pBuf;
	for(index = nFirstIndex; index <= nLastIndex; ++index)
	{
		pDest[0] = pWords[index] >> 8;
		pDest[1] = pWords[index] & 0xFF;
		pDest += 2;
	}    
	*pDest = 0;

	//trim space at the endof string; 0x20: _T(' ')
	--pDest;
	while(*pDest == 0x20)
	{
		*pDest = 0;
		--pDest;
	}
}

//获取硬盘ID
BOOL GetPhyDriveSerial(LPSTR pModelNo, LPSTR pSerialNo)
{
	//-1是因为 SENDCMDOUTPARAMS 的结尾是 BYTE bBuffer[1];
	BYTE IdentifyResult[sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1] = {0};
	DWORD dwBytesReturned = 0;
	GETVERSIONINPARAMS get_version;
	SENDCMDINPARAMS send_cmd = { 0 };

	HANDLE hFile = CreateFile(_T("\\\\.\\PHYSICALDRIVE0"), GENERIC_READ | GENERIC_WRITE,    
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	//get version
	DeviceIoControl(hFile, SMART_GET_VERSION, NULL, 0,
		&get_version, sizeof(get_version), &dwBytesReturned, NULL);

	//identify device
	send_cmd.irDriveRegs.bCommandReg = (get_version.bIDEDeviceMap & 0x10)? ATAPI_ID_CMD : ID_CMD;
	DeviceIoControl(hFile, SMART_RCV_DRIVE_DATA, &send_cmd, sizeof(SENDCMDINPARAMS) - 1,
		IdentifyResult, sizeof(IdentifyResult), &dwBytesReturned, NULL);
	CloseHandle(hFile);

	//adjust the byte order
	PUSHORT pWords = (USHORT*)(((SENDCMDOUTPARAMS*)IdentifyResult)->bBuffer);
	ToLittleEndian(pWords, 27, 46, pModelNo);
	ToLittleEndian(pWords, 10, 19, pSerialNo);
	return TRUE;
}


//滤除字符串起始位置的空格
void TrimStart(LPSTR pBuf)
{
	if(*pBuf != 0x20)
		return;

	LPSTR pDest = pBuf;
	LPSTR pSrc = pBuf + 1;
	while(*pSrc == 0x20)
		++pSrc;

	while(*pSrc)
	{
		*pDest = *pSrc;
		++pDest;
		++pSrc;
	}
	*pDest = 0;
}


//内部根据硬盘ID和cpuID返回这台计算机的唯一标识
BOOL GetPcPostId(char* szId, size_t ibuflen)
{
	USES_CONVERSION;

	BOOL bRun = FALSE;
	char szModelNo[48] = {0}, szSerialNo[24] ={0}, szHDTemp[17]={0};
	INT32 adwBuf[4]={0};
	char szCpuIdNo[24] = {0};
	char szConfigFile[MAX_PATH]={0};
	int iUniqueID=0;
	char* pTemp = NULL;
	do 
	{
		if (szId == NULL || ibuflen< PC_CODE_SIZE)
		{
			break;
		}
		//获得硬盘ID
		if(!GetPhyDriveSerial(szModelNo, szSerialNo))
		{
			break;
		}
		TrimStart(szSerialNo);
		//获取CPUID
		__cpuidex(adwBuf, 1,0);
		sprintf(szCpuIdNo, "%08X%08X", adwBuf[3], adwBuf[0]);

		//获得硬盘ID的前16位
		memcpy(szHDTemp,  szSerialNo, 16);

		//返回ID
		sprintf(szId, "%s%016s", szCpuIdNo, szHDTemp);

		//为了防止计算出来的唯一ID还可能一样，当然这样的几率很小，读取配置文件里面的数字标识做一个加法，得到最后的唯一ID，这个数字标识默认是0，不影响，只有真正出现重复的时候才设置
		if (GetProductDirA(szConfigFile, MAX_PATH))
		{
			StringCbCatA(szConfigFile,MAX_PATH,"\\D_Config.ini");
		}
		else
		{
			StringCbCatA(szConfigFile,MAX_PATH,"D_Config.ini");
		}
		iUniqueID= (int)GetPrivateProfileInt(_T("LOGINUSERINFO"), _T("UniqueID"),0, A2T(szConfigFile));
		pTemp = szId+ strlen(szId)-1;
		*pTemp = *pTemp+iUniqueID;

		bRun = TRUE;
	} while (FALSE);
	return bRun;
}


//将"012345678"形式的数字字符串转换成“零一二三四五六七八”形式的中文字符串
string NumberStringToChinese(char* strNumber)
{
	string strRun = "";
	int iLen  = 0;
	char ctemp;
	if (strNumber == NULL)
	{
		return strRun;
	}
	iLen = strlen(strNumber);
	for(int i = 0; i<iLen; i++)
	{
		ctemp = *(strNumber+i);
		switch(ctemp)
		{
		case '0':
			{
				strRun += "零";
			}
			break;
		case '1':
			{
				strRun += "一";
			}
			break;
		case '2':
			{
				strRun += "二";
			}
			break;
		case '3':
			{
				strRun += "三";
			}
			break;
		case '4':
			{
				strRun += "四";
			}
			break;
		case '5':
			{
				strRun += "五";
			}
			break;
		case '6':
			{
				strRun += "六";
			}
			break;
		case '7':
			{
				strRun += "七";
			}
			break;
		case '8':
			{
				strRun += "八";
			}
			break;
		case '9':
			{
				strRun += "九";
			}
			break;
		default:
			strRun += ctemp;
			break;
		}

	}
	return strRun;
}



//将2017形式的数字字符串转换成“两千零一十七”形式的中文数字字符串
string NumberValueToChinese(char* strNumber)
{
	string strRun = "";
	int iLen  = 0;
	char ctemp;
	int iIndex = 0;

	char szScale[][10] = {
		"",
		"十",
		"百",
		"千",
		"万",
		"十万",
		"百万",
		"千万",
		"亿",
		"十亿",
		"百亿",
		"千亿",
		"兆",
	};
	BOOL bBegin = FALSE;   //是否出现有效数字
	BOOL bZeroArise = FALSE; //前面紧邻的是否是0

	if (strNumber == NULL)
	{
		return strRun;
	}
	//超过13位，都超过兆了，非法了
	iLen = strlen(strNumber);
	if (iLen > 13)
	{
		return strRun;
	}
	for (int i = 0; i< iLen; i++)
	{
		iIndex = iLen -i-1;
		ctemp = *(strNumber+i);
		switch(ctemp)
		{
		case '0':
			{
				//证明已经开始了
				if (bBegin)
				{
					if (!bZeroArise) //前面没有出现0
					{
						strRun += "零";
						bZeroArise = TRUE;
					}
				}
			}
			break;
		case '1':
			{
				if ((strRun.length() >0) || (iIndex != 1))   //结果字符串长度不为0 代表前面已经有有效数字了 或者不是 十位
				{
					strRun += "一";
					strRun += szScale[iIndex];
				}
				else  //是十位，代表这个数是十位数，并且十位是1
				{
					strRun += szScale[iIndex]; //不读这个一
				}
				bBegin = TRUE;
				bZeroArise = FALSE;
			}
			break;
		case '2':
			{
				if (iIndex> 1)
				{
					strRun += "两";
				}
				else
				{
					strRun += "二";
				}
				strRun += szScale[iIndex];
				bBegin = TRUE;
				bZeroArise = FALSE;
			}
			break;
		case '3':
			{
				strRun += "三";
				strRun += szScale[iIndex];
				bBegin = TRUE;
				bZeroArise = FALSE;
			}
			break;
		case '4':
			{
				strRun += "四";
				strRun += szScale[iIndex];
				bBegin = TRUE;
				bZeroArise = FALSE;
			}
			break;
		case '5':
			{
				strRun += "五";
				strRun += szScale[iIndex];
				bBegin = TRUE;
				bZeroArise = FALSE;
			}
			break;
		case '6':
			{
				strRun += "六";
				strRun += szScale[iIndex];
				bBegin = TRUE;
			}
			break;
		case '7':
			{
				strRun += "七";
				strRun += szScale[iIndex];
				bBegin = TRUE;
			}
			break;
		case '8':
			{
				strRun += "八";
				strRun += szScale[iIndex];
				bBegin = TRUE;
				bZeroArise = FALSE;
			}
			break;
		case '9':
			{
				strRun += "九";
				strRun += szScale[iIndex];
				bBegin = TRUE;
				bZeroArise = FALSE;
			}
			break;
		default:
			return "";
			break;
		}
	}
	//如果数字的结尾是零，这个零不读，所以直接过滤了它
	int iPos = strRun.find("零", strRun.length()-2);
	if (iPos >= 0)
	{
		strRun = strRun.substr(0, strRun.length()-2);
	}
	//走到最后，输入字符串的长度大于0 并且返回字符穿为空，肯定是一串0，变成一个“零”
	if (strRun.length() <= 0 &&
		iLen > 0)
	{
		strRun += "零"; 
	}
	return strRun;
}


//将2017.00形式的带小数点的数字字符串转换成“”形式的中文字符串
string NumberFloatValueToChinese(char* strNumber)
{
	char *pos = NULL;
	string strRun = "", strHead = "", strEnd = "";
	if (strNumber == NULL)
	{
		return strRun;
	}
	//首先查找是不是有小数点
	pos = strstr(strNumber,".");
	if (pos != NULL)
	{
		//证明有小数点，再检查有几个
		char* posTemp = strstr(pos+1,".");
		if (posTemp != NULL)
		{
			return strRun;  //两个小数点证明无效
		}
	}
	//分割为三部分（小数点前 小数点 小数点后）采用不同的方式进行解析
	if (pos != NULL)
	{

		strHead = string(strNumber, pos-strNumber);
		strEnd = pos+1;
	}
	else 
	{
		strHead = strNumber;
	}

	strHead = NumberValueToChinese((char*)strHead.c_str()); //小数点之前的按照带“十百千万”的数值类型格式化
	strEnd = NumberStringToChinese((char*)strEnd.c_str());  //小数点之前的按照数字本身单个数值格式化

	strRun  += strHead;
	if (strEnd.length() > 0)
	{
		strRun  += "点";
		strRun  += strEnd;
	}
	return strRun;
}


//将形如“YYYY-MM-DD HH:MM:SS”(例如：“2017-08-02 15:43:58”) 形式的时间转换成 "年月日时分秒的中文表述"（例如：“二零一七年八月二日一十五时四十三分五十八秒”）
string NumberTimeToChineseTime(char* strNumber)
{
	string strRun = "";
	string strtemp = "", strtempTwo = "";
	int iTemp = 0;

	do 
	{
		if (strNumber == NULL)
		{
			break;
		}
		//首先确认一下字符串长度，不符合要求不解析
		if (strlen(strNumber) != strlen("YYYY-MM-DD HH:MM:SS") )
		{
			break;
		}
		//解析年
		strtemp = strNumber;
		strtemp = strtemp.substr(0,4);
		strtempTwo =  NumberStringToChinese((char*)strtemp.c_str());
		if (strtempTwo.length() != 0)
		{
			strRun += strtempTwo;
			strRun += "年";
		}

		//解析月
		strtemp = strNumber;
		strtemp = strtemp.substr(5,2);
		strtempTwo =  NumberValueToChinese((char*)strtemp.c_str());
		if (strtempTwo.length() != 0)
		{
			strRun += strtempTwo;
			strRun += "月";
		}

		//解析日
		strtemp = strNumber;
		strtemp = strtemp.substr(8,2);
		strtempTwo =  NumberValueToChinese((char*)strtemp.c_str());
		if (strtempTwo.length() != 0)
		{
			strRun += strtempTwo;
			strRun += "日";
		}

		//解析时
		strtemp = strNumber;
		strtemp = strtemp.substr(11,2);
		strtempTwo =  NumberValueToChinese((char*)strtemp.c_str());
		if (strtempTwo.length() != 0)
		{
			strRun += strtempTwo;
			strRun += "时";
		}

		//解析分
		strtemp = strNumber;
		strtemp = strtemp.substr(14,2);
		strtempTwo =  NumberValueToChinese((char*)strtemp.c_str());
		if (strtempTwo.length() != 0)
		{
			strRun += strtempTwo;
			strRun += "分";
		}

		//解析秒
		strtemp = strNumber;
		strtemp = strtemp.substr(17,2);
		strtempTwo =  NumberValueToChinese((char*)strtemp.c_str());
		if (strtempTwo.length() != 0)
		{
			strRun += strtempTwo;
			strRun += "秒";
		}
	} while (FALSE);
	return strRun;
}


//将在字符串strReplace 中查找所有的strSrc子串并且替换成strDes
string ReplaceSubStr(char* strReplace, char* strSrc, char* strDes)
{
	string strRun = "";
	int iIndex = 0;
	do 
	{
		if (strReplace == NULL ||
			strSrc == NULL ||
			strDes == NULL)
		{
			break;
		}
		strRun = strReplace;

		while(1)
		{
			iIndex = strRun.find(strSrc);
			if (iIndex >= 0)
			{
				strRun.replace(iIndex, strlen(strSrc), strDes);
			}
			else
			{
				break;
			}
		}

	} while (FALSE);
	return strRun;
}


BOOL AllowMeesageForVista(UINT uMessageID, BOOL bAllow)
{
	BOOL bResult = FALSE;
	HMODULE hUserMod = NULL;
	do 
	{
		hUserMod = LoadLibrary(_T("user32.dll"));
		if( NULL == hUserMod )
		{
			break;
		}
		_ChangeWindowMessageFilter pChangeWindowMessageFilter = (_ChangeWindowMessageFilter)GetProcAddress( hUserMod, "ChangeWindowMessageFilter" );
		if( NULL == pChangeWindowMessageFilter )
		{
			break;
		}
		bResult = pChangeWindowMessageFilter( uMessageID, bAllow ? 1 : 2 );//MSGFLT_ADD: 1, MSGFLT_REMOVE: 2
	} while (false);
	if( NULL != hUserMod )
	{
		FreeLibrary( hUserMod );
	}
	return bResult;

}

HWND GetWndFromPoint(POINT pt)
{
	HWND hwndFocus = NULL,hTmp = NULL;
	RECT tempRc={0};
	BOOL bFind=FALSE;

	hwndFocus= WindowFromPoint(pt);
	if (hwndFocus == NULL)
	{
		return hwndFocus;
	}
	hTmp= ::GetWindow(hwndFocus, GW_CHILD);
	while (hTmp)
	{
		::GetWindowRect(hTmp, &tempRc);
		if(::PtInRect(&tempRc,pt))
		{
			bFind = TRUE;
			break;
		}
		else
		{
			hTmp = ::GetWindow(hTmp, GW_HWNDNEXT);
		}
	}//while
	if (bFind)
	{
		hwndFocus = hTmp;
	}
	return hwndFocus;
}


//遍历子窗口参数
typedef struct _FINDWND_PAR_
{
    HWND  hwndGlobal;       //最后传出的保存的窗口句柄
	BOOL  g_fShowHidden;    //是否包含查找隐藏窗口 
	POINT pt;               //鼠标所在点的坐标
	DWORD dwArea;           //窗口的区域大小（长*宽）
}FINDWND_PAR, *PFINDWND_PAR;



static BOOL CALLBACK FindBestChildProc(HWND hwnd, LPARAM lParam)
{
	RECT  rect;
	DWORD a;
	PFINDWND_PAR pPar = (PFINDWND_PAR)lParam;

	GetWindowRect(hwnd, &rect);

	//点在窗口范围内
	if(PtInRect(&rect, pPar->pt))
	{
		//计算子窗口区域
		a = (rect.right-rect.left) * (rect.bottom-rect.top);

		//如果此子窗口小于当前的“最佳”窗口，则选择此窗口 (窗口可见，或者显示隐藏窗口)
		if(a < pPar->dwArea && (IsWindowVisible(hwnd) || pPar->g_fShowHidden == TRUE))
		{
			pPar->dwArea = a;         //更新区域大小
			pPar->hwndGlobal = hwnd;  //跟新句柄
		}
	}
	return TRUE;
}

HWND FindBestChild(HWND hwndFound, POINT pt, BOOL bfShowHidden)
{
	HWND  hwnd = NULL;
	DWORD dwStyle = 0;
	PFINDWND_PAR Par = NULL;

	hwnd = GetParent(hwndFound);
	dwStyle = GetWindowLong(hwndFound, GWL_STYLE);

	//原始窗口可能已经是顶层窗口，所以就不从父窗口开始了。
	if(hwnd == 0 || (dwStyle & WS_POPUP))
		hwnd = hwndFound;

	Par = new FINDWND_PAR;
	if (Par == NULL)
	{
		return NULL;
	}
	ZeroMemory(Par, sizeof(FINDWND_PAR));
	Par->pt = pt;
	Par->g_fShowHidden = bfShowHidden;
	Par->dwArea = -1;


	EnumChildWindows(hwnd, FindBestChildProc, (LPARAM)Par);

	if(Par->hwndGlobal != NULL)
	{
		hwnd = Par->hwndGlobal;
	}

	if (Par != NULL)
	{
		delete Par;
		Par = NULL;
	}
	return hwnd;
}



HWND GetWndFromPointSicond(POINT pt, BOOL fShowHidden)
{
	HWND hwndFocus = NULL;
	BOOL bFind=FALSE;
	RECT tempRc={0};

	hwndFocus= WindowFromPoint(pt);
	if (hwndFocus == NULL)
	{
		return hwndFocus;
	}
	hwndFocus = FindBestChild(hwndFocus, pt, fShowHidden);
	//如果不显示隐藏窗口，找到那个第一个可以显示的窗口
	if(!fShowHidden)
	{
		while(hwndFocus && !IsWindowVisible(hwndFocus))
		{
			hwndFocus = GetParent(hwndFocus);
		}
	}
	return hwndFocus;
}



//画窗口的边框
void InvertDrawFrame(HWND hWndPoint) 
{
	RECT rectFrame = {0};
	HDC hdc = NULL;
	HPEN hPen = NULL;
	int  iPenWidth = 3;

	if(hWndPoint<0)
	{
		return ;
	}	
	//窗口的位置
	GetWindowRect(hWndPoint,&rectFrame);

	//将窗口左上点置为（0，0）后，窗口四个角点的坐标
	rectFrame.right -=rectFrame.left;
	rectFrame.bottom-=rectFrame.top;
	rectFrame.left = 0;rectFrame.top = 0;

	hdc = GetWindowDC(hWndPoint); 
	SetROP2(hdc,R2_NOT);
	
	hPen =CreatePen(PS_INSIDEFRAME,iPenWidth,RGB(255,144,30));
	SelectObject(hdc,hPen);
	SelectObject(hdc,GetStockObject(NULL_BRUSH));

	Rectangle(hdc,rectFrame.left,rectFrame.top,rectFrame.right,rectFrame.bottom);

	ReleaseDC(hWndPoint,hdc);
	DeleteObject(hPen);
}


BOOL GetProcessNameById(DWORD pid, LPSTR pprocessNmae, DWORD dwLen)
{
	USES_CONVERSION;

	BOOL bRun = FALSE;
	DWORD dwRun = 0;
	if (pprocessNmae == NULL)
	{
		return bRun;
	}
	HANDLE hProcess=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	PROCESSENTRY32* pinfo = NULL;
	pinfo=new PROCESSENTRY32;
	if (NULL == pinfo)
	{
		CloseHandle(hProcess);
		return bRun;
	}
	pinfo->dwSize=sizeof(PROCESSENTRY32);
	BOOL report = TRUE;	
	report=Process32First(hProcess,pinfo);
	while(report)
	{
		if (pinfo->th32ProcessID == pid)
		{
			if (_tcslen(pinfo->szExeFile) <= dwLen)
			{
				strcpy_s(pprocessNmae, dwLen, T2A(pinfo->szExeFile));
				bRun = TRUE;
			}
			break;
		}
		report=Process32Next(hProcess, pinfo); 
	}
	delete pinfo;
	pinfo = NULL;
	CloseHandle(hProcess);
	return dwRun;
}


//设置connect超时
int SockSelectConnect( __in SOCKET s,
	__in_bcount(namelen) const struct sockaddr FAR *name,
	__in int namelen , int iTimeOutSecond)
{

	int iRun = SOCKET_ERROR;
	int iFlg = SOCKET_ERROR;

	unsigned long ul = 1;
	timeval tm;
	fd_set set;

	//超时时间设置
	tm.tv_sec  = iTimeOutSecond;
	tm.tv_usec = 0;

	iFlg = ioctlsocket(s, FIONBIO, &ul); //先设置为非阻塞模式
	if (iFlg == SOCKET_ERROR)
	{
		return iRun;
	}

	//先尝试连接一下
	if( connect(s, (struct sockaddr *)name, namelen) == -1)
	{
		//连接失败，设置超时用select等待时间

		FD_ZERO(&set);
		FD_SET(s, &set);

		int error = -1;
		int len = sizeof(int);

		if( select(NULL, NULL, &set, NULL, &tm) > 0)
		{
			//看看有没有错
			getsockopt(s, SOL_SOCKET, SO_ERROR, (char *)&error, /*(socklen_t *)*/&len);
			if(error == 0)  //如果连接建立成功，这个错误值将是0
			{
				iRun = 0;
			}
		} 
	}
	else 
	{
		iRun = 0;
	}
	ul = 0;
	iFlg = ioctlsocket(s, FIONBIO, &ul); //设置为阻塞模式
	if (iFlg == SOCKET_ERROR)
	{
		iRun = SOCKET_ERROR;
	}
	return iRun;
}

DWORD GetTimeFromServer(BOOL bUserProxy, char *ip_addr, char* ip_Proxy, int port_Proxy)
{
	// 参数ip_addr:表示指定的时间服务器IP地址
	// 返回：自1900年1月1日午0时0分0秒至今的毫秒数 或 0（表示获取失败）
	DWORD dwRun = 0;
	// 默认的时间服务器为"国家授时中心"
	if (ip_addr == NULL || strlen(ip_addr) == 0)
	{
		ip_addr = ("128.138.140.44");
	}
	//是否使用代理,如果使用，并且代理服务器IP不正确，退出
	if (bUserProxy &&
		(ip_Proxy == NULL))
	{
		return dwRun;
	}

	// 定义WSADATA结构体对象
	WSADATA date;
	// 定义版本号码
	WORD w = MAKEWORD(2, 0);
	// 初始化套接字库
	if ( ::WSAStartup(w, &date) != 0 )
	{
		return dwRun;
	}

	// 定义连接套接字句柄
	SOCKET s = INVALID_SOCKET;

	//使用代理
	if (bUserProxy)
	{
		CSocks cs;
		cs.SetVersion(SOCKS_VER5);
		cs.SetSocksPort(port_Proxy);
		cs.SetSocksAddress(ip_Proxy);
		cs.SetDestinationPort(37);
		cs.SetDestinationAddress(ip_addr);
		s = cs.Connect();
		if (cs.m_IsError)
		{
			::WSACleanup();
			return dwRun;
		}
	}
	else  //不使用代理
	{
		// 定义接收信息保存变量
		DWORD  m_serverTime;
		// 创建TCP套接字
		s = ::socket(AF_INET, SOCK_STREAM, 0);
		if (INVALID_SOCKET == s)
		{
			// 关闭套接字句柄
			::closesocket(s);
			// 释放套接字库
			::WSACleanup();
			return dwRun;
		}

		// 定义套接字地址结构
		sockaddr_in addr;

		// 初始化地址结构
		addr.sin_family = AF_INET;
		addr.sin_port = htons(37);
		addr.sin_addr.S_un.S_addr = inet_addr(ip_addr);

		// 连接
		if ( SockSelectConnect(s, (sockaddr*)&addr, sizeof(addr), 1) != 0 )
		{
			dwRun = ::WSAGetLastError();
			// 关闭套接字句柄
			::closesocket(s);
			// 释放套接字库
			::WSACleanup();
			return dwRun;
		}
	}

	int nNetTimeout = 2000;  //超时时间，因为设置的超时时间大大高于心跳，所以理论上不可能出现接收数据的时间大于超时时间，所以肯定是套接字发生了问题，例如拔出网线
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, ( char * )&nNetTimeout, sizeof( nNetTimeout ) );

	// 接收
	if ( ::recv(s, (char *)&dwRun, 4, MSG_PEEK) <= 0 )
	{
		// 关闭套接字句柄
		::closesocket(s);
		// 释放套接字库
		::WSACleanup();
		return dwRun;
	}

	// 关闭套接字句柄
	::closesocket(s);
	// 释放套接字库
	::WSACleanup();
	// 网络字节顺序转换为主机字节顺序
	dwRun = ::ntohl(dwRun);

	// 返回接收到的数据
	return dwRun;
}

SYSTEMTIME FormatServerTime(DWORD serverTime)
{
	 FILETIME      ftNew ;     
     SYSTEMTIME    stNew ;     

     stNew.wYear         = 1900 ;
     stNew.wMonth        = 1 ;
     stNew.wDay          = 1 ;
     stNew.wHour         = 0 ;
     stNew.wMinute       = 0 ;
     stNew.wSecond       = 0 ;
     stNew.wMilliseconds = 0 ;
	 ::SystemTimeToFileTime (&stNew, &ftNew);

     /*
	   将SYSTEMTIME结构设定为1900年1月1日午夜（0时）。
	   并将这个SYSTEMTIME结构传递给SystemTimeToFileTime，将此结构转化为FILETIME结构。
	   FILETIME实际上只是由两个32位元的DWORD一起组成64位元的整数，
	   用来表示从1601年1月1日至今间隔为100奈秒（nanosecond）的间隔数。 
	 */     

	 LARGE_INTEGER li ;			       //64位大整数
     li = * (LARGE_INTEGER *) &ftNew;
     li.QuadPart += (LONGLONG) 10000000 * serverTime; 
     ftNew = * (FILETIME *) &li;
     ::FileTimeToSystemTime (&ftNew, &stNew);

	 // 返回时间（注意：这里返回的是格林尼治时间，与北京时间相差8小时）
	 return stNew;
}

SYSTEMTIME Time_tToSystemTime(time_t t)
{

	tm temptm = *localtime(&t);
	SYSTEMTIME st = {1900 + temptm.tm_year, 

		1 + temptm.tm_mon, 

		temptm.tm_wday, 

		temptm.tm_mday, 

		temptm.tm_hour, 

		temptm.tm_min, 

		temptm.tm_sec, 

		0};
	return st;
}

time_t SystemTimeToTime_t( const SYSTEMTIME& st )
{
	tm temptm = {st.wSecond, 

		st.wMinute, 

		st.wHour, 

		st.wDay, 

		st.wMonth - 1, 

		st.wYear - 1900, 

		st.wDayOfWeek, 

		0, 

		0};
	return mktime(&temptm);

}

//比较两个时间
int CompareSystemTime(SYSTEMTIME s1, SYSTEMTIME s2)
{
	int iRun = 0;
	do 
	{
		//比较年
		if (s1.wYear > s2.wYear)  
		{
			iRun  = 1;
			break;
		}
		else if (s1.wYear < s2.wYear)
		{
			iRun  = -1;
			break;
		}
		//年相等，比较月
		if (s1.wMonth > s2.wMonth)  
		{
			iRun  = 1;
			break;
		}
		else if (s1.wMonth < s2.wMonth)
		{
			iRun  = -1;
			break;
		}
		//月相等，比较日
		if (s1.wDay > s2.wDay)  
		{
			iRun  = 1;
			break;
		}
		else if (s1.wDay < s2.wDay)
		{
			iRun  = -1;
			break;
		}
		//日相等，比较时
		if (s1.wHour> s2.wHour)  
		{
			iRun  = 1;
			break;
		}
		else if (s1.wHour < s2.wHour)
		{
			iRun  = -1;
			break;
		}
		//时相等，比较分钟
		if (s1.wMinute> s2.wMinute)  
		{
			iRun  = 1;
			break;
		}
		else if (s1.wMinute < s2.wMinute)
		{
			iRun  = -1;
			break;
		}
		//分钟相等，比较秒
		if (s1.wSecond> s2.wSecond)  
		{
			iRun  = 1;
			break;
		}
		else if (s1.wSecond < s2.wSecond)
		{
			iRun  = -1;
			break;
		}
	} while (false);
	return iRun;
}
