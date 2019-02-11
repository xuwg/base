// D_COMMON.cpp : ���� DLL Ӧ�ó���ĵ���������
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
		 //��ô�ӡ�������д�ӡ����
		 if (!GetJobs(hPrinter, &pJobs, &cJobs, &dwPrinterStatus))
		 {
			 break;
		 }
		 //û�����񲻹�
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
				 //��ֵ������ʱ��
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
			 // �𶯷���
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

//���ݾ����ö�������
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
		//���ض�̬��
		hNtDLL = LoadLibraryW(L"ntdll.dll");   
		if (hNtDLL == NULL)       
		{
			break;
		}  
		//��ȡZwQueryObject����ָ��
		ZWQUERYOBJECT ZwQueryObject = (ZWQUERYOBJECT)GetProcAddress(hNtDLL, "ZwQueryObject"); 
		if (ZwQueryObject == NULL) 
		{ 
			break; 
		} 

		//��ȡ��������
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


//��ȡ��ǰĿ¼
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

//��������CreateDirectoryPath
//���ܣ�����һ��·��
//������_pszPathҪ������·��
//����ֵ���ɹ���TRUE ʧ�ܣ�FALSE
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
	//��Ŀ¼
	if (GetFileAttributesA(pszDirectoryPath) & FILE_ATTRIBUTE_DIRECTORY)
	{
		BOOL bRet = TRUE;
		WIN32_FIND_DATAA findFileData;
		char tempFileFind[500] = {0}; 
		sprintf_s(tempFileFind,"%s\\*.*",pszDirectoryPath); 
		//���ļ�
		HANDLE hFind = ::FindFirstFileA(tempFileFind, &findFileData);
		if (INVALID_HANDLE_VALUE == hFind)
		{
			return FALSE;
		}
		while(bRet) 
		{ 
			//�����Ŀ¼
			if(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
			{ 
				//., ..������
				if (findFileData.cFileName[0] != '.')
				{
					char tempDir[500] = {0}; 
					sprintf_s(tempDir,"%s\\%s",pszDirectoryPath,findFileData.cFileName); 
					RemoveDir(tempDir); 
				}
			}
			else //�ļ�ֱ��ɾ��
			{ 
				char tempFileName[500] = {0}; 
				sprintf_s(tempFileName,"%s\\%s",pszDirectoryPath,findFileData.cFileName); 
				if(!DeleteFileA(tempFileName))
				{
					D_Log::WriteLog(D_Log::LOG_NORMAL,"[RemoveDir]��ɾ��Ŀ¼���ļ�ʧ�� %s Error=%d",tempFileName, GetLastError());
					break;
				}
			}  
			bRet = ::FindNextFileA(hFind, &findFileData);
		} 
		::FindClose(hFind);
		hFind = INVALID_HANDLE_VALUE;
		if(!RemoveDirectoryA(pszDirectoryPath)) 
		{ 
			D_Log::WriteLog(D_Log::LOG_NORMAL,"[RemoveDir]:ɾ��Ŀ¼ʧ�� %s Error=%d",pszDirectoryPath, GetLastError());
			return FALSE; 
		} 
	}
	else //���ļ�
	{
		DeleteFileA(pszDirectoryPath); 
	}
	return TRUE;
}

BOOL CopyFileMust(LPCSTR lpSrcFiles, LPCSTR LpDestPath)
{
	USES_CONVERSION;

	BOOL bRun = FALSE;
	char szDestPath[MAX_PATH] = {0}; //Ŀ��·��
	char szExpand[MAX_PATH/2] = {0};   //��չ��
	char szFileName[MAX_PATH/2] = {0}; //��ʵ����
	char szTemp[MAX_PATH] = {0};
	char szRemovePath[MAX_PATH] = {0}; //ɾ���ļ�����Ŀ¼
	//��ȡ��Ʒ��װĿ¼ ע������
	HKEY hKey = NULL;
	DWORD dwType = REG_SZ;
	DWORD dwCont = MAX_PATH;
	char szSubKey[]=DUL_BOX_REG_NAME;
	do 
	{
		//���������Ч�Ժ�Դ·���Ƿ����
		if (lpSrcFiles == NULL || LpDestPath == NULL || !PathFileExistsA(lpSrcFiles))
		{
			D_Log::WriteLog(D_Log::LOG_NORMAL,"[CopyFileMust]:Դ�ļ�·�����Ϸ��򲻴���");
			break;
		}
		//���Ŀ��·���ĺϷ���
		LPCSTR pTemp = strrchr( LpDestPath, '\\');
		if (!pTemp )
		{
			D_Log::WriteLog(D_Log::LOG_NORMAL,"[CopyFileMust]:Ŀ��·��������Ч��·����ʽ��%s",LpDestPath);
			break;
		}
		D_Log::WriteLog(D_Log::LOG_NORMAL,"[CopyFileMust]:��Ҫ�滻���ļ���%s %s",lpSrcFiles,LpDestPath);
		strncpy_s(szDestPath,LpDestPath, (pTemp-LpDestPath)/sizeof(TCHAR));
		//����Ŀ��·��
		if (!CreateDirectoryPath(szDestPath))
		{
			D_Log::WriteLog(D_Log::LOG_NORMAL,"[CopyFileMust]:����Ŀ��·��ʧ�ܣ�%s",szDestPath);
			break;
		}
		//���Ŀ���ļ��Ƿ����
		if (PathFileExistsA(LpDestPath))
		{
			D_Log::WriteLog(D_Log::LOG_NORMAL,"[CopyFileMust]:Ŀ��·�����Ѿ������˸��ļ���%s",LpDestPath);
			//�����չ��
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
			//���ַ�����-ǰ����ַ�����
			int iLen = strlen(LpDestPath) - (pExd-LpDestPath);
			//�����չ��
			strncpy_s(szExpand, pExd, iLen);
			//����ļ���(ȥ��Ŀ¼��ʾ)
			strncpy_s(szFileName, pName+1, pExd-pName-1);
			//�������������
			strncpy_s(szTemp, szDestPath, MAX_PATH);
			strcat_s(szTemp, "\\");
			strcat_s(szTemp, szFileName);
			strcat_s(szTemp, "_old");
			strcat_s(szTemp, szExpand);
			//���ڸ���ɾ��
			if(!MoveFileA(LpDestPath, szTemp))
			{
				D_Log::WriteLog(D_Log::LOG_NORMAL,"[CopyFileMust]:��Դ�ļ�����ʧ�ܣ�%s %s",LpDestPath, szTemp);
				break;
			}
			//��ȡע����ȡϵͳ��װĿ¼(û�еĻ������ڵ�ǰĿ¼�´�����ʱ�ļ���)
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
				D_Log::WriteLog(D_Log::LOG_NORMAL,"[CopyFileMust]:����ʱ�ļ��ƶ�����Ҫɾ����Ŀ¼ʧ�ܣ�%s %s",szTemp, szRemovePath);
				break;
			}
		}
		bRun = CopyFileA(lpSrcFiles, LpDestPath, FALSE);
	} while (false);
	if (!bRun)
	{
		D_Log::WriteLog(D_Log::LOG_NORMAL,"[CopyFileMust]:�������յ��ļ�ʧ�ܣ�%s %s Error=%d",lpSrcFiles, LpDestPath, GetLastError());
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
	char szRealPath[MAX_PATH] = {0};   //��Ҫ��������ʵ·��
	char szExpand[MAX_PATH/2] = {0};   //��չ��
	char szFileName[MAX_PATH/2] = {0}; //��ʵ����
	char szModePath[MAX_PATH] = {0};   //�м�BUF
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
		//�ֽ��ļ���
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
		//���ַ�����-ǰ����ַ�����
		int iLen = strlen(lpFilePath) - (pExd-lpFilePath);
		//�����չ��
		strncpy_s(szExpand, pExd, iLen);
		//����ļ���(ȥ��Ŀ¼��ʾ)
		strncpy_s(szFileName, pName+1, pExd-pName-1);
		//ȥ���汾��
		if (lpVersion != NULL)
		{
			size_t sIndex = 0;
			string strName = szFileName;
			sIndex = strName.rfind(lpVersion);
			if (sIndex != std::string::npos)
			{
				ZeroMemory(szFileName, MAX_PATH/2);
				strncpy_s(szFileName, strName.c_str(), sIndex-1); //-1��Ϊ����һ���»���Ҫȥ��
			}

		}
		char tfalg = *(szFileName+strlen(szFileName)-1);
		//ȥ����ʶ
		*(szFileName+strlen(szFileName)-1) = 0;
		switch (tfalg)
		{
		case '1':  //ϵͳsystem32Ŀ¼
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
		case '0':  //�����װĿ¼
			{
				//��ȡע����ȡϵͳ��װĿ¼(û�еĻ������ڵ�ǰĿ¼�´�����ʱ�ļ���)
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
			//ȱʡ��Ҳ��������LOSTDIR_TEMPĿ¼��
		default:
			{
				//��ȡע����ȡϵͳ��װĿ¼(û�еĻ������ڵ�ǰĿ¼��Ѱ����ʱ�ļ���)
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
		//�����ļ�
		bRun = CopyFileMust(lpFilePath, szRealPath);
		if(bRun <= 0)
		{
			D_Log::WriteLog(D_Log::LOG_NORMAL,"[CopyFileMust]:ʧ�ܣ�%s--->%s Error = %d",lpFilePath, szRealPath, GetLastError());
			break;
		}
		//ɾ��Դ�ļ�
		bRun = DeleteFileA(lpFilePath);
	} while (false);
	return bRun;
}


BOOL MoveFileWithDirFlag(LPCSTR lpFilePath)
{
	USES_CONVERSION;

	BOOL bRun  = FALSE;
	char szRealPath[MAX_PATH] = {0};   //��Ҫ��������ʵ·��
	char szFileName[MAX_PATH/2] = {0}; //��ʵ����
	char szModePath[MAX_PATH] = {0};   //�м�BUF
	char szTempDir[MAX_PATH] = {0};    //�м仺��Ŀ¼��
	//��ȡ��Ʒע������
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


		//��ȡ��ִ���ļ���
		LPCSTR pName = strrchr(lpFilePath, '//');
		if (!pName )
		{
			pName = strrchr(lpFilePath, '\\');
			if (!pName)
			{
				break;
			}
		}
		//���ַ�����-ǰ����ַ�����
		int iLen = strlen(lpFilePath) - (pName-lpFilePath);
		//����ļ���
		strncpy_s(szFileName, pName+1, iLen-1);


		//��ȡǰ���Ŀ¼
		strncpy_s(szModePath, lpFilePath, strlen(lpFilePath)-iLen);
		LPCSTR pTemp = strrchr(szModePath, '\\');
		if (!pTemp )
		{
			break;
		}
		strncpy_s(szTempDir, pTemp+1, strlen(szModePath)-(pTemp-szModePath)-1);

		D_Log::WriteLog(D_Log::LOG_NORMAL,"[MoveFileWithDirFlag]:��ȡ��Ҫ�ƶ����ļ�Ŀ¼��ʶ %s",szTempDir);

		if (strcmp(szTempDir, SYSDIR_TEMP) == 0) //ϵͳsystem32
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
		else if(strcmp(szTempDir, MODULE_TEMP) == 0) //��Ʒ��װĿ¼
		{
			
			//��ȡע����ȡϵͳ��װĿ¼(û�еĻ������ڵ�ǰĿ¼��Ѱ����ʱ�ļ���)
			if(::RegOpenKeyExA(HKEY_LOCAL_MACHINE ,szSubKey, NULL, KEY_READ|KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS)
			{
				RegQueryValueExA(hKey, "Path", NULL, &dwType, (LPBYTE)szRealPath, &dwCont);
				::RegCloseKey(hKey);
				hKey = NULL;
			}
			//�����ȡע���ʧ��
			if (0 == strcmp(szRealPath, ""))
			{
				//�ͷŵ���ǰĿ¼
				if(0 != GetModuleFileNameA(NULL, szModePath, MAX_PATH))
				{

					char* pTemp = strrchr( szModePath, '\\');
					if ( pTemp )
					{
						*pTemp = 0;
					}
					strncpy_s(szRealPath, szModePath, MAX_PATH);
					D_Log::WriteLog(D_Log::LOG_NORMAL,"[MoveFileWithDirFlag]:��ȡע���ʧ�ܣ���ȡ��ǰģ��·����%s",szRealPath);
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
		D_Log::WriteLog(D_Log::LOG_NORMAL,"[MoveFileWithDirFlag]:�����ļ���Ŀ¼·��������:%s",szRealPath);
		//�����ļ�
		if(!CopyFileMust(lpFilePath, szRealPath))
		{
			D_Log::WriteLog(D_Log::LOG_NORMAL,"[CopyFileMust]:�滻�ļ�ʧ��");
			break;
		}
		//ɾ��Դ�ļ�
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
	//��Ŀ¼
	if (GetFileAttributesA(lpDirPath) & FILE_ATTRIBUTE_DIRECTORY)
	{
		BOOL bRet = TRUE;
		WIN32_FIND_DATAA findFileData;
		char tempFileFind[500] = {0}; 
		sprintf_s(tempFileFind,"%s\\*.*",lpDirPath); 
		//���ļ�
		hFind = ::FindFirstFileA(tempFileFind, &findFileData);
		if (INVALID_HANDLE_VALUE == hFind)
		{
			return FALSE;
		}
		while(bRet) 
		{ 
			//�����Ŀ¼
			if(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
			{ 
				//., ..������
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
			else //�ļ�ֱ���ƶ�
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
	else //���ļ�
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
	//���ж�һ���ж�����
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
	case 1: //��һ
	case 2: //�ܶ�
	case 3: //����
	case 4: //����
	case 5: //����
	case 6: //����
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
		//��ȡ��װĿ¼
		if(::RegOpenKeyExA(HKEY_LOCAL_MACHINE ,szSubKey, NULL, KEY_READ|KEY_WOW64_64KEY, &hKey) != ERROR_SUCCESS)
		{
			OutputDebugString(_T("RegOpenKeyExA is  fail"));		
			MzOutputDebugStringA("[PortHook]:GetProductDirA  RegOpenKeyExA ʧ�� Error Code:%d\n",GetLastError());
			break;
		}
		if(::RegQueryValueExA(hKey, "Path", NULL, &dwType, (LPBYTE)pDir, &dwCont) != ERROR_SUCCESS)
		{
			OutputDebugString(_T("::RegQueryValueExA is  fail"));
			MzOutputDebugStringA("[PortHook]:GetProductDirA RegQueryValueExA ʧ�� Error Code:%d\n",GetLastError());
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
		//��ȡ��װĿ¼
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
// 	//ѭ�������ڴ棬��ֹ��С����
// 	do 
// 	{
// 		lpMsg = new char[iCount*MAX_PATH];
// 		if (lpMsg)
// 		{	
// 			ZeroMemory(lpMsg, iCount*MAX_PATH);
// 			//��ʽ����־��Ϣ
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
// 		//��ȡʧ�ܣ��ͻ�ȡ��ǰĿ¼
// 		GetModelDir(szLogFigPaht, MAX_PATH);
// 	}
// 	else
// 	{
// 		strcat_s(szLogFigPaht, "\\");
// 	}
// 	strcat_s(szLogFigPaht, LOG_FILE_PARH);
// 	CreateDirectoryPath(szLogFigPaht);
// 	//���ϵͳʱ��
// 	::GetLocalTime(&systemTime);
// 	//��õ�ǰ�ļ�����
// 	sprintf_s(szFileName, "%d-%02d-%02d.log", systemTime.wYear, systemTime.wMonth, systemTime.wDay);
// 	//�ļ����·��
// 	sprintf_s(szLogFigPaht, "%s\\%s", szLogFigPaht, szFileName);
// 	//���ļ�
// 	fopen_s(&pFile,szLogFigPaht, "a+");
// 	if (pFile != NULL && lpMsg != NULL)
// 	{
// 		//д�ļ�ͷ
// 		sprintf_s(szFileName, "%02d-%02d-%02d", systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
// 		fprintf(pFile,"��%s��:",szFileName);
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
	//ɾ�������������
	WORD wYear = 0;
	WORD wDay = 0;
	WORD wMonth = 0;
	WORD wFlag  = 0;
	//��ȡ��־·��
	if(GetProductDirA(szLogDir, MAX_PATH))
	{
		strcat_s(szLogDir, "\\");
	}
	strcat_s(szLogDir, LOG_FILE_PARH);

	vector<string> vecAllFileInfo;
	DirectoryList(szLogDir, vecAllFileInfo);
	// ���˵��� *.log �ļ�
	FilterFile(vecAllFileInfo);
	
	//���ϵͳʱ��
	::GetLocalTime(&systemTime);
	//�ҵ���Ҫɾ�� ����ĳһ��֮ǰ��������־

	do 
	{
		if ( systemTime.wDay >= 30)
		{
			// �����ڱ����ڵĵ�һ��
			wDay =systemTime.wDay-29;
			wYear = systemTime.wYear;
			wMonth = systemTime.wMonth;
			break;
		}
		if (systemTime.wMonth == 1) //�������һ��
		{
			wMonth  = 12;
			wYear = systemTime.wYear-1;
			wDay = systemTime.wDay + 2;
			break;
		}else{
			wMonth = systemTime.wMonth-1;
			wYear = systemTime.wYear;

			//����ϸ����ж�����
			WORD nDay = GetMonthDay(wYear, wMonth);
			//����������Ǽ���
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
// 	//ɾ�������������
// 	WORD wYear = 0;
// 	WORD wDay = 0;
// 	WORD wMonth = 0;
// 	WORD wFlag  = 0;
// 	SYSTEMTIME systemTimeRightPar;

// 	vector<string> vecAllFileInfo;
// 	vector<string>::iterator itor;
	

// 	//��ȡ�ļ�����Ŀ¼·��
// 	if(GetProductDirA(szHandoverDir, MAX_PATH))
// 	{
// 		strcat(szHandoverDir, "\\");
// 	}
// 	strcat(szHandoverDir, D_BOX_HANDOVERS_FILEDIRA);

// 	//��ȡĿ¼�µ��ļ�
// 	DirectoryList(szHandoverDir, vecAllFileInfo);

// 	//���ϵͳʱ��
// 	::GetLocalTime(&systemTime);
// 	//�ҵ���Ҫɾ�� ����ĳһ��֮ǰ�������ļ�
// 	do 
// 	{
// 		if ( systemTime.wDay >= 30)
// 		{
// 			// �����ڱ����ڵĵ�һ��
// 			wDay =systemTime.wDay-29;
// 			wYear = systemTime.wYear;
// 			wMonth = systemTime.wMonth;
// 			break;
// 		}
// 		if (systemTime.wMonth == 1) //�������һ��
// 		{
// 			wMonth  = 12;
// 			wYear = systemTime.wYear-1;
// 			wDay = systemTime.wDay + 2;
// 			break;
// 		}else{
// 			wMonth = systemTime.wMonth-1;
// 			wYear = systemTime.wYear;

// 			//����ϸ����ж�����
// 			WORD nDay = GetMonthDay(wYear, wMonth);
// 			//����������Ǽ���
// 			wDay = nDay+systemTime.wDay-29;
// 		}
// 	} while (FALSE);

// 	//��ʽ�����ʱ���ΪSYSTEMTIME�ṹ��
// 	systemTimeRightPar.wYear = wYear;
// 	systemTimeRightPar.wMonth = wMonth;
// 	systemTimeRightPar.wDay = wDay;

// 	for (itor = vecAllFileInfo.begin(); itor!= vecAllFileInfo.end(); ++itor)
// 	{
// 		//��ȡ�ļ��Ĵ���ʱ��
// 		HANDLE hFile = CreateFileA((*itor).c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
// 		if (hFile != INVALID_HANDLE_VALUE)
// 		{
// 			SYSTEMTIME systemTimeOneDay;
// 			FILETIME ftCreate, ftModify, ftAccess;
// 			if (GetFileTime(hFile, &ftCreate, &ftAccess, &ftModify))
// 			{
// 				FileTimeToSystemTime(&ftCreate, &systemTimeOneDay);
// 				//�ж��ļ��ǲ��ǳ�������ʱ������֮ǰ������
// 				if (bIsLeftDateLessThanRightDate(systemTimeOneDay, systemTimeRightPar ))
// 				{
// 					CloseHandle(hFile);
// 					DeleteFileA((*itor).c_str()); //ɾ���ļ�
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
	//ɾ�������������
	WORD wYear = 0;
	WORD wDay = 0;
	WORD wMonth = 0;
	WORD wFlag  = 0;
	//��ȡ��־·��
	if (GetProductDirA(szLogDir, MAX_PATH))
	{
		_tcscat_s(szLogDir, _T("\\"));
	}
	strcat_s(szLogDir, LOG_FILE_PARH);
	//���ϵͳʱ��
	::GetLocalTime(&systemTime);
	//ѭ����������



	for (WORD i = 0; i<365; i++)
	{
		// ��ü���ǰ
		if (systemTime.wDayOfWeek == 0)
		{
			wFlag= 30 + i;
		}
		else
		{
			wFlag = systemTime.wDayOfWeek+ i;
		}

		if (systemTime.wDay > wFlag) //�����ڱ�����
		{
			wDay  = systemTime.wDay - wFlag;
			wMonth = systemTime.wMonth;
			wYear = systemTime.wYear;
		}
		else //�������ϸ���
		{
			if (systemTime.wMonth == 1) //�������һ��
			{
				wMonth  = 12;
				wYear = systemTime.wYear-1;
			}
			else //�ϸ��²���һ��
			{

				wMonth = systemTime.wMonth-1;
				wYear = systemTime.wYear;

			}
			//����ϸ����ж�����
			WORD nDay = GetMonthDay(wYear, wMonth);
			//����������Ǽ���
			wDay = nDay-(wFlag-systemTime.wDay);
		}	
		//�ļ����·��
		sprintf_s(szFileName, "%d-%02d-%02d.log", wYear, wMonth, wDay);
		sprintf_s(szLogFigPaht, "%s\\%s", szLogDir, szFileName);
		//ɾ���������־
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
	//�����
	const char EncodeTable[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	//����ֵ
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
	//��ʣ�����ݽ��б���
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
	//�����
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
	//����ֵ
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
		else// �س�����,����
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
���ݽ�������ý��̾��
RUN:���̾����ʧ�ܷ���INVALID_HANDLE_VALUE
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
		//ȫ��ת���ɴ�д
		transform(str1.begin(), str1.end(), str1.begin(), toupper);
		transform(str2.begin(), str2.end(), str2.begin(), toupper);

		//swprintf(szMsg, L"EnumProcess name = %s pid =%d\n", pinfo->szExeFile, pinfo->th32ProcessID);
		//OutputDebugStringW(szMsg);

		if((str1.compare(str2) == 0))
		{
			dwExplorerId = pinfo->th32ProcessID;
			//OutputDebugStringW(L"����ҵ��˸ý��̡�");
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
			//ȫ��ת���ɴ�д
			transform(str1.begin(), str1.end(), str1.begin(), toupper);
			transform(str2.begin(), str2.end(), str2.begin(), toupper);
		
			if((str1.compare(str2) == 0))
			{
				vProcessId.push_back(pinfo->th32ProcessID);  //�������ID
			}
			report=Process32NextW(hModule, pinfo); 
		}

		if (vProcessId.size() <= 0)
		{
			break;
		}
		//�������ID���ڣ������ڴ汣��
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
			//���Դ򿪽��̣���ȡ�������·��(ϵͳ���̻��ʧ�ܣ��������ν)
			hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,pinfo->th32ProcessID);
			if (hProcess != NULL)
			{
				//��ȡ��ִ���ļ�����Ŀ¼
				WCHAR wszProcessPath[MAX_PATH+1]={0};
				GetModuleFileNameExW(hProcess,NULL,wszProcessPath,MAX_PATH+1);

				CloseHandle(hProcess);
				hProcess = NULL;

				//��ȡĿ¼
				WCHAR* tmp = (WCHAR*)wcsrchr(wszProcessPath, L'\\');
				if(tmp != NULL )
				{
					*tmp = NULL;
				}
				//·��ת������д
				str1 = wszProcessPath;
				transform(str1.begin(), str1.end(), str1.begin(), toupper);
				
				//�Ƚ�·���Ƿ�ƥ��
				if((str1.compare(str2) == 0))
				{
					vProcessId.push_back(pinfo->th32ProcessID);  //�������ID
				}	
			}
			report=Process32NextW(hModule, pinfo); 
		}

		if (vProcessId.size() <= 0)
		{
			break;
		}
		//�������ID���ڣ������ڴ汣��
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
	DWORD dwNeeded = 0;            //��Ҫ�����ֽ�
	DWORD dwModulesCount = 0;      //��Ҫ���ٸ�ģ������dwNeeded/sizeof(HMODULE)��
	HMODULE* phMods = NULL;        //������ģ����������
	char szModeName[MAX_PATH] = {0};
	do 
	{
		if (hProcess == INVALID_HANDLE_VALUE || lpModulePath == NULL)
		{
			break;
		}
		//���Ȼ�ȡ������Ҫ���ٿռ�
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


		//�ٴ�ö�ٻ��ģ��
		if(!EnumProcessModules(hProcess, phMods, sizeof(HMODULE)*(dwModulesCount+1), &dwNeeded))
		{
			break;
		}

		//ѭ�����ģ��
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

//  //��ȡ������ע������汣����������������
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
// 		 //��ȡ��װĿ¼
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

//  //�������������������д��ע���
//  BOOL SetAutoRun(char* pRandName, DWORD dwCont)
//  {
// 	 BOOL bRun = FALSE;
// 	 HKEY hKey = NULL;
// 	 DWORD dwType = REG_SZ;
// 	 do
// 	 {
// 		 //��ȡϵͳ����������
// 		 if (::RegOpenKeyExA(HKEY_LOCAL_MACHINE , DUL_BOX_REG_NAME, NULL, KEY_SET_VALUE | KEY_WOW64_64KEY, &hKey) != ERROR_SUCCESS)
// 		 {
// 			 break;
// 		 }
// 		 //д��ע���
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
			//������ͬ��ɱ���ý���
			HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE,FALSE,pinfo->th32ProcessID); 
			if (hProcess != NULL)
			{
				if(!::TerminateProcess(hProcess,0))
				{
					_stprintf(szBuf, _T("ǿɱTerminateProcessʧ�ܣ�PID��%d Error��%d"),pinfo->th32ProcessID, GetLastError());
					OutputDebugString(szBuf);
					bRun = FALSE;                       //��һ��ûɱ�����ж�ʧ�ܣ�һ����������
				}
				CloseHandle(hProcess); 
				hProcess = NULL;
			}
			else
			{
				_stprintf(szBuf, _T("ǿɱOpenProcesssʧ�ܣ�PID��%d Error��%d"),pinfo->th32ProcessID, GetLastError());
				OutputDebugString(szBuf);
				bRun = FALSE;   
			}
			
		}
		report=Process32Next(hProcessInfo, pinfo); 
	}
	if (bRun)
	{
		OutputDebugString(_T("-----ǿɱ�ɹ�-----"));
		OutputDebugStringA(pprocessNmae);
		OutputDebugString(_T("----------------"));
	}
	else
	{
		OutputDebugString(_T("-----ǿɱʧ��-----"));
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
	// ����·��
	char FullPathName[MAX_PATH];
	strcpy(FilePathName, Path);
	strcat(FilePathName, "\\*.*");
	hError = FindFirstFileA(FilePathName, &FindData);
	if (hError == INVALID_HANDLE_VALUE)
	{
		printf("����ʧ��!");
		return 0;
	}
	while(::FindNextFileA(hError, &FindData))
	{
		// ����.��..
		if (strcmp(FindData.cFileName, ".") == 0 
			|| strcmp(FindData.cFileName, "..") == 0 )
		{
			continue;
		}

		// ��������·��
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
		// ����������ļ�
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

//�õ���ǰ����·��
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
//�õ�������������·��
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

//�õ������û�����·��
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
�������ܣ���ָ���ļ���ָ����Ŀ¼�´������ݷ�ʽ
����������
lpszFileName    ָ���ļ���ΪNULL��ʾ��ǰ���̵�EXE�ļ���
lpszLnkFileDir  ָ��Ŀ¼������ΪNULL��
lpszLnkFileName ��ݷ�ʽ���ƣ�ΪNULL��ʾEXE�ļ�����
wHotkey         Ϊ0��ʾ�����ÿ�ݼ�
pszDescription  ��ע
iShowCmd        ���з�ʽ��Ĭ��Ϊ���洰��
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
	IShellLink     *pLink;  //IShellLink����ָ��
	IPersistFile   *ppf; //IPersisFil����ָ��

	//����IShellLink����
	hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pLink);
	if (FAILED(hr))
		return FALSE;

	//��IShellLink�����л�ȡIPersistFile�ӿ�
	hr = pLink->QueryInterface(IID_IPersistFile, (void**)&ppf);
	if (FAILED(hr))
	{
		pLink->Release();
		return FALSE;
	}

	//Ŀ��
	if (lpszFileName == NULL)
		pLink->SetPath(A2T(_pgmptr));
	else
		pLink->SetPath(A2T(lpszFileName));

	//����Ŀ¼
	if (lpszWorkDir != NULL)
		pLink->SetWorkingDirectory(A2T(lpszWorkDir));

	//��ݼ�
	if (wHotkey != 0)
		pLink->SetHotkey(wHotkey);

	//��ע
	if (lpszDescription != NULL)
		pLink->SetDescription(A2T(lpszDescription));

	//��ʾ��ʽ
	pLink->SetShowCmd(iShowCmd);


	//��ݷ�ʽ��·�� + ����
	char szBuffer[MAX_PATH];
	if (lpszLnkFileName != NULL) //ָ���˿�ݷ�ʽ������
		sprintf(szBuffer, "%s\\%s", lpszLnkFileDir, lpszLnkFileName);
	else  
	{
		//û��ָ�����ƣ��ʹ�ȡָ���ļ����ļ�����Ϊ��ݷ�ʽ���ơ�
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
		//ע���׺��Ҫ��.exe��Ϊ.lnk
		sprintf(szBuffer, "%s\\%s", lpszLnkFileDir, pstr);
		int nLen = strlen(szBuffer);
		szBuffer[nLen - 3] = 'l';
		szBuffer[nLen - 2] = 'n';
		szBuffer[nLen - 1] = 'k';
	}
	//�����ݷ�ʽ��ָ��Ŀ¼��
	WCHAR  wsz[MAX_PATH];  //����Unicode�ַ���
	MultiByteToWideChar(CP_ACP, 0, szBuffer, -1, wsz, MAX_PATH);

	hr = ppf->Save(wsz, TRUE);

	ppf->Release();
	pLink->Release();
	return SUCCEEDED(hr);
}

// string GetDLBVersion()
// {
// 	USES_CONVERSION;

// 	//��ȡ�����ļ�·��
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
		// һ���ֽڴ������2λ�ַ����ָ�λ�͵�λ����λҪ����16
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
	// ��ʼ����ʱָ��
	pTemp = pData;

	for(i=0;i<uDataSize;i++)
	{
		if( !bStartMoney && EmfIsNumberOrLetter(pTemp[i]) )
		{
			// ����ǵ�һ���ҵ����֣����ʾ����ȡ��ʼ
			bStartMoney = TRUE;
			szDeskNo[uIndex++] = pTemp[i];		// ��������ֿ���������
		}
		else if( bStartMoney && EmfIsNumberOrLetter(pTemp[i]) )
		{
			szDeskNo[uIndex++] = pTemp[i];		// ��������ֿ���������
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
	// ��ʼ����ʱָ��
	pTemp = pData;
	memset(pMoneyValue,0,TRUST_BUFFER_SIZE);

	for(i=0;i<uDataSize;i++)
	{
		if( !bStartMoney && EmfIsNumber(pTemp[i]) )
		{
			// ����ǵ�һ���ҵ����֣����ʾ����ȡ��ʼ
			bStartMoney = TRUE;
			pMoneyValue[uIndex++] = pTemp[i];		// ��������ֿ���������
		}
		else if( bStartMoney && EmfIsNumber(pTemp[i]) )
		{
			pMoneyValue[uIndex++] = pTemp[i];		// ��������ֿ���������
		}
		else if( bStartMoney && pTemp[i] == '.' )
		{
			pMoneyValue[uIndex++] = pTemp[i];		// �����С���㿽��������
		}// 1000���ϵĽ�����ֶ��ţ����� 2,129
		else if( bStartMoney && ( pTemp[i] == ' ' ||  pTemp[i] == ',' ||  pTemp[i] == '��') )	// ����Ȧ���ļ���������м����ֿո���Ӣ�Ķ��Ŷ�Ҫ����
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
			//���ſ��ܳ��ֺ��֣������ų��֣�����1 201�Ľ����������	
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
	// �Ƚ���ǰ���ݿ�ת��Ϊ�ַ���
	pTemp = (unsigned char*)MzHeapAlloc(uDataSize*2+128);
	if( !pTemp )
	{
		return FALSE;
	}
	// ת��
	MzStringToHex((unsigned char*)pData,uDataSize,pTemp,uDataSize*2+128);
	// ��ת������ַ���ָ���в�����ָֽ��
	if( strlen(pCutCommand) )
	{
		for(uIndex=0;uIndex<uDataSize*2;uIndex++)
		{
			if( 0 == memcmp(pTemp+uIndex,pCutCommand,strlen(pCutCommand)) )
			{
				bFind = TRUE;
				// ���ص�ǰ�ļ��е�ƫ��
				*nPosition = uIndex/2;
				break;
			}
		}
	}
	else
	{
		// ����Ĭ��ָ��1
		for(uIndex=0;uIndex<uDataSize*2;uIndex++)
		{
			if( 0 == memcmp(pTemp+uIndex,szCutCmd1,strlen(szCutCmd1)) )
			{
				bFind = TRUE;
				// ���ص�ǰ�ļ��е�ƫ��
				*nPosition = uIndex/2;
				break;
			}
		}
		// ���Ĭ��ָ��1Ҳû�ҵ�������Ĭ��ָ��2
		if( !bFind )
		{
			for(uIndex=0;uIndex<uDataSize*2;uIndex++)
			{
				if( 0 == memcmp(pTemp+uIndex,szCutCmd2,strlen(szCutCmd2)) )
				{
					bFind = TRUE;
					// ���ص�ǰ�ļ��е�ƫ��
					*nPosition = uIndex/2;
					break;
				}
			}
		}
	}
	if( bFind )
	{
		MzOutputDebugStringA("[APL]:�ڳ���%d�������ҵ���ָֽ��%s\n",uDataSize,pCutCommand);
	}
	else
	{
		MzOutputDebugStringA("[APL]:�ڳ���%d�����в��Ҳ�����ָֽ��%s\n",uDataSize,pCutCommand);
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
	// ��β����ʼ������ָֽ��
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
		// ���ú���ģʽ
	//szPos[uIndex++] = 0x1C;
	//szPos[uIndex++] = 0x26;

	StringCbCatA(szPos,TRUST_BUFFER_SIZE,pText);
	uIndex = strlen(szPos);
	// ȡ������ģʽ
	// ������ӣ��ʹ�ӡ������
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
		MzOutputDebugStringA("[PortHook]:EmfAddEsc ���ļ�[%ws]ʧ�� Error Code:%d\n",pRawFile,GetLastError());
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
			// �ƶ�����ָֽ��֮ǰ��д����Ҫ��ӵ�POS����
			SetFilePointer(hFile,uPostion,NULL,FILE_BEGIN);
			// ��ȡ��ָֽ�����������
			ReadFile(hFile,szTailData,uFileSize-uPostion,&dwRtnSize,NULL);
			// �ٴ��ƶ�����ָֽ��ǰ����������
			SetFilePointer(hFile,uPostion,NULL,FILE_BEGIN);
			// ��������
			WriteFile(hFile,pData,uDataSize,&dwRtnSize,NULL);
			// �����ǵ�����д��ȥ
			WriteFile(hFile,szTailData,uFileSize-uPostion,&dwRtnSize,NULL);
		}
	}while(0);
	// �ͷ���Դ
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
	if(X%2==0)//����������������ż������ô+1ʹ֮��Ϊ������ż���϶�����������  
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
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP,NULL,wText,-1,NULL,0,NULL,FALSE);// WideCharToMultiByte������
	char *psText;  // psTextΪchar*����ʱ���飬��Ϊ��ֵ��std::string���м����
	psText = new char[dwNum];
	WideCharToMultiByte (CP_OEMCP,NULL,wText,-1,psText,dwNum,NULL,FALSE);// WideCharToMultiByte���ٴ�����
	szDst = psText;// std::string��ֵ
	delete []psText;// psText�����
}


//��WORD��������ֽ���Ϊlittle-endian�����˳��ַ�����β�Ŀո�
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

//��ȡӲ��ID
BOOL GetPhyDriveSerial(LPSTR pModelNo, LPSTR pSerialNo)
{
	//-1����Ϊ SENDCMDOUTPARAMS �Ľ�β�� BYTE bBuffer[1];
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


//�˳��ַ�����ʼλ�õĿո�
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


//�ڲ�����Ӳ��ID��cpuID������̨�������Ψһ��ʶ
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
		//���Ӳ��ID
		if(!GetPhyDriveSerial(szModelNo, szSerialNo))
		{
			break;
		}
		TrimStart(szSerialNo);
		//��ȡCPUID
		__cpuidex(adwBuf, 1,0);
		sprintf(szCpuIdNo, "%08X%08X", adwBuf[3], adwBuf[0]);

		//���Ӳ��ID��ǰ16λ
		memcpy(szHDTemp,  szSerialNo, 16);

		//����ID
		sprintf(szId, "%s%016s", szCpuIdNo, szHDTemp);

		//Ϊ�˷�ֹ���������ΨһID������һ������Ȼ�����ļ��ʺ�С����ȡ�����ļ���������ֱ�ʶ��һ���ӷ����õ�����ΨһID��������ֱ�ʶĬ����0����Ӱ�죬ֻ�����������ظ���ʱ�������
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


//��"012345678"��ʽ�������ַ���ת���ɡ���һ�����������߰ˡ���ʽ�������ַ���
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
				strRun += "��";
			}
			break;
		case '1':
			{
				strRun += "һ";
			}
			break;
		case '2':
			{
				strRun += "��";
			}
			break;
		case '3':
			{
				strRun += "��";
			}
			break;
		case '4':
			{
				strRun += "��";
			}
			break;
		case '5':
			{
				strRun += "��";
			}
			break;
		case '6':
			{
				strRun += "��";
			}
			break;
		case '7':
			{
				strRun += "��";
			}
			break;
		case '8':
			{
				strRun += "��";
			}
			break;
		case '9':
			{
				strRun += "��";
			}
			break;
		default:
			strRun += ctemp;
			break;
		}

	}
	return strRun;
}



//��2017��ʽ�������ַ���ת���ɡ���ǧ��һʮ�ߡ���ʽ�����������ַ���
string NumberValueToChinese(char* strNumber)
{
	string strRun = "";
	int iLen  = 0;
	char ctemp;
	int iIndex = 0;

	char szScale[][10] = {
		"",
		"ʮ",
		"��",
		"ǧ",
		"��",
		"ʮ��",
		"����",
		"ǧ��",
		"��",
		"ʮ��",
		"����",
		"ǧ��",
		"��",
	};
	BOOL bBegin = FALSE;   //�Ƿ������Ч����
	BOOL bZeroArise = FALSE; //ǰ����ڵ��Ƿ���0

	if (strNumber == NULL)
	{
		return strRun;
	}
	//����13λ�����������ˣ��Ƿ���
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
				//֤���Ѿ���ʼ��
				if (bBegin)
				{
					if (!bZeroArise) //ǰ��û�г���0
					{
						strRun += "��";
						bZeroArise = TRUE;
					}
				}
			}
			break;
		case '1':
			{
				if ((strRun.length() >0) || (iIndex != 1))   //����ַ������Ȳ�Ϊ0 ����ǰ���Ѿ�����Ч������ ���߲��� ʮλ
				{
					strRun += "һ";
					strRun += szScale[iIndex];
				}
				else  //��ʮλ�������������ʮλ��������ʮλ��1
				{
					strRun += szScale[iIndex]; //�������һ
				}
				bBegin = TRUE;
				bZeroArise = FALSE;
			}
			break;
		case '2':
			{
				if (iIndex> 1)
				{
					strRun += "��";
				}
				else
				{
					strRun += "��";
				}
				strRun += szScale[iIndex];
				bBegin = TRUE;
				bZeroArise = FALSE;
			}
			break;
		case '3':
			{
				strRun += "��";
				strRun += szScale[iIndex];
				bBegin = TRUE;
				bZeroArise = FALSE;
			}
			break;
		case '4':
			{
				strRun += "��";
				strRun += szScale[iIndex];
				bBegin = TRUE;
				bZeroArise = FALSE;
			}
			break;
		case '5':
			{
				strRun += "��";
				strRun += szScale[iIndex];
				bBegin = TRUE;
				bZeroArise = FALSE;
			}
			break;
		case '6':
			{
				strRun += "��";
				strRun += szScale[iIndex];
				bBegin = TRUE;
			}
			break;
		case '7':
			{
				strRun += "��";
				strRun += szScale[iIndex];
				bBegin = TRUE;
			}
			break;
		case '8':
			{
				strRun += "��";
				strRun += szScale[iIndex];
				bBegin = TRUE;
				bZeroArise = FALSE;
			}
			break;
		case '9':
			{
				strRun += "��";
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
	//������ֵĽ�β���㣬����㲻��������ֱ�ӹ�������
	int iPos = strRun.find("��", strRun.length()-2);
	if (iPos >= 0)
	{
		strRun = strRun.substr(0, strRun.length()-2);
	}
	//�ߵ���������ַ����ĳ��ȴ���0 ���ҷ����ַ���Ϊ�գ��϶���һ��0�����һ�����㡱
	if (strRun.length() <= 0 &&
		iLen > 0)
	{
		strRun += "��"; 
	}
	return strRun;
}


//��2017.00��ʽ�Ĵ�С����������ַ���ת���ɡ�����ʽ�������ַ���
string NumberFloatValueToChinese(char* strNumber)
{
	char *pos = NULL;
	string strRun = "", strHead = "", strEnd = "";
	if (strNumber == NULL)
	{
		return strRun;
	}
	//���Ȳ����ǲ�����С����
	pos = strstr(strNumber,".");
	if (pos != NULL)
	{
		//֤����С���㣬�ټ���м���
		char* posTemp = strstr(pos+1,".");
		if (posTemp != NULL)
		{
			return strRun;  //����С����֤����Ч
		}
	}
	//�ָ�Ϊ�����֣�С����ǰ С���� С����󣩲��ò�ͬ�ķ�ʽ���н���
	if (pos != NULL)
	{

		strHead = string(strNumber, pos-strNumber);
		strEnd = pos+1;
	}
	else 
	{
		strHead = strNumber;
	}

	strHead = NumberValueToChinese((char*)strHead.c_str()); //С����֮ǰ�İ��մ���ʮ��ǧ�򡱵���ֵ���͸�ʽ��
	strEnd = NumberStringToChinese((char*)strEnd.c_str());  //С����֮ǰ�İ������ֱ�������ֵ��ʽ��

	strRun  += strHead;
	if (strEnd.length() > 0)
	{
		strRun  += "��";
		strRun  += strEnd;
	}
	return strRun;
}


//�����硰YYYY-MM-DD HH:MM:SS��(���磺��2017-08-02 15:43:58��) ��ʽ��ʱ��ת���� "������ʱ��������ı���"�����磺������һ������¶���һʮ��ʱ��ʮ������ʮ���롱��
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
		//����ȷ��һ���ַ������ȣ�������Ҫ�󲻽���
		if (strlen(strNumber) != strlen("YYYY-MM-DD HH:MM:SS") )
		{
			break;
		}
		//������
		strtemp = strNumber;
		strtemp = strtemp.substr(0,4);
		strtempTwo =  NumberStringToChinese((char*)strtemp.c_str());
		if (strtempTwo.length() != 0)
		{
			strRun += strtempTwo;
			strRun += "��";
		}

		//������
		strtemp = strNumber;
		strtemp = strtemp.substr(5,2);
		strtempTwo =  NumberValueToChinese((char*)strtemp.c_str());
		if (strtempTwo.length() != 0)
		{
			strRun += strtempTwo;
			strRun += "��";
		}

		//������
		strtemp = strNumber;
		strtemp = strtemp.substr(8,2);
		strtempTwo =  NumberValueToChinese((char*)strtemp.c_str());
		if (strtempTwo.length() != 0)
		{
			strRun += strtempTwo;
			strRun += "��";
		}

		//����ʱ
		strtemp = strNumber;
		strtemp = strtemp.substr(11,2);
		strtempTwo =  NumberValueToChinese((char*)strtemp.c_str());
		if (strtempTwo.length() != 0)
		{
			strRun += strtempTwo;
			strRun += "ʱ";
		}

		//������
		strtemp = strNumber;
		strtemp = strtemp.substr(14,2);
		strtempTwo =  NumberValueToChinese((char*)strtemp.c_str());
		if (strtempTwo.length() != 0)
		{
			strRun += strtempTwo;
			strRun += "��";
		}

		//������
		strtemp = strNumber;
		strtemp = strtemp.substr(17,2);
		strtempTwo =  NumberValueToChinese((char*)strtemp.c_str());
		if (strtempTwo.length() != 0)
		{
			strRun += strtempTwo;
			strRun += "��";
		}
	} while (FALSE);
	return strRun;
}


//�����ַ���strReplace �в������е�strSrc�Ӵ������滻��strDes
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


//�����Ӵ��ڲ���
typedef struct _FINDWND_PAR_
{
    HWND  hwndGlobal;       //��󴫳��ı���Ĵ��ھ��
	BOOL  g_fShowHidden;    //�Ƿ�����������ش��� 
	POINT pt;               //������ڵ������
	DWORD dwArea;           //���ڵ������С����*��
}FINDWND_PAR, *PFINDWND_PAR;



static BOOL CALLBACK FindBestChildProc(HWND hwnd, LPARAM lParam)
{
	RECT  rect;
	DWORD a;
	PFINDWND_PAR pPar = (PFINDWND_PAR)lParam;

	GetWindowRect(hwnd, &rect);

	//���ڴ��ڷ�Χ��
	if(PtInRect(&rect, pPar->pt))
	{
		//�����Ӵ�������
		a = (rect.right-rect.left) * (rect.bottom-rect.top);

		//������Ӵ���С�ڵ�ǰ�ġ���ѡ����ڣ���ѡ��˴��� (���ڿɼ���������ʾ���ش���)
		if(a < pPar->dwArea && (IsWindowVisible(hwnd) || pPar->g_fShowHidden == TRUE))
		{
			pPar->dwArea = a;         //���������С
			pPar->hwndGlobal = hwnd;  //���¾��
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

	//ԭʼ���ڿ����Ѿ��Ƕ��㴰�ڣ����ԾͲ��Ӹ����ڿ�ʼ�ˡ�
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
	//�������ʾ���ش��ڣ��ҵ��Ǹ���һ��������ʾ�Ĵ���
	if(!fShowHidden)
	{
		while(hwndFocus && !IsWindowVisible(hwndFocus))
		{
			hwndFocus = GetParent(hwndFocus);
		}
	}
	return hwndFocus;
}



//�����ڵı߿�
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
	//���ڵ�λ��
	GetWindowRect(hWndPoint,&rectFrame);

	//���������ϵ���Ϊ��0��0���󣬴����ĸ��ǵ������
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


//����connect��ʱ
int SockSelectConnect( __in SOCKET s,
	__in_bcount(namelen) const struct sockaddr FAR *name,
	__in int namelen , int iTimeOutSecond)
{

	int iRun = SOCKET_ERROR;
	int iFlg = SOCKET_ERROR;

	unsigned long ul = 1;
	timeval tm;
	fd_set set;

	//��ʱʱ������
	tm.tv_sec  = iTimeOutSecond;
	tm.tv_usec = 0;

	iFlg = ioctlsocket(s, FIONBIO, &ul); //������Ϊ������ģʽ
	if (iFlg == SOCKET_ERROR)
	{
		return iRun;
	}

	//�ȳ�������һ��
	if( connect(s, (struct sockaddr *)name, namelen) == -1)
	{
		//����ʧ�ܣ����ó�ʱ��select�ȴ�ʱ��

		FD_ZERO(&set);
		FD_SET(s, &set);

		int error = -1;
		int len = sizeof(int);

		if( select(NULL, NULL, &set, NULL, &tm) > 0)
		{
			//������û�д�
			getsockopt(s, SOL_SOCKET, SO_ERROR, (char *)&error, /*(socklen_t *)*/&len);
			if(error == 0)  //������ӽ����ɹ����������ֵ����0
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
	iFlg = ioctlsocket(s, FIONBIO, &ul); //����Ϊ����ģʽ
	if (iFlg == SOCKET_ERROR)
	{
		iRun = SOCKET_ERROR;
	}
	return iRun;
}

DWORD GetTimeFromServer(BOOL bUserProxy, char *ip_addr, char* ip_Proxy, int port_Proxy)
{
	// ����ip_addr:��ʾָ����ʱ�������IP��ַ
	// ���أ���1900��1��1����0ʱ0��0������ĺ����� �� 0����ʾ��ȡʧ�ܣ�
	DWORD dwRun = 0;
	// Ĭ�ϵ�ʱ�������Ϊ"������ʱ����"
	if (ip_addr == NULL || strlen(ip_addr) == 0)
	{
		ip_addr = ("128.138.140.44");
	}
	//�Ƿ�ʹ�ô���,���ʹ�ã����Ҵ��������IP����ȷ���˳�
	if (bUserProxy &&
		(ip_Proxy == NULL))
	{
		return dwRun;
	}

	// ����WSADATA�ṹ�����
	WSADATA date;
	// ����汾����
	WORD w = MAKEWORD(2, 0);
	// ��ʼ���׽��ֿ�
	if ( ::WSAStartup(w, &date) != 0 )
	{
		return dwRun;
	}

	// ���������׽��־��
	SOCKET s = INVALID_SOCKET;

	//ʹ�ô���
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
	else  //��ʹ�ô���
	{
		// ���������Ϣ�������
		DWORD  m_serverTime;
		// ����TCP�׽���
		s = ::socket(AF_INET, SOCK_STREAM, 0);
		if (INVALID_SOCKET == s)
		{
			// �ر��׽��־��
			::closesocket(s);
			// �ͷ��׽��ֿ�
			::WSACleanup();
			return dwRun;
		}

		// �����׽��ֵ�ַ�ṹ
		sockaddr_in addr;

		// ��ʼ����ַ�ṹ
		addr.sin_family = AF_INET;
		addr.sin_port = htons(37);
		addr.sin_addr.S_un.S_addr = inet_addr(ip_addr);

		// ����
		if ( SockSelectConnect(s, (sockaddr*)&addr, sizeof(addr), 1) != 0 )
		{
			dwRun = ::WSAGetLastError();
			// �ر��׽��־��
			::closesocket(s);
			// �ͷ��׽��ֿ�
			::WSACleanup();
			return dwRun;
		}
	}

	int nNetTimeout = 2000;  //��ʱʱ�䣬��Ϊ���õĳ�ʱʱ����������������������ϲ����ܳ��ֽ������ݵ�ʱ����ڳ�ʱʱ�䣬���Կ϶����׽��ַ��������⣬����γ�����
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, ( char * )&nNetTimeout, sizeof( nNetTimeout ) );

	// ����
	if ( ::recv(s, (char *)&dwRun, 4, MSG_PEEK) <= 0 )
	{
		// �ر��׽��־��
		::closesocket(s);
		// �ͷ��׽��ֿ�
		::WSACleanup();
		return dwRun;
	}

	// �ر��׽��־��
	::closesocket(s);
	// �ͷ��׽��ֿ�
	::WSACleanup();
	// �����ֽ�˳��ת��Ϊ�����ֽ�˳��
	dwRun = ::ntohl(dwRun);

	// ���ؽ��յ�������
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
	   ��SYSTEMTIME�ṹ�趨Ϊ1900��1��1����ҹ��0ʱ����
	   �������SYSTEMTIME�ṹ���ݸ�SystemTimeToFileTime�����˽ṹת��ΪFILETIME�ṹ��
	   FILETIMEʵ����ֻ��������32λԪ��DWORDһ�����64λԪ��������
	   ������ʾ��1601��1��1��������Ϊ100���루nanosecond���ļ������ 
	 */     

	 LARGE_INTEGER li ;			       //64λ������
     li = * (LARGE_INTEGER *) &ftNew;
     li.QuadPart += (LONGLONG) 10000000 * serverTime; 
     ftNew = * (FILETIME *) &li;
     ::FileTimeToSystemTime (&ftNew, &stNew);

	 // ����ʱ�䣨ע�⣺���ﷵ�ص��Ǹ�������ʱ�䣬�뱱��ʱ�����8Сʱ��
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

//�Ƚ�����ʱ��
int CompareSystemTime(SYSTEMTIME s1, SYSTEMTIME s2)
{
	int iRun = 0;
	do 
	{
		//�Ƚ���
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
		//����ȣ��Ƚ���
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
		//����ȣ��Ƚ���
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
		//����ȣ��Ƚ�ʱ
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
		//ʱ��ȣ��ȽϷ���
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
		//������ȣ��Ƚ���
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
