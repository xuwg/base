#ifndef _D_TOOL_H__
#define _D_TOOL_H__
/************************************************************************
������ ��ConvertUTCToLocalTime
������������UTCʱ����ݵ�ǰʱ��ת��Ϊ����ʱ��
���������_cstrTime:UTC time
���������
����ֵ �� 
���� ��
************************************************************************/

//CStringA ConvertUTCToLocalTime(CStringA _cstrTime);
//bDelSelf false:ɾ��Ŀ¼ true:���Ŀ¼����
int DeleteDir ( const wchar_t* pszDir, bool bDelSelf = false );
int DeleteDir ( const char* pszDir,bool bDelSelf = false );

bool GetFileExtName ( const char* pszFullPath, char* pszExt );
int UTF8ToGB( const char* pszSrc, char* pszDest );
const char* toLower ( const char* pszSrc );
const wchar_t* toLower ( const wchar_t* pszSrc );
int HexToDec ( const char * pszHex );
bool GetSVDDir ( wstring& strSvgDir );
bool GetExeDir ( wstring& strExeDir );
bool IsExistFile ( const char * pszPath );
bool IsExistFile ( const wchar_t * pszPath );

const string GetGuid ();
int GetFileSize( const char *path );
int GetFileSize( const wchar_t *path );
int64_t GetFileSizeEx( const wchar_t *path );
char dec2hexChar( short int n );
short int hexChar2dec(char c);
string escapeURL(const string &URL);
string GBK2UTF8 ( const char* data );
wstring GBK2WCHAR ( const char* data );
string	WCHAR2UTF8 ( const wchar_t* data );
wstring	UTF82WCHAR ( const char* data );
string	UTF82CHAR ( const char* data );
WCHAR* AnsiToUTF8( const string& strIn );
WCHAR* AnsiToUTF8( const char* data  );
string ws2s(const wstring& ws);
//wstring s2ws(const string& s);
#ifdef _UNICODE
#define GetStr(x) GetWStr(x)
#else
#define GetStr(x) GetCStr(x)
#endif //_UNICODE

wstring GetWStr ( const char* data );
wstring GetWStr ( const wchar_t* data );
string GetCStr ( const char* data );
string GetCStr ( const wchar_t* data );

// ���ַ���תΪutf-8�ҽ�����תΪ��%���롱��ʽ
string URLEncode ( const char* data, const char* encode );

//string GetClientVersion ();

bool FolderExist ( const char* pszDir );

wstring SlashDirW ( const wchar_t* pszDir );
string SlashDir ( const char* pszDir );
wstring splitDirNameW ( const wchar_t* pszDir );
string splitDirName ( const char* pszDir );

wstring SlashLogicalW ( const wchar_t* pszDir );
string SlashLogical ( const char* pszDir );

string splitDirNameEx ( const char* pszDir );
wstring splitDirNameExW ( const wchar_t* pszDir );
string GetString ( const wchar_t* lpctstr );

string GetFileTime ( FILETIME filetime );
bool ConvertCommunicate ( LPCTSTR szName, DWORD dwNumberOfBytesToMap, LPCTSTR szMsg );
//string GetCrcEx ( LPCTSTR lpPath, LPCTSTR lpEventName ); // ����ļ�ժҪ
//string GetCrcEx ( const wchar_t* lpPath, LPCTSTR lpEventName ); // ����ļ�ժҪ
string GetFileDigestStoppable(const std::wstring &path, HANDLE stopEvent);
//string GetCrc ( LPCTSTR lpPath ); // ����ļ�ժҪ
string GetCrc ( const wchar_t* lpPath ); // ����ļ�ժҪ
DWORD TerminateProcessEx( LPCTSTR strProcessName );
BOOL GetDrive ( LPCTSTR Logical );
BOOL GetDriveA ( const char* Logical );
BOOL IsWow64();
int IsFolderOrFile(LPCTSTR strPath);
int exception_access_violation_filter(LPEXCEPTION_POINTERS p_exinfo);

// BOOL GetHostName ( wstring& strHostName ); // ���������
//BOOL IsLocalFile( const wstring& wsFilePath ,const string &strFileCrc, const string &strFileSize); // �ж��ļ������Ƿ����
string GetCurTime ();
string GetCurTimeEx ();
// 
// int MD5_CAL(const wchar_t *FilePath,  string& strMD5, bool* bExit = NULL);
// int SHA1_CAL(const wchar_t *FilePath, string& strSHA1);
// int GetMD5_SHA1 ( const wchar_t* FilePath, string& strMD5_SHA1 ); 
uint64_t filetime2int64(const FILETIME & ac_FileTime);
void GetSystemLanguage(std::string& strLanguage);
BOOL GetLanguageFromReg(std::string& strLanguage);
int ConvertStrToInt(std::string _strInt);

/************************************************************************
Function��    GetGuid
Description�� ͨ��MAC��PC Info ����guid
Input��       
Output��      
Return��     >0 �ɹ� <=0ʧ��   
Others��           
************************************************************************/
int GetGuidEx(std::string & _strGuid);

/************************************************************************
Function��   FilterTempFile 
Description�� ����office ��ʱ�ļ� 
Input��       
Output��      
Return��     0:�����ļ��ϴ��ļ�  1�������ʽ���ϴ��ļ�  2�� tmp �ļ�
Others��           
************************************************************************/
int FilterTempFile(std::wstring _strTmp);
int FindCharCount(const std::string &_strPath);
std::wstring StrToupper(const std::wstring &_strValue);
//-----------------------------end------------------------------
int D_copyfile(const wchar_t* tar_path, const wchar_t* src_path);
// void OutTestTime(std::wstring _wstr);
// void OutTestFile(std::wstring _wstr,int _iLen );
/************************************************************************
Function��    StrTolower
Description�� ���ַ���ת��Ϊ��д
Input��       _strValue�������ַ���
Output��      
Return��      ת������ַ���       
Others��           
************************************************************************/
std::string StrTolower(const std::string &_strValue);
/************************************************************************
Function��    StrToupper
Description�� ���ַ���ת��Ϊ��д
Input��       _strValue�������ַ���
Output��      
Return��      ת������ַ���       
Others��           
************************************************************************/
std::string StrToupper(const std::string & _strValue);

std::wstring GetAbsolutePath(const std::wstring &path);
  /// Creates an absolute or full path name for the specified relative path name.


bool FileExists(const wchar_t *path);

bool FileExists(const char *path);
/************************************************************************
Function��    NewGUID
Description�� ����GUid
Input��      
Output��      
Return��     ����GUID
Others��           
************************************************************************/
const char* NewGUID();
/************************************************************************
Function��    GetRegKeyValue
Description�� ��ȡָ����ֵ
Input��      _strKey������
Output��      _strValue������
Return��     
Others��           
************************************************************************/
bool GetRegKeyValue(const string _strKey,string & _strValue);
/************************************************************************
Function��    SetRegKeyValue
Description�� ����ָ����ֵ
Input��      _strKey��������_strValue������
Output��      
Return��     
Others��           
************************************************************************/
bool SetRegKeyValue(const string _strKey,const string _strValue);

/// ���ļ������ļ��б��������ط���true
bool IsHidden(const std::wstring &path);

/// ���ļ������ļ��б�����ϵͳ�ļ�����true
bool IsSystem(const std::wstring &path);

/// ���ļ������ļ��б��������ػ��������⸸Ŀ¼Ϊ���ط���true
bool IsHiddenOrParentHidden(const std::wstring &path);

int GetDayOfWeek (int nYear, int nMonth, int nDay);

void ModifyFileTime (const wchar_t* pszFilePath, const wchar_t* pszCreateTime, const wchar_t* pszWriteTime, const wchar_t* pszAccessTime);
int CreateCatalog(const wchar_t* pszParent, const wchar_t* pszPath);
wstring ExtractFileNameEx(wstring _strFullName);
string GetLastWriteUTCTime(const wchar_t* pFilePath);
string IntToStr(int _iValue);
string Int64ToStr(int64_t value);


 BOOL GetMacAddress(char * pszMax ); // ��ȡMAC��ַ
#endif

