#include "StdAfx.h"
#include "D_Log.h"



static FILE *open_file_retry(const std::wstring &logfile, const wchar_t *mode, int max_retry = 5)
{
  FILE *fp = NULL;
  errno_t  e;

  while ((e = _wfopen_s(&fp, logfile.c_str(), mode)) == EACCES && (--max_retry) >= 0) 
  { //opened by surbasic log runtine
    Sleep(100);
  }
  if (e != 0)
    return NULL;
  return fp;
}

int D_Log::debug_l = -1;
D_Lock m_hLogMutex;
D_Log::D_Log(void)
{
}

D_Log::~D_Log(void)
{
}
bool _trace(TCHAR *format, ...)
{
	TCHAR buffer[1000];
	va_list argptr;
	va_start(argptr, format);
	wvsprintf(buffer, format, argptr);
	va_end(argptr);
	OutputDebugString(buffer);
	return true;
}
	
void D_Log::WriteLogImpl (const char *module, ELogLevel eLogLevel, const char* format, va_list arglist)
{
	if (eLogLevel < GetLogLevel())
	{
		return;
	}

	m_hLogMutex.Lock ();
	int nErroLen = _vscprintf( format, arglist );
	char* pszError = new char[nErroLen + 1];
	vsprintf(pszError, format, arglist);
	
	D_Dir dir;

	CreateDirectoryPathW(dir.GetLogDir().c_str());

	FILE* fp = open_file_retry(dir.GetLogFileName(), L"a");
	if ( fp == NULL )
	{
		m_hLogMutex.UnLock ();
		delete [] pszError;
		return;
	}
	fseek ( fp, 0, SEEK_END );
	int size = ftell ( fp );
	rewind ( fp );
// 	if ( size == 0 )
// 	{
// 		char utf8[4] = { 0xef, 0xbb, 0xbf, 0x00 };
// 		fwrite ( utf8, 1, 3, fp );
// 	}
	time_t  nSysTime;
	struct tm nCurTm;
	time ( &nSysTime );
	_tzset ();
	localtime_s ( &nCurTm, &nSysTime );

	char szTime[21];
	memset ( szTime, 0, sizeof ( szTime ) );
	strftime ( szTime, sizeof ( szTime ), "%H:%M:%S", &nCurTm );
	
	size_t lastlen = nErroLen + strlen ( szTime ) + 35;
	char* pszLastError = new char [lastlen];
	memset ( pszLastError, 0, lastlen);

	if (strlen(module)>0 && module!= NULL)
	{
		sprintf_s ( pszLastError, lastlen, "¡¾%s¡¿ [%s] :%s\n",  szTime, module, pszError );
	}
	else
	{
		sprintf_s ( pszLastError, lastlen, "¡¾%s¡¿ : %s\n",  szTime,  pszError );
	}
	//sprintf_s ( pszLastError, lastlen, "[%s]  [%s] %s\n",  szTime, module, pszError );
	fwrite ( pszLastError, 1, strlen ( pszLastError ), fp );
	fclose ( fp );
	delete [] pszError;
	delete [] pszLastError;
	m_hLogMutex.UnLock ();
}
void D_Log::WriteLogImpl(const char *module, ELogLevel eLogLevel, const wchar_t* format, va_list arglist)
{
	if (eLogLevel < GetLogLevel())
	{
		return;
	}

	m_hLogMutex.Lock ();
	int nErroLen = _vscwprintf( format, arglist );
	wchar_t* pszError = new wchar_t[nErroLen + 1];
	vswprintf(pszError, nErroLen + 1, format, arglist);
	va_end(arglist);
	
	D_Dir dir;


	FILE* fp = open_file_retry(dir.GetLogFileName(), L"a");
	if ( fp == NULL )
	{
		delete [] pszError;
		m_hLogMutex.UnLock ();
		return;
	}
	fseek ( fp, 0, SEEK_END );
	int size = ftell ( fp );
	rewind ( fp );
	if ( size == 0 )
	{
		char utf8[4] = { 0xef, 0xbb, 0xbf, 0x00 };
		fwrite ( utf8, 1, 3, fp );
	}

	time_t  nSysTime;
	struct tm nCurTm;
	time ( &nSysTime );
	_tzset ();
	localtime_s ( &nCurTm, &nSysTime );


	char szTime[21];
	memset ( szTime, 0, sizeof ( szTime ) );
	strftime ( szTime, sizeof ( szTime ), "%H:%M:%S", &nCurTm );
	string strError = WCHAR2UTF8 ( pszError ).c_str ();
	size_t lastlen = strError.length () + strlen ( szTime ) + 35;
	char* pszLastError = new char [lastlen];
	memset ( pszLastError, 0, lastlen);
	if (strlen(module)>0)
	{
		sprintf_s ( pszLastError, lastlen, "¡¾%s¡¿ [%s] :%s\n",  szTime, module, strError.c_str () );
	}
	else
	{
		sprintf_s ( pszLastError, lastlen, "¡¾%s¡¿ : %s\n",  szTime,  strError.c_str () );
	}
	
	fwrite ( pszLastError, 1, strlen ( pszLastError ), fp );
	fclose ( fp );
	delete [] pszError;
	delete [] pszLastError;
	m_hLogMutex.UnLock ();
}
D_Log::ELogLevel D_Log::GetLogLevel()
{
  if (debug_l == -1)
  {
    debug_l = LOG_NORMAL;
    D_Dir dir;
//     D_Config config;
// 
//     D_XML xml;
//     if ( xml.xmlParser( dir.GetAnyConfigXmlExists().c_str(), D_XML::FILE_LOAD ) && xml.LoadConfig ( config ))
//     {
//       if (config.GetDebugLevel().length() != 0) 
//       {
//         debug_l = ELogLevel(atoi(config.GetDebugLevel().c_str()));
//       }
//     }

    return (ELogLevel)(debug_l%(LOG_LEVEL-1));
  }
  else
  {
    return (ELogLevel)(debug_l%(LOG_LEVEL-1));
  }
}
void D_Log::DeleteOldLog ()
{
    D_Dir dir;
	std::wstring strFindFilter = dir.GetLogDir() +  L"*.log";
	struct _wfinddata_t c_file;
	intptr_t hFile;

	if ( ( hFile = _wfindfirst ( strFindFilter.c_str(), &c_file ) ) == -1L )
	{
		return;
	}
	else
	{
		do 
		{
			if (IsNeedDelete(c_file.name))
			{
				wstring strLogName = dir.GetLogDir() + c_file.name;
				_wremove(strLogName.c_str());
			}

		} while ( _wfindnext ( hFile, &c_file ) == 0 );

		_findclose( hFile );
	}
}
BOOL D_Log::IsNeedDelete( const wchar_t* pszLogName )
{
	wstring strLogName(pszLogName);

	wstring strCurDate;
	int nCurYear;
	int nCurMonth;
	int iCurDate;
	wstring strDate;
	int nYear;
	int nMonth;
	int iDate;
	CTime time = CTime::GetCurrentTime();
	CString strCurTime = time.Format(_T("%Y%m%d"));

	strCurDate = GetWStr(strCurTime);
	nCurYear = _wtoi(strCurDate.substr(0, 4).c_str());
	nCurMonth = _wtoi(strCurDate.substr(4, 2).c_str());
	iCurDate  = _wtoi(strCurDate.substr(6,2).c_str());

	wstring::size_type index = strLogName.find(L".log");
	if ( string::npos != index )
	{
		strDate = strLogName.substr(0, index+1);

		if (L"" == strDate)
		{
			return FALSE;
		}

		nYear = _wtoi(strDate.substr(0, 4).c_str());
		nMonth = _wtoi(strDate.substr(5, 2).c_str());
		iDate  = _wtoi(strDate.substr(8,2).c_str());
	}//if ( string::npos != index )

	if ( (nYear>0) & (nMonth>0) & (iDate >0) )
	{
		CTime tNowDate(nCurYear, nCurMonth, iCurDate, 0, 0, 0);//today time
		CTime tFileDate(nYear, nMonth, iDate, 0, 0, 0);//file time
		CTimeSpan tSpan = tNowDate - tFileDate;       
		int64_t dlDiffDate = tSpan.GetTotalHours()/24;//È¡Ê±¼ä²î
		if (dlDiffDate>7)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

void D_Log::WriteLog ( ELogLevel eLogLevel, const char* format, ...)
{
	va_list arglist;
	va_start(arglist, format);
	WriteLogImpl("", eLogLevel, format, arglist);
	va_end(arglist);
}

void D_Log::WriteModuleMoLog ( const char* Module,ELogLevel eLogLevel, const char* format, ...)
{
	va_list arglist;
	va_start(arglist, format);
	WriteLogImpl(Module, eLogLevel, format, arglist);
	va_end(arglist);
}


void D_Log::WriteLog ( ELogLevel eLogLevel, const wchar_t* format, ... )
{
	va_list arglist;
	va_start(arglist, format);
	WriteLogImpl("", eLogLevel, format, arglist);
	va_end(arglist);
}

void D_Log::WriteClientLog ( ELogLevel eLogLevel, const char* format, ...)
{
	va_list arglist;
	va_start(arglist, format);
	WriteLogImpl("Client  ", eLogLevel, format, arglist);
	va_end(arglist);
}

void D_Log::WriteClientLog ( ELogLevel eLogLevel, const wchar_t* format, ... )
{
	va_list arglist;
	va_start(arglist, format);
	WriteLogImpl("Client  ", eLogLevel, format, arglist);
	va_end(arglist);
}
void D_Log::WriteSerivceLog ( ELogLevel eLogLevel, const char* format, ...)
{
	va_list arglist;
	va_start(arglist, format);
	WriteLogImpl("Serivce  ", eLogLevel, format, arglist);
	va_end(arglist);
}

void D_Log::WriteSerivceLog ( ELogLevel eLogLevel, const wchar_t* format, ... )
{
	va_list arglist;
	va_start(arglist, format);
	WriteLogImpl("Serivce  ", eLogLevel, format, arglist);
	va_end(arglist);
}
