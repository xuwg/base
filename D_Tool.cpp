#include "stdafx.h"
#include "D_Tool.h"

#include "D_StringUtils.h"
// #include <shlobj.h> 
// #include <sys/stat.h>
// #include <Psapi.h>
// #include <Iphlpapi.h>
//#include "md5_dgst.h"
//#include "sha.h"
// 
// #include <locale.h>
// #include <windows.h>
// #include <cassert>
// #include <stdio.h>
// #include "FileInfo.h"
// #include "File.h"

// CStringA ConvertUTCToLocalTime(CStringA _cstrTime)
// {
// 	TIME_ZONE_INFORMATION DEFAULT_TIME_ZONE_INFORMATION;
// 	//时区的信息
// 	GetTimeZoneInformation(&DEFAULT_TIME_ZONE_INFORMATION);
// 	//int izone = DEFAULT_TIME_ZONE_INFORMATION.Bias/ -60; //时区，如果是中国标准时间则得到8
// 	int   iYear,iMonth,iDate,iHour,iMin,iSec ,iWeek;   
// 	sscanf(_cstrTime,   "%4d-%02d-%02d %02d:%02d:%02d",   &iYear,   &iMonth,   &iDate,   &iHour,   &iMin,   &iSec);   
// 	if (iYear< 1970  ||iMonth < 1 ||iDate < 1 ||iHour < 0)
// 	{
// 		return _cstrTime;
// 	}
// 	
// 	if (iYear == 1970 && iMonth ==1 && iDate == 1 && iHour < 12)
// 	{
// 		return _cstrTime;
// 	}
// 	if (iYear> 3000  ||iMonth > 12 ||iDate > 31 )
// 	{
// 		return _cstrTime;
// 	}
// 	CTime   tTmp(iYear,iMonth,iDate,iHour,iMin,iSec);
// 	iWeek = tTmp.GetDayOfWeek();
// 	SYSTEMTIME sysTime = {0}; 
// 	sysTime.wYear = iYear;
// 	sysTime.wMonth= iMonth;
// 	sysTime.wDay  = iDate;
// 	sysTime.wHour = iHour;
// 	sysTime.wMinute = iMin;
// 	sysTime.wSecond = iSec;
// 	sysTime.wDayOfWeek = iWeek;
// 	//将UTC时间转换为中国时区的本地时间
// 	SystemTimeToTzSpecificLocalTime(&DEFAULT_TIME_ZONE_INFORMATION,&sysTime,&sysTime);
// 	CStringA cstrTime;
// 	cstrTime.Format("%4d-%02d-%02d %02d:%02d:%02d",sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute,sysTime.wSecond);
// 	return cstrTime;
// }
int DeleteDir ( const wchar_t* pszDir, bool bDelSelf )
{
	if ( pszDir == NULL
		|| wcslen ( pszDir ) == 0 )
		return -1;

	struct _wfinddata_t c_file;
    intptr_t hFile;
	wstring strDir = pszDir;
	size_t nLen = strDir.length ();
	if ( strDir.at ( nLen - 1 ) == '\\'
		|| strDir.at ( nLen - 1 ) == '//' )
	{
		strDir += L"*.*";
	}
	else
	{
		strDir += L"\\*.*";
	}
	if ( ( hFile = _wfindfirst ( strDir.c_str (), &c_file ) ) == -1L )
	{
		return  -1;
	}
    else
    {
      do 
	  {
		  if ( wcscmp ( c_file.name, L"." ) != 0 && wcscmp ( c_file.name, L".." ) != 0 )
		  { 
			   strDir = pszDir;

			   size_t nLen = strDir.length ();
			   if ( strDir.at ( nLen - 1 ) != '\\'
					&& strDir.at ( nLen - 1 ) != '//' )
			   {
					 strDir += L"\\";
			   }
			  			
			   strDir += c_file.name;
			   if ( c_file.attrib & _A_SUBDIR )			  
			   {
				   DeleteDir ( strDir.c_str () );
				   _wrmdir ( strDir.c_str () );
			   }
			   _wremove ( strDir.c_str () );   
		  }
      } while ( _wfindnext ( hFile, &c_file ) == 0 );
	  	 
      _findclose( hFile );
   }
	return  ( !bDelSelf ) ?_wrmdir ( pszDir ) : 1;
}
int DeleteDir ( const char* pszDir, bool bDelSelf )
{
	if ( pszDir == NULL 
		|| strlen ( pszDir ) == 0 ) 
		return -1;
	struct _finddata_t c_file;
    intptr_t hFile;
	string strDir = pszDir;
	size_t nLen = strDir.length ();
	if ( strDir.at ( nLen - 1 ) == '\\' 
		|| strDir.at ( nLen - 1 ) == '//')
	{
		strDir += "*.*";
	}
	else
	{
		strDir += "\\*.*";
	}
	if ( ( hFile = _findfirst ( strDir.c_str (), &c_file ) ) == -1L )
	{
		return  -1;
	}
    else
    {
      do 
	  {
		  if ( strcmp ( c_file.name, "." ) != 0 && strcmp ( c_file.name, ".." ) != 0 )
		  { 
			   strDir = pszDir;
			   size_t nLen = strDir.length ();
			   if ( strDir.at ( nLen - 1 ) != '\\'
					&& strDir.at ( nLen - 1 ) != '//' )
			   {
					 strDir += "\\";
			   }
			   strDir += c_file.name;
			   if ( c_file.attrib & _A_SUBDIR )			  
			   {
				   DeleteDir ( strDir.c_str () );
				   _rmdir ( strDir.c_str () );
			   }
			   remove ( strDir.c_str () );   
		  }
      } while ( _findnext ( hFile, &c_file ) == 0 );
	  	 
      _findclose( hFile );
   }
	return  ( !bDelSelf ) ?_rmdir ( pszDir ) : 1;
}
bool FolderExist ( const char* pszDir )
{
	if ( pszDir == NULL )
		return false;
	struct _wfinddata_t fileinfo;
    intptr_t hFile;
	bool bFolder = false;
	string strDirtmp = SlashLogical ( pszDir );			// change by gaoyu 20111220
	wstring strDir = UTF82WCHAR ( strDirtmp.c_str());
	if ( ( hFile = _wfindfirst ( strDir.c_str (), &fileinfo ) ) != -1 
		&& ( fileinfo.attrib & _A_SUBDIR ) )
	{
		bFolder = true;
    } 
    _findclose( hFile );	
	return  bFolder;
}


bool FileExists(const wchar_t *path)
{
  DWORD attr = ::GetFileAttributesW(path);
  if (attr == INVALID_FILE_ATTRIBUTES)
    return false;
  return (attr & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

bool FileExists(const char *path)
{
  DWORD attr = ::GetFileAttributesA(path);
  if (attr == INVALID_FILE_ATTRIBUTES)
    return false;
  return (attr & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

bool GetFileExtName ( const char* pszFullPath, char* pszExt )
{
	if ( NULL == pszFullPath || NULL == pszExt ) return false;
	char* temp = ( char* ) pszFullPath;
	while ( *temp!= '\0' )
	{
		*temp++;
	}
	
	while ( *temp != '.' )
	{
		*temp--;
	}
	*temp++;
	char* extTemp = pszExt;
	while ( *temp != '\0' )
	{
		*extTemp++ = tolower ( *temp++ );
	}
	if ( strcmp ( pszExt, "sep" ) != 0 )
		return false;
	return true;
}

std::wstring StrToupper(const std::wstring &_strValue)
{
	wstring strRet = L"";

	for(int i=0;i<_strValue.size();i++)
	{
		strRet = strRet + (wchar_t)toupper(_strValue.at(i));
	}

	return strRet;
}
const char* toLower ( const char* pszSrc )
{
	if ( NULL == pszSrc )
	{
		throw D_Exception ( D_ERROR_POINTERNULL);
	}
	char* temp =(char*) pszSrc;
	do
	{
		if ( *temp >= 'A' && *temp <= 'Z' )
		{
			*temp += ' ';
		}

	}while ( *temp++ != '\0' );
	return pszSrc;
}
const wchar_t* toLower ( const wchar_t* pszSrc )
{
	if ( NULL == pszSrc )
	{
		throw D_Exception ( D_ERROR_POINTERNULL);
	}
	wchar_t* temp =(wchar_t*) pszSrc;
	do
	{
		if ( *temp >= 'A' && *temp <= 'Z' )
		{
			*temp += ' ';
		}

	}while ( *temp++ != '\0' );
	return pszSrc;
}
int UTF8ToGB( const char* pszSrc, char* pszDest )
{
	int i = MultiByteToWideChar(CP_UTF8, 0, pszSrc, -1, NULL, 0);
	wchar_t* strWSrc = new wchar_t[i];
	MultiByteToWideChar(CP_UTF8, 0, pszSrc, -1, strWSrc, i);
	int nLen = WideCharToMultiByte(CP_ACP, 0, strWSrc, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, strWSrc, -1, pszDest, nLen, NULL, NULL);
	delete [] strWSrc;
	return nLen;
}
int HexToDec ( const char * pszHex )
{
	int num = 0;
	int n = 0;
	int nLen = ( int )strlen ( pszHex );
	for ( int i = 0; i < nLen && nLen > 0 ; ++i )
	{
		if ( pszHex[i] >= '0' && pszHex[i] <= '9' )
		{
			n = pszHex[i] - '0';
		}
		if ( pszHex[i] >= 'A' && pszHex[i] <= 'F' )
		{
			n = pszHex[i] - '7';
		}
		if ( pszHex[i] >= 'a' && pszHex[i] <= 'f' )
		{
			n = pszHex[i] - 32- '7';
		}
		num +=  n * ( int ) pow ( 16.0, nLen - 1 - i );
	}
	return num;
}
bool GetSVDDir ( wstring& strSvgDir )
{
	wchar_t szPath[MAX_PATH] ={0};
	strSvgDir = L"";
	if ( !SHGetSpecialFolderPathW ( NULL, szPath, 0x0023, true ) )
	{
		return false;
	}
	strSvgDir = szPath;
	return true;
}
bool GetExeDir ( wstring& strExeDir )
{
	wchar_t szEXE[MAX_PATH] = {0};
	strExeDir = L"";
	DWORD nRet = GetModuleFileNameW ( NULL, szEXE, MAX_PATH );
	if ( nRet == 0 ) return false;
	wstring strExe = szEXE;
	size_t nPos = strExe.find_last_of  ( L"\\");
	strExeDir = strExe.substr ( 0, nPos );
	return true;
}
bool IsExistFile ( const char * pszPath )
{
	return ( pszPath != NULL && _access( pszPath, 0 ) == 0 ) ? true : false;
}
bool IsExistFile ( const wchar_t * pszPath )
{
	return ( pszPath != NULL && _waccess( pszPath, 0 ) == 0 ) ? true : false;
}

const string GetGuid ()
{
    GUID guid;	
	char szGuid [36]= {0};
	string strGuid ="";
	if ( SUCCEEDED (::CoCreateGuid ( &guid ) ) )
	{		
		sprintf_s ( szGuid, 36, "%X%X%X%X%X%X%X%X%X%X%X", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0],
				 guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7] );	
		strGuid = szGuid;		
	}
	else
	{	
		return strGuid;
	}
	return strGuid;
}
int GetFileSize( const char *path )
{
    int iresult;
    struct _stat buf;
	iresult = ::_stat( path, &buf );
    if(iresult == 0)
    {
        return buf.st_size;
    }
    return 0;
}
int GetFileSize( const wchar_t *path )
{
    int iresult;
    struct _stat buf;
	iresult = ::_wstat( path, &buf );
    if(iresult == 0)
    {
        return buf.st_size;
    }
    return 0;
}
int64_t GetFileSizeEx( const wchar_t *path )
{
	int iresult;
	struct __stat64 buf;
	iresult = ::_wstat64( path, &buf );
	if(iresult == 0)
	{
		return buf.st_size;
	}
	return 0;
}
char dec2hexChar(short int n) {
 if ( 0 <= n && n <= 9 ) {
  return char( short('0') + n );
 } else if ( 10 <= n && n <= 15 ) {
  return char( short('A') + n - 10 );
 } else {
  return char(0);
 }
}

short int hexChar2dec(char c) {
 if ( '0'<=c && c<='9' ) {
  return short(c-'0');
 } else if ( 'a'<=c && c<='f' ) {
  return ( short(c-'a') + 10 );
 } else if ( 'A'<=c && c<='F' ) {
  return ( short(c-'A') + 10 );
 } else {
  return -1;
 }
}

string escapeURL(const string &URL)
{
 string result = "";
 for ( unsigned int i=0; i<URL.size(); i++ ) {
  char c = URL[i];
  // check whether is unreserved character
  // see http://tools.ietf.org/html/rfc3986#section-2.3
  if ( 
   ( '0'<=c && c<='9' ) ||
   ( 'a'<=c && c<='z' ) ||
   ( 'A'<=c && c<='Z' ) ||
   c=='-' || c=='.' || c == '_' || c == '~' 
   ) {
   result += c;
  } else {
   int j = (short int)c;
   if ( j < 0 ) {
    j += 256;
   }
   int i1, i0;
   i1 = j / 16;
   i0 = j - i1*16;
   result += '%';
   result += dec2hexChar(i1);
   result += dec2hexChar(i0); 
  }
 }
 return result;
}
wstring GBK2WCHAR ( const char* data )
{
	int len = MultiByteToWideChar(CP_ACP, 0, data, -1, NULL, 0);
	wchar_t* szBuf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, data, -1, szBuf, len );
	wstring str = szBuf;
	delete []szBuf;
	return str;
}
string GBK2UTF8 ( const char* data )
{
	int len = MultiByteToWideChar(CP_ACP, 0, data, -1, NULL, 0);
	wchar_t* szBuf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, data, -1, szBuf, len );
	
	int nlen = WideCharToMultiByte(CP_UTF8, 0, szBuf, -1, NULL, 0, NULL, NULL);
	std::vector<char> utf8(nlen );   
    WideCharToMultiByte(CP_UTF8, 0, szBuf, -1, &utf8[0], nlen, NULL, NULL);
	delete []szBuf;
	return &utf8[0];
}
string	WCHAR2UTF8 ( const wchar_t* data )
{	
	if ( data == NULL )
		return "";
	int nlen = WideCharToMultiByte(CP_UTF8, 0, data, -1, NULL, 0, NULL, NULL);
	std::vector<char> utf8(nlen );   
    WideCharToMultiByte(CP_UTF8, 0, data, -1, &utf8[0], nlen, NULL, NULL);
	return &utf8[0];
}
wstring	UTF82WCHAR ( const char* data )
{	
	if ( data == NULL )
		return L"";
	int len = MultiByteToWideChar(CP_UTF8, 0, data, -1, NULL, 0);
	wchar_t* szBuf = new wchar_t[len];
	MultiByteToWideChar(CP_UTF8, 0, data, -1, szBuf, len );	
	wstring str = szBuf;
	delete []szBuf;
	return str;
}
// Add by zhaojunjiang 2016-6-8 -start-----
//支持ansi转 utf8
WCHAR* AnsiToUTF8( const string& strIn )
{
	WCHAR* strSrc    = NULL;
	int iRet = MultiByteToWideChar(CP_UTF8, 0, strIn.c_str(), -1, NULL, 0);
	strSrc = new WCHAR[iRet+1];
	MultiByteToWideChar(CP_UTF8, 0, strIn.c_str(), -1, strSrc, iRet);

	return strSrc;
}
WCHAR* AnsiToUTF8( const char* data  )
{
	WCHAR* strSrc    = NULL;
	int iRet = MultiByteToWideChar(CP_UTF8, 0, data, -1, NULL, 0);
	strSrc = new WCHAR[iRet+1];
	MultiByteToWideChar(CP_UTF8, 0, data, -1, strSrc, iRet);

	return strSrc;
}
//------end------------------
string	UTF82CHAR ( const char* data )
{
	return GetString ( UTF82WCHAR ( data ).c_str());
}
// 网络传输中中文以utf-8编码，%字节编码格式传输
// encode :utf-8 表示先将GBK转为utf-8在进行网络编码处理
//		   gbk   表示已为utf-8 直接进行网络编码处理
string URLEncode ( const char* data, const char* encode )
{
	if ( strcmp ( encode, "utf-8" ) == 0 )
	{
		string strRet = GBK2UTF8 ( data );
		return escapeURL ( strRet );
	}
	if ( strcmp ( encode, "gbk" ) == 0 )
	{
		return escapeURL ( data ); 
	}
	return escapeURL ( data ); 
}
// string GetClientVersion ()//引用Version.lib
// {
// 	string strVersion;
// 	TCHAR szEXE[MAX_PATH] = {0};
// 	DWORD nRet = GetModuleFileName ( NULL, szEXE, MAX_PATH );
// 	
// 	int  iVerInfoSize;
// 	char  *pBuf;   
// 	
// 	VS_FIXEDFILEINFO  *pVsInfo;   
// 	unsigned int iFileInfoSize = sizeof ( VS_FIXEDFILEINFO );   
// 	      
// 	iVerInfoSize = GetFileVersionInfoSize ( szEXE, NULL );    
// 	char szBuf[20] = {0};
// 	if ( iVerInfoSize != 0 )   
// 	{      
// 		pBuf = new char[iVerInfoSize];   
// 		if ( GetFileVersionInfo(szEXE, 0,iVerInfoSize, pBuf ) )      
// 		{      
// 			if(VerQueryValue(pBuf, _T ("\\"), (void   **)&pVsInfo,&iFileInfoSize))      
// 			{     
// 				sprintf ( szBuf, "%d.%d.%d.%d",
// 						 HIWORD(pVsInfo->dwFileVersionMS),
// 						 LOWORD(pVsInfo->dwFileVersionMS),
// 						 HIWORD(pVsInfo->dwFileVersionLS),
// 						 LOWORD(pVsInfo->dwFileVersionLS) );
// 				strVersion = szBuf;
// 			}      
// 		}      
// 		delete []pBuf;      
// 	}
// 	return strVersion;
// }
string SlashDir ( const char* pszDir )
{
	string str = pszDir;
	if ( str.empty () )
		return "";
	size_t size = str.length ();
	if ( str[size - 1] == '\\' 
		|| str[size - 1] == '/')
	{
		str = str.substr ( 0, size - 1 );
	}
	if ( str.length () == 2 )
	{
		if ( str.at( 1 ) == ':'  )
		{
			str = str.substr ( 0, 1 );
		}
	}
	return str;
}
wstring SlashDirW ( const wchar_t* pszDir )
{
	wstring str = pszDir;
	if ( str.empty () )
		return L"";
	size_t size = str.length ();
	if ( str[size - 1] == '\\' 
		|| str[size - 1] == '/')
	{
		str = str.substr ( 0, size - 1 );
	}
	if ( str.length () == 2 )
	{
		if ( str.at( 1 ) == ':'  )
		{
			str = str.substr ( 0, 1 );
		}
	}
	return str;
}
string splitDirName ( const char* pszDir )
{
	string strDir = SlashDir ( pszDir );
	if ( strDir.empty () )
		return "";
	size_t size = strDir.length ();
	size_t pos = strDir.find_last_of ( "\\" );
	if ( pos == -1 )
	{
		pos = strDir.find_last_of ( "/" );
	}
	if ( pos == -1 )
		return strDir;
	strDir = strDir.substr ( pos + 1, size - pos - 1);
	return strDir;
}
wstring splitDirNameW ( const wchar_t* pszDir )
{
	wstring strDir = SlashDirW ( pszDir );
	if ( strDir.empty () )
		return L"";
	size_t size = strDir.length ();
	size_t pos = strDir.find_last_of ( L"\\" );
	if ( pos == -1 )
	{
		pos = strDir.find_last_of ( L"/" );
	}
	if ( pos == -1 )
		return strDir;
	strDir = strDir.substr ( pos + 1, size - pos - 1);
	return strDir;
}
wstring SlashLogicalW ( const wchar_t* pszDir )
{
	wstring str = pszDir;
	if ( str.empty () )
		return L"";
	size_t size = str.length ();
	if ( str[size - 1] == '\\' 
		|| str[size - 1] == '/')
	{
		str = str.substr ( 0, size - 1 );
	}
	return str;
}
string SlashLogical ( const char* pszDir )
{
	string str = pszDir;
	if ( str.empty () )
		return "";
	size_t size = str.length ();
	if ( str[size - 1] == '\\' 
		|| str[size - 1] == '/')
	{
		str = str.substr ( 0, size - 1 );
	}
	return str;
}
string splitDirNameEx ( const char* pszDir )
{
	string strDir = SlashLogical ( pszDir );
	if ( strDir.empty () )
		return "";
	size_t size = strDir.length ();
	size_t pos = strDir.find_last_of ( "\\" );
	if ( pos == -1 )
	{
		pos = strDir.find_last_of ( "/" );
	}
	if ( pos == -1 )
		return strDir;
	strDir = strDir.substr ( pos + 1, size - pos - 1);
	return strDir;
}
wstring splitDirNameExW ( const wchar_t* pszDir )
{
	wstring strDir = SlashLogicalW ( pszDir );
	if ( strDir.empty () )
		return L"";
	size_t size = strDir.length ();
	size_t pos = strDir.find_last_of ( L"\\" );
	if ( pos == -1 )
	{
		pos = strDir.find_last_of ( L"/" );
	}
	if ( pos == -1 )
		return strDir;
	strDir = strDir.substr ( pos + 1, size - pos - 1);
	return strDir;
}
string GetString ( const wchar_t* lpctstr )
{
	if ( lpctstr == NULL)
		return "";
	int nlen = WideCharToMultiByte(CP_ACP, 0, lpctstr, -1, NULL, 0, NULL, NULL);
	char* szBuf = new char[nlen];
	memset ( szBuf, 0, nlen );
	WideCharToMultiByte(CP_ACP, 0, lpctstr, -1, szBuf, nlen, NULL, NULL);
	szBuf[nlen -1] = 0;
	string strRet = szBuf;
	delete []szBuf;
	return strRet;
}
string GetFileTime ( FILETIME filetime )
{
	SYSTEMTIME SystemTime;
	FileTimeToSystemTime ( &filetime, &SystemTime );
	TIME_ZONE_INFORMATION zinfo;
	GetTimeZoneInformation(&zinfo);
	SystemTimeToTzSpecificLocalTime(&zinfo, &SystemTime, &SystemTime);

	char szData[4] = {0};
	char szMonth[4] = {0};
	char szHour[4]= {0};
	char szMinute[4] = {0};
	char szSecond[4] = {0};

	if ( SystemTime.wMonth >= 10 )
		sprintf ( szMonth, "%d", SystemTime.wMonth );
	else
		sprintf ( szMonth, "0%d", SystemTime.wMonth );

	if ( SystemTime.wDay >= 10 )
		sprintf ( szData, "%d", SystemTime.wDay );
	else
		sprintf ( szData, "0%d", SystemTime.wDay );

	WORD wHour = SystemTime.wHour;
	if ( wHour >= 10 )
		sprintf ( szHour, "%d", wHour );
	else
		sprintf ( szHour, "0%d", wHour);

	if ( SystemTime.wMinute >= 10 )
		sprintf ( szMinute, "%d", SystemTime.wMinute );
	else
		sprintf ( szMinute, "0%d", SystemTime.wMinute );

	if ( SystemTime.wSecond >= 10 )
		sprintf ( szSecond, "%d", SystemTime.wSecond );
	else
		sprintf ( szSecond, "0%d", SystemTime.wSecond );

	char szTime[100] = {0};
	sprintf_s ( szTime, 100, "%d-%s-%s %s:%s:%s", SystemTime.wYear, szMonth, szData, szHour, szMinute, szSecond);
	return szTime;
}

bool ConvertCommunicate ( LPCTSTR szName, DWORD dwNumberOfBytesToMap, LPCTSTR szMsg )
{
	  HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE,   
										 NULL,   
										 PAGE_READWRITE,   
										 0,   
										dwNumberOfBytesToMap,   
										szName );   
	 
	  if (hMapFile == NULL)  
	  {  		  
		  return false;
	  }
	  LPCTSTR pBuf = (LPCTSTR) MapViewOfFile( hMapFile,   
								     FILE_MAP_ALL_ACCESS,  
								     0,   
								     0,   
								     dwNumberOfBytesToMap );   
	 
	  if (pBuf == NULL)  
	  {  
		 return false;
	  }
	    
	  CopyMemory((PVOID)pBuf, szMsg, lstrlen(szMsg));
	 
	  UnmapViewOfFile(pBuf);  
	  CloseHandle(hMapFile);
	  return true;
}
// 获得文件摘要
//string GetCrcEx ( LPCTSTR lpPath, LPCTSTR lpEventName ) 
// string GetCrcEx ( const wchar_t* lpPath, LPCTSTR lpEventName ) 
// {
// #ifndef MD5_SHA1
// 	if ( lpPath == NULL )
// 		return "0";
// 	CCrc32Dynamic pobCrc32Dynamic;
// 	pobCrc32Dynamic.Init ( lpEventName );
// 	DWORD dwRet;
// 	DWORD nn = pobCrc32Dynamic.FileCrc32Assembly ( lpPath, dwRet );
// 	pobCrc32Dynamic.Free();
// 	char szBuf [100] = {0};
// 	sprintf ( szBuf, "%u", dwRet );
// 	return szBuf;
// #else
// 	string strMD5_SHA1;
// 	GetMD5_SHA1(lpPath, strMD5_SHA1 );
// 	return strMD5_SHA1;
// #endif
// }
// //string GetCrc ( LPCTSTR lpPath ) 
// string GetCrc ( const wchar_t* lpPath ) 
// {
// #ifndef MD5_SHA1
// 	if ( lpPath == NULL )
// 		return "0";
// 	CCrc32Dynamic pobCrc32Dynamic;
// 	pobCrc32Dynamic.Init ();
// 	DWORD dwRet;
// 	DWORD nn = pobCrc32Dynamic.FileCrc32Assembly ( lpPath, dwRet );
// 	pobCrc32Dynamic.Free();
// 	char szBuf [100] = {0};
// 	sprintf ( szBuf, "%u", dwRet );
// 	return szBuf;
// #else
// string strMD5_SHA1;
// GetMD5_SHA1(lpPath, strMD5_SHA1 );
// return strMD5_SHA1;
// #endif
// }
DWORD TerminateProcessEx( LPCTSTR strProcessName )
{
    DWORD aProcesses[1024], cbNeeded, cbMNeeded;
    HMODULE hMods[1024];
    HANDLE hProcess;
    TCHAR szProcessName[MAX_PATH];

    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )  
		return 0;
    for(int i=0; i< (int) (cbNeeded / sizeof(DWORD)); i++)
    {      
        hProcess = OpenProcess ( PROCESS_ALL_ACCESS, FALSE, aProcesses[i]);
        EnumProcessModules( hProcess, hMods, sizeof(hMods), &cbMNeeded );

        GetModuleFileNameEx( hProcess, hMods[0], szProcessName, sizeof(szProcessName) );
		BOOL bRet;
        if( _tcsstr(szProcessName, strProcessName) && hProcess != NULL )
        {
           	DWORD dwRet;
			GetExitCodeProcess ( hProcess, &dwRet );
			bRet = TerminateProcess ( hProcess, dwRet );
			if(!bRet)
			{
				_tprintf(_T("TerminateProcess error = %d"), GetLastError());
				Sleep(10);
				bRet = TerminateProcess( hProcess, dwRet );
			}
        }
    }
    return 0;
}
BOOL GetDrive ( LPCTSTR Logical )
{
	if ( lstrlen ( Logical ) > 3 )
		return false;
	UINT nDrive = GetDriveType ( Logical );
	if ( nDrive != DRIVE_NO_ROOT_DIR && nDrive != DRIVE_UNKNOWN )
		return TRUE;;
	return FALSE;
}
BOOL GetDriveA ( const char* Logical )
{
	UINT nDrive = GetDriveTypeA ( Logical );
	if ( nDrive != DRIVE_NO_ROOT_DIR && nDrive != DRIVE_UNKNOWN )
		return TRUE;
	return FALSE;
}
typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

LPFN_ISWOW64PROCESS fnIsWow64Process;

BOOL IsWow64()
{
    BOOL bIsWow64 = FALSE;

    fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
        GetModuleHandle(TEXT("kernel32")),"IsWow64Process");
  
    if (NULL != fnIsWow64Process)
    {
        if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
        {
           
        }
    }
    return bIsWow64;
}
int IsFolderOrFile(LPCTSTR strPath)
{
    WIN32_FIND_DATA wfd;
    BOOL rValue = FALSE;
    HANDLE hFind = FindFirstFile(strPath, &wfd);
	int nRet = -1;
    if ( ( hFind != INVALID_HANDLE_VALUE ) )
	{
        if (wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		{
			nRet = 1;
		}
		else
		{
			 nRet = 0;
		}
	}
    FindClose(hFind);
    return nRet;
}

int exception_access_violation_filter(LPEXCEPTION_POINTERS p_exinfo)  
{  
	if(p_exinfo->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)  
	{  
		return 1;  
	} 
	return 0;
}

// BOOL GetMacAddress ( char * pszMax )  
// {
//   if ( pszMax == NULL )
//     return FALSE;
//   pszMax[0] = '\0';
// 
//   PIP_ADAPTER_INFO pAdapterInfo = NULL;
//   DWORD AdapterInfoSize = 0;
//   DWORD dwRet = GetAdaptersInfo ( NULL, &AdapterInfoSize );
//   if ( dwRet != 0 && dwRet !=  ERROR_BUFFER_OVERFLOW )
//   {
//     return  FALSE;
//   }
//   pAdapterInfo = ( PIP_ADAPTER_INFO ) GlobalAlloc ( GPTR, AdapterInfoSize );
//   if ( pAdapterInfo == NULL)
//   {
//     return FALSE;
//   }
// 
//   PIP_ADAPTER_INFO pNext = pAdapterInfo;
//   if ( GetAdaptersInfo ( pNext, &AdapterInfoSize ) != ERROR_SUCCESS )
//   {
//     GlobalFree ( pAdapterInfo );
//     return FALSE;
//   }
// 
//   PIP_ADAPTER_INFO pTemp = pNext;
//   while ( pTemp != NULL && pTemp->Type != MIB_IF_TYPE_ETHERNET)
//   {
//     pTemp = pTemp->Next;
//   }
//   BYTE addr1, addr2, addr3, addr4, addr5, addr6;
//   if ( pTemp == NULL )
//   {
//     addr1 = pNext->Address[0];
//     addr2 = pNext->Address[1];
//     addr3 = pNext->Address[2];
//     addr4 = pNext->Address[3];
//     addr5 = pNext->Address[4];
//     addr6 = pNext->Address[5];
//   }
//   else
//   {
//     addr1 = pTemp->Address[0];
//     addr2 = pTemp->Address[1];
//     addr3 = pTemp->Address[2];
//     addr4 = pTemp->Address[3];
//     addr5 = pTemp->Address[4];
//     addr6 = pTemp->Address[5];
//   }
// 
// 
//   sprintf ( pszMax, "%02X-%02X-%02X-%02X-%02X-%02X",
//     addr1,
//     addr2,
//     addr3,
//     addr4,
//     addr5,
//     addr6 );  
// 
//   GlobalFree(pAdapterInfo);
//   return TRUE;
// }

// BOOL GetHostName ( wstring& strHostName )
// {
// 	DWORD  dwSize = 50;
// 	LPTSTR pszHostName = new TCHAR[dwSize];
// 	memset ( pszHostName, 0, dwSize );
// 	BOOL bRet = GetComputerNameEx( ComputerNameDnsHostname, pszHostName, &dwSize );
// 
// 	if ( !bRet )
// 	{
// 		DWORD dwRet = GetLastError ();
// 		delete []pszHostName;
// 		pszHostName = NULL;
// 		if ( dwRet == ERROR_MORE_DATA )
// 		{		
// 			pszHostName = new TCHAR[dwSize+1];
// 			BOOL bRet = GetComputerNameEx( ComputerNameDnsHostname, pszHostName, &dwSize );
// 			if ( pszHostName != NULL )
// 			{
// 				strHostName = pszHostName;
// 				delete []pszHostName;
// 				pszHostName = NULL;
// 			}		
// 		}
// 		return bRet;
// 	}
// 	strHostName = pszHostName;
// 	if ( pszHostName != NULL )
// 	{
// 		delete pszHostName;
// 		pszHostName = NULL;
// 	}
// 	return bRet;
// }

// 判断文件本地是否存在
// BOOL IsLocalFile( const wstring& wsFilePath ,const string &strFileCrc, const string &strFileSize )
// {
// 	// GetFileSize
// #ifdef MD5_SHA1
// 	int64_t tp;
// 	if ((tp = _atoi64(strFileSize.c_str())) < 0){tp =0;}
// 	char szBuf[256] = {0};
// 	_i64toa(tp,(char *)szBuf,16);
// #endif //MD5_SHA1
// 	if(IsExistFile( wsFilePath.c_str() ) 
// #ifndef MD5_SHA1
// 		&& GetFileSize(wsFilePath.c_str()) == atoi(strFileSize.c_str())
// 		&& !GetCrc(wsFilePath.c_str() ).compare(strFileCrc))
// #else
// 		&& !(GetCrc(wsFilePath.c_str() )+szBuf).compare(strFileCrc))
// #endif
// 
// 	{
// 		return TRUE;
// 	}
// 
// 	return FALSE;
// }
string GetCurTime ()
{
	char szTime  [50] = {0};
	time_t start;
  	time_t starttime = time ( &start );
	struct tm starttm;
	_tzset ();
	localtime_s ( &starttm, &starttime );
	strftime ( szTime, sizeof ( szTime ), "%Y-%m-%d %H:%M:%S", &starttm );
	//strftime ( szTime, sizeof ( szTime ), "%m-%d-%Y %H:%M:%S", &starttm );
	return szTime;
}
string GetCurTimeEx ()
{
	char szData[4] = {0};
	char szMonth[4] = {0};
	char szHour[4]= {0};
	char szMinute[4] = {0};
	char szSecond[4] = {0};
	SYSTEMTIME SystemTime;
	GetSystemTime ( &SystemTime );

	if ( SystemTime.wMonth >= 10 )
		sprintf ( szMonth, "%d", SystemTime.wMonth );
	else
		sprintf ( szMonth, "0%d", SystemTime.wMonth );

	if ( SystemTime.wDay >= 10 )
		sprintf ( szData, "%d", SystemTime.wDay );
	else
		sprintf ( szData, "0%d", SystemTime.wDay );

	WORD wHour = SystemTime.wHour;
	if ( wHour >= 10 )
		sprintf ( szHour, "%d", wHour );
	else
		sprintf ( szHour, "0%d", wHour);

	if ( SystemTime.wMinute >= 10 )
		sprintf ( szMinute, "%d", SystemTime.wMinute );
	else
		sprintf ( szMinute, "0%d", SystemTime.wMinute );

	if ( SystemTime.wSecond >= 10 )
		sprintf ( szSecond, "%d", SystemTime.wSecond );
	else
		sprintf ( szSecond, "0%d", SystemTime.wSecond );

	char szTime[100] = {0};
	sprintf_s ( szTime, 100, "%d-%s-%s %s:%s:%s", SystemTime.wYear, szMonth, szData, szHour, szMinute, szSecond);
	return szTime;
}
wstring GetWStr ( const char* data )
{
	return GBK2WCHAR(data);
}
wstring GetWStr ( const wchar_t* data )
{
	return wstring(data);
}
string GetCStr ( const char* data )
{
	return string(data);
}
string GetCStr ( const wchar_t* data )
{
	int nlen = WideCharToMultiByte(CP_ACP, 0, data, -1, NULL, 0, NULL, NULL);
	char* szBuf = new char[nlen];
	memset ( szBuf, 0, nlen );
	WideCharToMultiByte(CP_ACP, 0, data, -1, szBuf, nlen, NULL, NULL);
	string strRet = szBuf;
	delete []szBuf;
	return strRet;
}

// int GetMD5_SHA1( const wchar_t* FilePath, string& strMD5_SHA1)
// {
// 	if ( FilePath == NULL )
// 		return -1;
// 	if (MD5_CAL(FilePath, strMD5_SHA1))
// 	{
// 		return -1;
// 	}
// 
// 	if (SHA1_CAL(FilePath, strMD5_SHA1))
// 	{
// 		return -1;
// 	}
// 
// 	return 0;
// }

// int MD5_CAL(const wchar_t *FilePath,  string& strMD5, bool* bExit)
// {
// 	char out[3] = {0};
// 	unsigned char hash[16] = {0};
// 	char newMD5[32 + 1] = {0};
// 	int ret = 0;
// 	DWORD start = GetTickCount();
// 	MD5_CTX s;
// 	int i;
// 	FILE* from_fd;
// 	int bytes_read;
// 	char buffer[1024];
// 
// 	if (NULL == FilePath) 
// 	{
// 		return -1;
// 	}
// 
// 	if ((from_fd=_wfopen(FilePath, L"rb"))==NULL) 
// 	{
// 		return -1;
// 	}
// 
// 	MD5_Init(&s);
// 
// 	while ((bytes_read=(int)fread(buffer, sizeof(char), 1024, from_fd))) 
// 	{
// 		if(bExit && *bExit)
// 		{
// 			fclose(from_fd);
// 			return -2;
// 		}
// 		if (ferror(from_fd))
// 		{
// 			ret = -1;
// 			break;
// 		}
// 		else 
// 		{
// 			MD5_Update(&s, buffer, bytes_read);
// 		}
// 	}
// 	fclose(from_fd);
// 	MD5_Final(hash, &s);
// 
// 	for(int i=0; i<16; ++i)
// 	{
// 		//printf ("%.2x", (int)hash[i]);
// 		sprintf ( out,"%02x", ( int )hash[i] );
// 		memcpy ( newMD5 + 2 * i, out, 2 );
// 	}
// 
// 	strMD5.append(newMD5);
// 	return ret;
// }

// int SHA1_CAL(const wchar_t *FilePath, string& strSHA1)
// {
// 	char out[3] = {0};
// 	unsigned char hash[20] = {0};
// 	char newSHA1[40 + 1] = {0};
// 	int ret = 0;
// 	SHA_CTX s;
// 	int i;
// 	FILE* from_fd;
// 	int bytes_read;
// 	char buffer[1024];
// 
// 	if (NULL == FilePath) 
// 	{
// 		return -1;
// 	}
// 
// 	if ((from_fd=_wfopen(FilePath, L"rb"))==NULL) 
// 	{
// 		return -1;
// 	}
// 
// 	SHA1_Init(&s);
// 
// 	while ((bytes_read=(int)fread(buffer, sizeof(char), 1024, from_fd))) 
// 	{
// 		if (ferror(from_fd))
// 		{
// 			ret = -1;
// 			break;
// 		}
// 		else 
// 		{
// 			SHA1_Update(&s, buffer, bytes_read);
// 		}
// 	}
// 
// 	fclose(from_fd);
// 	SHA1_Final(hash, &s);
// 
// 	for(int i=0; i<20; ++i)
// 	{
// 		//	printf ("%.2x", (int)hash[i]);
// 		sprintf ( out,"%02x", ( int )hash[i] );
// 		memcpy ( newSHA1 + 2 * i, out, 2 );
// 	}
// 	strSHA1.append(newSHA1);
// 	return ret;
// }

uint64_t filetime2int64(const FILETIME & ac_FileTime)													// add by gaoyu 20160509 convert filetime to int64
{
	ULARGE_INTEGER    lv_Large ;
	lv_Large.LowPart  = ac_FileTime.dwLowDateTime   ;
	lv_Large.HighPart = ac_FileTime.dwHighDateTime  ;
	return (lv_Large.QuadPart = lv_Large.QuadPart / 10000000ULL - 11644473600ULL );
}

void GetSystemLanguage(std::string& strLanguage)
{
	LCID lcid = GetSystemDefaultLCID();
	WORD PriLan = PRIMARYLANGID(lcid);
	WORD SubLan = SUBLANGID(lcid);

	if (LANG_ENGLISH == PriLan)
	{
		strLanguage = "en";
	}
	else if (LANG_CHINESE == PriLan && SUBLANG_CHINESE_SIMPLIFIED == SubLan)
	{
		strLanguage = "chs";
	}
	else
	{
		strLanguage = "en";
	}
}

// BOOL GetLanguageFromReg(std::string& strLanguage)
// {
//   if (Registry::GetStringValA(HKEY_CURRENT_USER, utf8(AppRegisterKeyNewName), "Language", strLanguage))
//     return TRUE;
//   
//   if (Registry::GetStringValA(HKEY_LOCAL_MACHINE, utf8(AppRegisterKeyOldName), "Language", strLanguage))
//     return TRUE;
//   
//   return FALSE;
// }
// bool GetRegKeyValue(const string _strKey,string & _strValue)
// {
// 	 if (Registry::GetStringValA(HKEY_CURRENT_USER, utf8(AppRegisterKeyNewName), _strKey, _strValue)) return true;
// 	 return false;
// }
// bool SetRegKeyValue(const string _strKey,const string _strValue)
// {
// 	if (Registry::SetStringValA(HKEY_CURRENT_USER, utf8(AppRegisterKeyNewName), _strKey, _strValue)) return true;
// 	return false;
// }
int ConvertStrToInt(std::string _strInt)
{
	int iRet = atoi(_strInt.c_str());
	return iRet;
}

//Converting a WChar string to a Ansi string
std::string WChar2Ansi(LPCWSTR pwszSrc)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
	if (nLen<= 0) return std::string("");
	char* pszDst = new char[nLen];
	if (NULL == pszDst) return std::string("");
	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen -1] = 0;
	std::string strTemp(pszDst);
	delete [] pszDst;
	return strTemp;
}
string ws2s(wstring& inputws){ return WChar2Ansi(inputws.c_str()); }
//Converting a Ansi string to WChar string
// std::wstring Ansi2WChar(LPCSTR pszSrc, int nLen)
// {
// 	int nSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, 0, 0);
// 	if(nSize <= 0) return _T("");
// 	WCHAR *pwszDst = new WCHAR[nSize+1];
// 	if( NULL == pwszDst) return _T("");
// 	MultiByteToWideChar(CP_ACP, 0,(LPCSTR)pszSrc, nLen, pwszDst, nSize);
// 	pwszDst[nSize] = 0;
// 	if( pwszDst[0] == 0xFEFF) // skip Oxfeff
// 		for(int i = 0; i < nSize; i ++) 
// 			pwszDst[i] = pwszDst[i+1]; 
// 	wstring wcharString(pwszDst);
// 	delete []pwszDst;
// 	return wcharString;
// }
/*std::wstring s2ws(const string& s){ return Ansi2WChar(s.c_str(),s.size());}*/
//-----------------------------------------------------------------------------
// 加密
// string EncryptString(string _strTargetString)
// {
// 	string strDest;
// 	unsigned char pchTemp[1024];
// 	memset(pchTemp,0,1024);
// 	memcpy(pchTemp,_strTargetString.c_str(),_strTargetString.length());
// 	 char cTemp[2048] = {0};
// 	//加密字符表
// 	char pchEncryptTable[10] = {'$','M','!','K','%','*','+',',','m','.'};
// 
// 	for(int i = 0;i < _strTargetString.length(); i++)
// 	{
// 		pchTemp[i] = pchTemp[i]^pchTemp[i+1]^0xAD;
// 		int iIndex1 = pchTemp[i] / 100;
// 		int iIndex2 = (pchTemp[i] / 10) % 10;
// 		int iIndex3 = pchTemp[i] % 10;		
// 		sprintf ( cTemp, "%c%c%c", pchEncryptTable[iIndex1], pchEncryptTable[iIndex2], pchEncryptTable[iIndex3]);
// 		strDest += GetCStr(cTemp);
// 	}
// 	
// 	return strDest;
// }
//----------------------------------------------------------------------------
// 解密
// string UnEncryptString(string _strTargetString)
// {
// 	string strDest;
// 	unsigned char pchTemp[1024];
// 	memset(pchTemp,0,1024);
// 	char cTemp[2048] = {0};
// 	//解密字符表
// 	char pchEncryptTable[10] = {'$','M','!','K','%','*','+',',','m','.'};
// 	int iLength = 0;
// 
// 	//解密字符串
// 	for(int i = 0;i < _strTargetString.length();i += 3)
// 	{
// 		int iIndex1 = 0;
// 		int iIndex2 = 0;
// 		int iIndex3 = 0;
// 		for(int j = 0;j < 10;j++)
// 		{
// 			if(_strTargetString[i] == pchEncryptTable[j])
// 			{
// 				iIndex1 = j;
// 				break;
// 			}
// 		}
// 		for(int j = 0;j < 10;j++)
// 		{
// 			if(_strTargetString[i + 1] == pchEncryptTable[j])
// 			{
// 				iIndex2 = j;
// 				break;
// 			}
// 		}
// 		for(int j = 0;j < 10;j++)
// 		{
// 			if(_strTargetString[i + 2] == pchEncryptTable[j])
// 			{
// 				iIndex3 = j;
// 				break;
// 			}
// 		}
// 
// 		pchTemp[iLength++] = iIndex1 * 100 + iIndex2 * 10 + iIndex3; 
// 	}
// 
// 	//还原解密
// 	for(int i = iLength;i > 0;i--)
// 		pchTemp[i-1] = pchTemp[i] ^ pchTemp[i-1] ^ 0xAD;
// 
// 	//转化为字符串
// 	for(int i = 0;i < iLength;i++)
// 	{
// 		sprintf ( cTemp, "%c", pchTemp[i]);
// 		strDest += GetCStr(cTemp);
// 	}
// 
// 	return strDest;
// }

int FilterTempFile(std::wstring _strTmp)
{
	//实现方法：
	// if
	// 1:首先判断文件的扩展名是否是在不处理的范围当中 true return
	// else
	// 2:判断文件名当中是否有特殊字符串   true return
	// return false
	wstring wstrFileName;
	wstring wstrFile = _strTmp;
	wchar_t *pstrExtension[] = {L".tmp",L".~vsd",L".ldb"};//扩展名判断
	
	wchar_t *pstrFileName[]  = {L"~$"};//文件名判断
	//查找扩展名 是否包含在pstrExtension当中
	string::size_type sPos = wstrFile.find_last_of(L".");
	D_Log::WriteLog ( D_Log::LOG_NORMAL, L"[ %s ] = [%d]\n",wstrFile.c_str(),sPos);
	 int iLen = wstrFile.length();
	 if (sPos != string::npos)
	 {
		 wstring wstrTemp =  wstrFile.substr(sPos,iLen);
		 for (int i=0;i<3;i++)
		 {
			 wstring wstr = pstrExtension[i];
			if(wstrTemp ==wstr)
			{
				if (i==0)
				{
					return 2;
				}
				else
				{
					return 1;
				}
			}
		 }
	 }
	
	//查询文件名当中是否包含pstrFileName

	wstring wstrTemp =  wstrFile.substr(0,2);
	wstring wstr     =  pstrFileName[0];
	if (wstrTemp == wstr)
	{
		return 1;
	}

	return 0;
}

// int D_copyfile(const wchar_t* tar_path, const wchar_t* src_path)
// {
// 	if (tar_path == NULL || src_path == NULL) {
// 		return -1;
// 	}
// 
// 	char rec[8096];
// 	File inFile;
// 	if (!inFile.OpenForRead(src_path).ok())
// 		return -1;
// 	File outFile;
// 	if (!outFile.OpenForWrite(tar_path, true).ok())
// 		return -1;
// 
// 	while (true) {
// 		DWORD read_bytes = 0;
// 		DWORD written_bytes = 0;
// 		Status s = inFile.Read(rec, sizeof(rec), &read_bytes);
// 		if (!s.ok())
// 			return -1;
// 		if (read_bytes == 0)
// 			break;
// 		s = outFile.Write(rec, read_bytes, &written_bytes);
// 		if (!s.ok() || written_bytes != read_bytes)
// 			return -1;
// 	}
// 
// 	return 0;
// } 

// void OutTestTime(std::wstring _wstr)
// {
// 	#ifdef _DEBUG
// 		time_t tTmp;
// 		time(&tTmp);
// 		int64_t itime = tTmp;
// 		CString cstrTmp;
// 		cstrTmp = _wstr.c_str();
// 		cstrTmp.AppendFormat(L"   %d",itime);
// 	//	TRACE(cstrTmp);
// 		OutputDebugString(cstrTmp);	
// 	#endif
// }
// 
// void OutTestFile(std::wstring _wstr,int _iLen )
// {
// 	#ifdef _DEBUG
// 		CString cstrTmp;
// 		cstrTmp.AppendFormat(L"FileName:%s  ,Length:%d",_wstr.c_str(),_iLen);
// 		OutputDebugString(cstrTmp);
// 	#endif
// }
int FindCharCount(const std::string &_strPath)
{
	int iCount = 0;
	for (int i=0;i<_strPath.length();i++)
	{
		if (_strPath[i]==',')
		{
			++iCount;
		}
	}
	return iCount;
}
//---------------------------------------------------------------------------
std::string StrTolower(const std::string &_strValue)
{
	string strRet = "";

	for(int i=0;i<_strValue.size();i++)
	{
		strRet = strRet + (char)tolower(_strValue.at(i));
	}

	return strRet;
}
std::string StrToupper(const std::string & _strValue)
{
	string strRet = "";

	for(int i=0;i<_strValue.size();i++)
	{
		strRet = strRet + (char)toupper(_strValue.at(i));
	}

	return strRet;
}

std::wstring GetAbsolutePath(const std::wstring &path)
{
  wchar_t buf[_MAX_PATH+1];
  _wfullpath(buf, path.c_str(), _MAX_PATH+1);
  return buf;
}
const char* NewGUID()
{
	static char buf[64] = {0};
	GUID guid;
	CoInitialize(NULL);
	if (S_OK == ::CoCreateGuid(&guid))
	{
		_snprintf(buf, sizeof(buf),
			"%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
			guid.Data1,
			guid.Data2,
			guid.Data3,
			guid.Data4[0], guid.Data4[1],
			guid.Data4[2], guid.Data4[3],
			guid.Data4[4], guid.Data4[5],
			guid.Data4[6], guid.Data4[7]);
	}
	CoUninitialize();
	return (const char*)buf;
}

bool IsSystem(const std::wstring &path)
{
	DWORD attr = ::GetFileAttributesW(path.c_str());
	if (attr == INVALID_FILE_ATTRIBUTES)
		return false;
	
	return (attr & FILE_ATTRIBUTE_SYSTEM) != 0;
}

static bool IsDrivePath(const std::wstring &path)
{
	if (path.length() == 2 && path[1] == L':')
		return true;
	
	if (path.length() >= 3 && path[1] == L':' && path.find_last_not_of(L'\\') == 1)
		return true;
	
	return false;
}

bool IsHidden(const std::wstring &path)
{
	if (IsDrivePath(path))
		return false;
		
	DWORD attr = ::GetFileAttributesW(path.c_str());
	if (attr == INVALID_FILE_ATTRIBUTES)
		return false;
	
	return (attr & FILE_ATTRIBUTE_HIDDEN) != 0;
}


bool IsHiddenOrParentHidden(const std::wstring &path)
{
	if (IsDrivePath(path))
		return false;

	if (IsHidden(path))
		return true;

	size_t lastslash = path.find_last_of(L'\\');
	if (lastslash == wstring::npos)
		return false;

	wstring parent = path.substr(0, lastslash);
	return IsHiddenOrParentHidden(parent);
}

static std::string BinaryHashToHexString(const unsigned char *hash, int len)
{
	std::string out;
	
	// reverse enough space to gain a little speedup
	out.reserve(len * 2 + 1);  
	for (int i = 0; i < len; ++i)
		out += D_NumFmt("%02x", (int)hash[i]).c_str();
	return out;
}

// static int GetMD5_SHA1_Stoppable(const std::wstring &path, string &digest, HANDLE stopEvent)
// {
// 	const int kBufferSize = 1024 * 16; 
// 	
// 	digest = "";
// 	int ret = 0;
// 	
// 	std::string md5dgst;
// 	std::string sha1dgest;
// 	unsigned char md5hash[16] = {0};
// 	unsigned char sha1hash[20] = {0};
// 	MD5_CTX md5ctx;
// 	SHA_CTX sha1ctx;
// 	
// 	FILE* fd;
// 	int bytes_read;
// 	char buffer[kBufferSize];
// 
// 	if ((fd=_wfopen(path.c_str(), L"rb"))==NULL) 
// 	{
// 		return -1;
// 	}
// 
// 	MD5_Init(&md5ctx);
// 	SHA1_Init(&sha1ctx);
// 
// 	while ((bytes_read=(int)fread(buffer, sizeof(char), kBufferSize, fd))) 
// 	{
// 		if (ferror(fd))
// 		{
// 			ret = -1;
// 			break;
// 		}
// 		else 
// 		{
// 			MD5_Update(&md5ctx, buffer, bytes_read);
// 			SHA1_Update(&sha1ctx, buffer, bytes_read);
// 		}
// 		
// 		if (::WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0)
// 		{
// 			ret = -1;
// 			break;
// 		}
// 	}
// 	fclose(fd);
// 	MD5_Final(md5hash, &md5ctx);
// 	SHA1_Final(sha1hash, &sha1ctx);
// 	
// 	if (ret == -1)
// 		return -1;
// 	
// 	digest += BinaryHashToHexString(md5hash, 16);
// 	digest += BinaryHashToHexString(sha1hash, 20);
// 
// 	return ret;
// }

// string GetFileDigestStoppable(const std::wstring &path, HANDLE stopEvent)
// {
// 	assert (!path.empty()); 
// 	assert (stopEvent != NULL);
// 	std::string digest;
// 	int ret = GetMD5_SHA1_Stoppable(path, digest, stopEvent);
// 
// 	return (ret == -1) ? "" : digest;
// }
int GetDayOfWeek (int nYear, int nMonth, int nDay)
{
	int i,year1,year2,month1,month2,day1,day2,a[13],days1=0,days2=0,b=0,c=0,d=0,asum=0,alldays=0;
	a[1]=a[3]=a[5]=a[7]=a[8]=a[10]=a[12]=31;
	a[4]=a[6]=a[9]=a[11]=30;
	year1 = 1;
	month1 = 1;
	day1 = 1;

	year2 = nYear;
	month2  = nMonth;
	day2 = nDay;
	for(i=year1+1;i<year2;i++)
	{
		if(i%4==0&&i%100!=0||i%400==0)
			d=366;
		else
			d=365;
		asum+=d;
	}
	if(year1%4==0&&year1%100!=0||year1%400==0)
		a[2]=29;
	else
		a[2]=28; 
	for(i=month1;i<=12;i++)
		days1+=a[i];
	b=days1-day1;
	if(year1%4==0&&year1%100!=0||year1%400==0)
		a[2]=29;
	else
		a[2]=28;
	for(i=1;i<month2;i++)
		days2+=a[i];
	c=days2+day2;
	alldays=b+c+asum;

	int nWeek = alldays % 7 + 1;
	return nWeek;
}
// void ModifyFileTime (const wchar_t* pszFilePath, const wchar_t* pszCreateTime, const wchar_t* pszWriteTime, const wchar_t* pszAccessTime)
// {
// 	if (!pszFilePath)
// 		return;
// 	
// 	if( !pszCreateTime && !pszWriteTime && !pszAccessTime)
// 		return;
// 
// 	HANDLE hFile = FileInfo::SafeCreateFile (pszFilePath, GENERIC_READ|GENERIC_WRITE, 
// 		FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
// 	if(hFile == INVALID_HANDLE_VALUE)
// 		return;
// 	int iYear, iMonth, iDate, iHour, iMin, iSec, iWeek;
// 	FILETIME ftCreateTime, ftWriteTime, ftAccessTime;
// 	LPFILETIME pftCreateTime, pftWriteTime, pftAccessTime;
// 	pftCreateTime = pftWriteTime = pftAccessTime = NULL;
// 	SYSTEMTIME st = {0}; 
// 	if(pszCreateTime)
// 	{
// 		COleDateTime oletime;
// 		oletime.ParseDateTime(pszCreateTime);
// 		st.wYear = oletime.GetYear();
// 		st.wMonth= oletime.GetMonth();
// 		st.wDay  = oletime.GetDay();
// 		st.wHour = oletime.GetHour();
// 		st.wMinute = oletime.GetMinute();
// 		st.wSecond = oletime.GetSecond();
// 		st.wDayOfWeek = oletime.GetDayOfWeek();
// 		SystemTimeToFileTime(&st, &ftCreateTime);
// 		pftCreateTime = &ftCreateTime;
// 	}
// 	if(pszWriteTime)
// 	{
// 		COleDateTime oletime;
// 		oletime.ParseDateTime(pszWriteTime);
// 		st.wYear = oletime.GetYear();
// 		st.wMonth= oletime.GetMonth();
// 		st.wDay  = oletime.GetDay();
// 		st.wHour = oletime.GetHour();
// 		st.wMinute = oletime.GetMinute();
// 		st.wSecond = oletime.GetSecond();
// 		st.wDayOfWeek = oletime.GetDayOfWeek();
// 
// 		TIME_ZONE_INFORMATION zinfo;
// 		GetTimeZoneInformation(&zinfo);
// 		TzSpecificLocalTimeToSystemTime(&zinfo, &st, &st);
// 
// 		SystemTimeToFileTime(&st, &ftWriteTime);
// 		pftWriteTime = &ftWriteTime;
// 	}
// 	if(pszAccessTime)
// 	{
// 		COleDateTime oletime;
// 		oletime.ParseDateTime(pszAccessTime);
// 		st.wYear = oletime.GetYear();
// 		st.wMonth= oletime.GetMonth();
// 		st.wDay  = oletime.GetDay();
// 		st.wHour = oletime.GetHour();
// 		st.wMinute = oletime.GetMinute();
// 		st.wSecond = oletime.GetSecond();
// 		st.wDayOfWeek = oletime.GetDayOfWeek();
// 
// 		TIME_ZONE_INFORMATION zinfo;
// 		GetTimeZoneInformation(&zinfo);
// 		TzSpecificLocalTimeToSystemTime(&zinfo, &st, &st);
// 
// 		SystemTimeToFileTime(&st, &ftAccessTime);
// 		pftAccessTime = &ftAccessTime;
// 	}
// 	DWORD dwRet = GetLastError ();
// 	SetFileTime (hFile, pftCreateTime, pftAccessTime, pftWriteTime);	
// 	CloseHandle(hFile);
// }
int CreateCatalog(const wchar_t* pszParent, const wchar_t* pszPath)
{
	wstring strParent = pszParent;
	wstring strPath = pszPath;
	if(strPath.find('\\') == wstring::npos)
	{
		strParent.append(L"\\");
		strParent.append(strPath);
		CreateDirectoryW(strParent.c_str(), NULL);
	}
	else
	{
		size_t nStartPos(1), nEndPos(0);
		while(nEndPos != wstring::npos)
		{
			nEndPos = (int)strPath.find('\\', nStartPos);
			if(nEndPos == wstring::npos)
			{
				wstring szData = strPath.substr(nStartPos);
				strParent.append(L"\\");
				strParent.append(szData);
				CreateDirectoryW(strParent.c_str(), NULL);
			}
			else
			{
				wstring szData = strPath.substr(nStartPos, nEndPos-nStartPos);
				strParent.append(L"\\");
				strParent.append(szData);
				CreateDirectoryW(strParent.c_str(), NULL);
				nStartPos = nEndPos + 1 ;
			}
		}
	}
	return 0;
}

wstring ExtractFileNameEx(wstring _strFullName)
{
	wstring strRet= L"";
	wstring strFileName(_strFullName);

	wstring::size_type iIndex = strFileName.find_last_of(L".");
	if (iIndex != std::wstring::npos)
	{
		strRet = strFileName.substr(0, iIndex);	
	}
	return strRet;
}
// string GetLastWriteUTCTime(const wchar_t* pFilePath)
// {	
// 	TCHAR szBuf[MAX_PATH];
// 	char szWriteTime[MAX_PATH] = {0};
// 	HANDLE hFile = FileInfo::SafeCreateFile(pFilePath, GENERIC_READ, FILE_SHARE_READ, NULL,
// 		OPEN_EXISTING, 0, NULL);
// 	string strUTCTime;
// 	if(hFile != INVALID_HANDLE_VALUE)
// 	{
// 		FILETIME ftCreate, ftAccess, ftWrite;
// 		SYSTEMTIME stUTC;
// 		if (GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite)
// 			&& FileTimeToSystemTime(&ftWrite, &stUTC))
// 		{
// 			_snprintf(szWriteTime, sizeof(szWriteTime)/sizeof(char) - 1, "%04d-%02d-%02d %02d:%02d:%02d", 
// 				stUTC.wYear, stUTC.wMonth, stUTC.wDay, stUTC.wHour, stUTC.wMinute, stUTC.wSecond);
// 			strUTCTime = szWriteTime;
// 		}
// 		::CloseHandle(hFile);
// 	}
// 	return strUTCTime;
// }

string IntToStr(int _iValue)
{
	char pcValue[32] = {0};
	sprintf(pcValue, "%d" ,_iValue);
	string strTemp = pcValue;
	return strTemp;
}

string Int64ToStr(int64_t value)
{
	char pcValue[65] = {0};
	sprintf(pcValue, "%lld" ,value);
	string strTemp = pcValue;
	return strTemp;
}