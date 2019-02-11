#ifndef _D_TOOL_H__
#define _D_TOOL_H__
/************************************************************************
函数名 ：ConvertUTCToLocalTime
功能描述：将UTC时间根据当前时区转换为本地时间
输入参数：_cstrTime:UTC time
输出参数：
返回值 ： 
其它 ：
************************************************************************/

//CStringA ConvertUTCToLocalTime(CStringA _cstrTime);
//bDelSelf false:删除目录 true:清空目录内容
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

// 将字符串转为utf-8且将中文转为“%编码”格式
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
//string GetCrcEx ( LPCTSTR lpPath, LPCTSTR lpEventName ); // 获得文件摘要
//string GetCrcEx ( const wchar_t* lpPath, LPCTSTR lpEventName ); // 获得文件摘要
string GetFileDigestStoppable(const std::wstring &path, HANDLE stopEvent);
//string GetCrc ( LPCTSTR lpPath ); // 获得文件摘要
string GetCrc ( const wchar_t* lpPath ); // 获得文件摘要
DWORD TerminateProcessEx( LPCTSTR strProcessName );
BOOL GetDrive ( LPCTSTR Logical );
BOOL GetDriveA ( const char* Logical );
BOOL IsWow64();
int IsFolderOrFile(LPCTSTR strPath);
int exception_access_violation_filter(LPEXCEPTION_POINTERS p_exinfo);

// BOOL GetHostName ( wstring& strHostName ); // 获得主机名
//BOOL IsLocalFile( const wstring& wsFilePath ,const string &strFileCrc, const string &strFileSize); // 判断文件本地是否存在
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
Function：    GetGuid
Description： 通过MAC与PC Info 生成guid
Input：       
Output：      
Return：     >0 成功 <=0失败   
Others：           
************************************************************************/
int GetGuidEx(std::string & _strGuid);

/************************************************************************
Function：   FilterTempFile 
Description： 过滤office 临时文件 
Input：       
Output：      
Return：     0:其它文件上传文件  1：特殊格式不上传文件  2： tmp 文件
Others：           
************************************************************************/
int FilterTempFile(std::wstring _strTmp);
int FindCharCount(const std::string &_strPath);
std::wstring StrToupper(const std::wstring &_strValue);
//-----------------------------end------------------------------
int D_copyfile(const wchar_t* tar_path, const wchar_t* src_path);
// void OutTestTime(std::wstring _wstr);
// void OutTestFile(std::wstring _wstr,int _iLen );
/************************************************************************
Function：    StrTolower
Description： 将字符串转化为下写
Input：       _strValue：输入字符串
Output：      
Return：      转化后的字符串       
Others：           
************************************************************************/
std::string StrTolower(const std::string &_strValue);
/************************************************************************
Function：    StrToupper
Description： 将字符串转化为大写
Input：       _strValue：输入字符串
Output：      
Return：      转化后的字符串       
Others：           
************************************************************************/
std::string StrToupper(const std::string & _strValue);

std::wstring GetAbsolutePath(const std::wstring &path);
  /// Creates an absolute or full path name for the specified relative path name.


bool FileExists(const wchar_t *path);

bool FileExists(const char *path);
/************************************************************************
Function：    NewGUID
Description： 生成GUid
Input：      
Output：      
Return：     返回GUID
Others：           
************************************************************************/
const char* NewGUID();
/************************************************************************
Function：    GetRegKeyValue
Description： 获取指定项值
Input：      _strKey：项名
Output：      _strValue：内容
Return：     
Others：           
************************************************************************/
bool GetRegKeyValue(const string _strKey,string & _strValue);
/************************************************************************
Function：    SetRegKeyValue
Description： 设置指定项值
Input：      _strKey：项名，_strValue：内容
Output：      
Return：     
Others：           
************************************************************************/
bool SetRegKeyValue(const string _strKey,const string _strValue);

/// 当文件或者文件夹本身是隐藏返回true
bool IsHidden(const std::wstring &path);

/// 当文件或者文件夹本身是系统文件返回true
bool IsSystem(const std::wstring &path);

/// 当文件或者文件夹本身是隐藏或者其任意父目录为隐藏返回true
bool IsHiddenOrParentHidden(const std::wstring &path);

int GetDayOfWeek (int nYear, int nMonth, int nDay);

void ModifyFileTime (const wchar_t* pszFilePath, const wchar_t* pszCreateTime, const wchar_t* pszWriteTime, const wchar_t* pszAccessTime);
int CreateCatalog(const wchar_t* pszParent, const wchar_t* pszPath);
wstring ExtractFileNameEx(wstring _strFullName);
string GetLastWriteUTCTime(const wchar_t* pFilePath);
string IntToStr(int _iValue);
string Int64ToStr(int64_t value);


 BOOL GetMacAddress(char * pszMax ); // 获取MAC地址
#endif

