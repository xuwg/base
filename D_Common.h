// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 D_COMMON_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
//  函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
// #ifdef D_COMMON_EXPORTS
// #define  __declspec(dllexport)
// #else
// #define  __declspec(dllimport)
// #endif

#ifndef D_COMMON_H
#define D_COMMON_H



enum D_CODE {
	D_SUCCEED = 					1,
	D_FAILED = 						0,
	D_ERROR = 						-1,
	D_NO_VALUE = 					-10,

	D_ERROR_POINTERNULL = 90001,
	D_ACCESS_VIOLATION,


	D_UPLOAD_FAILED,
	D_ERROR_UPLOAD_RET,
	D_FAILED_CONNECT_SERVER,
	D_ILLEGAL_IMAGELIST_XML,
	D_SUCCESS_AUTH,
	D_ERROR_AUTHACCOUNT,
	D_ERROR_AUTHPASSWORD,
	D_ERROR_SERVER,
	D_ERROR_URL,
	D_ERROR_UPLOADIMAGE,
	D_ERROR_NO_INTERNET_CONN,
	D_ERROR_UNKNOWN,
	D_ERROR_DOWNLOAD,



	D_FILE_NEANDCC,
	D_FILE_NEANDSC,
	D_FILE_NEANDNC,
	D_FILE_CSANDUF,
	D_XML_PARSEERROR,
	D_SUCCESS_ADDTASKLIST,
	D_ERROR_ADDTASKLIST,
	D_FILE_EXANDRE,
	D_FILE_NUPLOAD_LIMIT,
	D_FILE_SVR_EXIST,


	D_BACK_FILE10M_LIMIT,

	D_BACK_NONDOC_SAPCELESS, 
	D_BACK_TOTAL_SPACELESS,

	D_SIGNUP_USER_ALREADY_EXISTS,

	//open api returns non-200 http code
	D_OPENAPI_NO_CONTENT,                 // 204
	D_OPENAPI_NOT_MODIFIED,               // 304
	D_OPENAPI_308,                        // 308
	D_OPENAPI_BAD_REQUESET,               // 400
	D_OPENAPI_INVALID_TOKEN,							// 401
	D_OPENAPI_INVALID_OAUTH_REQUEST,      // 403
	D_OPENAPI_FILE_NOT_FOUND,             // 404
	D_OPENAPI_METHOD_UNEXPECTED,          // 405
	D_OPENAPI_FILE_OR_DIR_EXISTS,         // 409
	D_OPENAPI_SERVER_FILE_CHANGED,        // 412

	D_INVALID_DCM_FILE,
	D_CANCELLED,
	D_NOT_FOUND,


};

#define  PC_CODE_SIZE        (32)      //PC唯一标识的长度


#define  APPDATA_NAME  L"DLBox"
#define  APPDATA_NAME_PATH  L"DLBox"

#define SYSDIR_TEMP  "SYSDIR"
#define MODULE_TEMP  "MODEDIR"
#define DIRVER_TEMP  "DRIVERDIR"
#define LOSTDIR_TEMP _T("LostTemp")
#define REMOVE_DIR   _T("RemoveDir")
#define UPDATA_TEMP  _T("UpdataTemp")
#define LOG_FILE_PARH "Log"

#define DUL_BOX_REG_NAME  "SOFTWARE\\DLBox"
#define DUL_BOX_REG_NAMEW  L"SOFTWARE\\DLBox"

//#define D_CONFIG_INI	"D_Config.ini"
#define  D_CONFIG_HOSTINFO          (("HOSTINFO"))       //登录时读取的服务器信息节
#define  D_HOSTINFO_VERSION         (("Version"))        //客户端版本

//判断两个字符串是否相等
#define IEQUAL(str1, str2) (str1 && str2 && (0 == _stricmp(str1, str2)))

#define IEQUALW(str1, str2) (str1 && str2 && (0 == _wcsicmp(str1, str2)))

//ZwQueryInfo的函数指针
typedef LONG (CALLBACK* ZWQUERYOBJECT)(  
	HANDLE ObjectHandle,  
	ULONG  ObjectInformationClass,  
	PVOID  ObjectInformation,  
	ULONG  ObjectInformationLength,  
	PULONG ReturnLength  
	); 

//其实就是驱动中的UNICODE_STRING
typedef struct _MYUNICODE_STRING_ {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} MYUNICODE_STRING, *PMYUNICODE_STRING;

//
typedef BOOL (WINAPI *_ChangeWindowMessageFilter)( UINT , DWORD);



 //启动指定名字的服务
 BOOL StartServerByName(LPCSTR lpServerName);




//根据句柄获取对象名字
 BOOL GetFileNameFromHandle(HANDLE hFile, LPCSTR lpFilePath, DWORD dwBufCount);

//获得当前进程的名字
 BOOL GetCurrentProcessName(LPSTR lpProcessName, DWORD dwBufCount);


//根据打印机名字获取打印机句柄
 HDC GetPrinterDC(char* pPrinterName);

 //传入打印机名字，和超时时间（s），删除超过这个时间并且已经完成的打印任务
 VOID DelPrintedJosTimeOut(char* pPrinterName, DWORD dwTimeSecond);


//获取当前目录
 BOOL GetModelDir(LPSTR lpPath, UINT uiBufSize);

/*****************************************
函数名：CreateDirectoryPath
功能：创建一条路径
参数：_pszPath要创建的路径
返回值：成功，TRUE 失败，FALSE
*****************************************/
 BOOL CreateDirectoryPath(LPCSTR _pszPath);
 BOOL CreateDirectoryPathW(LPCWSTR _pwzPath);

/*****************************************
函数名：CopyFileMust
功能：拷贝文件
参数：lpSrcFiles：源文件路径 LpDestPath：目的文件路径
返回值：成功，TRUE 失败，FALSE
（源文件不存在失败， 目的路径不存在并且创建不出来失败， 拷贝文件失败失败）
*****************************************/
 BOOL CopyFileMust(LPCSTR lpSrcFiles, LPCSTR LpDestPath);

/*****************************************
函数名：RemoveDir
功能：递归删除目录
返回值：成功，TRUE 失败，FALSE
******************************************/
 BOOL RemoveDir(const char* szFileDir);

/*****************************************
函数名：RemoveUpdataTempDir
功能：删除升级时，保存原来存在文件的目录
返回值：成功，TRUE 失败，FALSE
（因为原来存在文件运行时不能删除，可移动，所以在再次启动时删除上次原来文件的目录RemoveDir）
*****************************************/
 BOOL RemoveUpdataTempDir();



/****************************************************
函数名：MoveFileWithFlag
功能：解析给出的文件路径，移动文件（文件格式形如 XXX0.exe XXX1.exe ---->(XXX.exe)） 
	0:代表当前执行程序的目录
	1：代表系统目录（system32）
参数：lpFilePath：源文件路径
     lpVersion: 最新版本号(例如：1.0.0.0)
返回值：成功，TRUE 失败，FALSE
*****************************************************/
 BOOL MoveFileWithFlag(LPCSTR lpFilePath, LPCSTR lpVersion);




/****************************************************
函数名：MoveFileWithDirFlag
功能：解析给出的文件路径，移动文件（文件格式形如 //SYSDIR_TEMP//XXX.exe //MODULE_TEMP//XXX.exe ---->(XXX.exe)） 
	MODULE_TEMP:代表当前执行程序的目录
	SYSDIR_TEMP：代表系统目录（system32）
参数：lpFilePath：源文件路径
返回值：成功，TRUE 失败，FALSE
*****************************************************/
 BOOL MoveFileWithDirFlag(LPCSTR lpFilePath);


/****************************************************
函数名：MoveDirWithDirFlag
功能：将传入目录下所有文件根据目录名，转存到指定目录（文件格式形如：..\SYSDIR_TEMP---->系统system32下 
                                                         ..\MODULE_TEMP---->产品安装目录下
														 ..\DIRVER_TEMP---->Drivers目录下）
参数：lpFilePath：源文件路径
返回值：成功，TRUE 失败，FALSE
****************************************************/
 BOOL MoveDirWithDirFlag(LPCSTR lpDirPath);


/*
将字符串形式内存装换为内存
*/
 int String2Bytes(unsigned char* szSrc, unsigned char* pDst, int nDstMaxLen);


/*
获得某年的某月有多少天
*/
 WORD GetMonthDay(WORD wYear, WORD wMonth);

//获取参数日期所在周周一的日期
 void GetMondy(SYSTEMTIME *pST);
//获取当前日期的下一天
 void GetNextDay(SYSTEMTIME *pST);
/*
获取安装路径（读取注册表获得）
*/
 BOOL GetProductDirA(char* pDir, DWORD dwSize);
 BOOL GetProductDirW(wchar_t* pwDir, DWORD dwSize);

 //获取随机字母组成名字
 BOOL GetRandName(LPSTR pRandName, DWORD dwBufLen);

 //获取哆啦宝随机重名名注册表里面保存的随机重命名名字
 BOOL GetDlbRegRandName(char* pRandName, DWORD dwSize);

 //将随机启动重命名名字写入注册表
 BOOL SetAutoRun(char* pRandName, DWORD dwCont);

/*
删除上一周的日志文件
*/
 VOID DeletLogWithTime();

/*
inet_ntop :解析地址，只在vista以上支持，自己实现
*/
 const char * My_inet_ntop(int af, const void *src, char *dst, size_t size);
/*
Ipv4
*/
   const char * My_inet_ntop_v4 (const void *src, char *dst, size_t size);
/*
Ipv6
*/
//static const char * My_inet_ntop_v6 (const u_char *src, char *dst, size_t size);

//base解析类（升级下载文件的数据是BASE64编码）
class  ZBase64
{
public:
    /*编码
    DataByte
        [in]输入的数据长度,以字节为单位
    */
    string Encode(const unsigned char* Data,int DataByte);
    /*解码
    DataByte
        [in]输入的数据长度,以字节为单位
    OutByte
        [out]输出的数据长度,以字节为单位,请不要通过返回值计算
        输出数据的长度
    */
    string Decode(const char* Data,int DataByte,int& OutByte);
};


//提升当前进程权限
 bool AdjustProcessTokenPrivlege();


/*****************************************
根据进程名获得进程句柄
RUN:进程句柄，失败返回INVALID_HANDLE_VALUE
*****************************************/
 HANDLE GetprocessHandByName(PWCHAR lpProcessname);

 /*
 根据进程名字获取进程PID
 lpProcessname：进程名字
 dwCount      : 出参，返回同名进程数量
 返回值:可能返回多个pid ,每个pid占一个DWORD的空间, 在函数内部new的,需要外部释放，如果为NULL，证明获取失败，或者没有
 */
PDWORD GetAllProcessIdByName(PWCHAR lpProcessname, PDWORD pdwCount);

 /*
 根据路径遍历当前是否存在当前路径下可执行文件，或者当前目录下子文件夹下的可执行文件
 lpProcessname：路径名字
 dwCount      : 出参，返回同名进程数量
 返回值:可能返回多个pid ,每个pid占一个DWORD的空间, 在函数内部new的,需要外部释放，如果为NULL，证明获取失败，或者没有
 */
PDWORD GetAllProcessIdByPath(PWCHAR lpPath, PDWORD pdwCount);

//判断当前进程是否加载了指定模块
 BOOL IsLoadModule(HANDLE hProcess, LPCSTR lpModulePath);

//判断指定进程是否存在
BOOL WhaterProcess(LPCSTR pprocessNmae);// 根据名判断进程是否存在

DWORD GetProcessIdByName(LPCSTR pprocessNmae);//根据用户名字获取用户pid

//强制杀进程
BOOL KillProcess(LPCSTR pprocessNmae); //杀死所有指定名字的进程

//以用户模式创建进程（在服务等跨SESSION程序中使用）
 int CreateUserProcess(LPTSTR filename, STARTUPINFO &siStartInfo, PROCESS_INFORMATION &piProcInfo);

//获取时间戳函数
 __int64 SecondsBase1970(const char *szSysTime);
 void TimeFromBase1970(double dDiff, char *szSysTime);

// 安全释放内存


// 
 string BuildSignStringCommon( const char * szSecretKey, vector<string> &strVec, string &strFirst, char *szTimeStamp );

 ULONGLONG CurrentTimeStampCommon();
 BOOL DirectoryList(LPCSTR Path, vector<string>& vectorStr);
 BOOL StringConvertDateFormat(string& strInDate,OUT WORD& dwYear, OUT WORD& dwMonth, OUT WORD & dwDay);
 BOOL bIsLeftDateLessThanRightDate(SYSTEMTIME& lp, SYSTEMTIME& rp);
 VOID SaveLogInThirtyDays();
 VOID SaveHandoverFileInThirtyDays();
 VOID FilterFile(vector<string>& vecIn);
 VOID SafeFreeMem(  VOID* pszMem );
 BOOL GetAllUsersDesktopPath(char *pszDesktopPath);
 BOOL CreateFileShortcut( LPSTR lpszFileName, LPCSTR lpszLnkFileDir, LPCSTR lpszLnkFileName, LPSTR lpszWorkDir, WORD wHotkey, LPSTR lpszDescription, int iShowCmd = SW_SHOWNORMAL);


//************************************
// Method:    GetDLBVersion
// FullName:  GetDLBVersion
// Access:    public 
// Returns:   string
// Qualifier:
//************************************
 string GetDLBVersion();


//************************************
// Method:    得到打印机纸张的宽度
// FullName:  GetPrinterPaperWidth
// Access:    public 
// Returns:   DWORD
// Qualifier:
// Parameter: char * pPrinterName
//************************************
 DWORD GetPrinterPaperWidth(LPSTR pPrinterName);


 bool GetStringValW(HKEY hKey, const std::wstring &subkey, const std::wstring &name, std::wstring &value);

 bool GetStringValA(HKEY hKey, const std::string &subkey, const std::string &name, std::string &value);

 BOOL X64OS();


 void 	MzHexStrToBytes(char* pHexString,char* pOutput);
 BOOL	RawFindQrEmbeKey(char* pData,size_t uDataSize,char* szQrKey);
 VOID	RawGetMoneyValue(char* pData,size_t uDataSize,char* szMoneyKey,char* szMoneyValue);
 VOID RawGetDeskNo(char* pData,size_t uDataSize,char* szDeskNoKey, char *szDeskNoTailKey,char* szDeskNo);

 void	MzGetTextPos(char* pText,char* szPos);
 void	MzAddPosData(char* pRawFile,char* pCutCommand,char* pData,size_t uDataSize);



 long MyRand(int n);
 string BuildSignStringCommon( const char * szSecretKey, vector<string> &strVec, string &strFirst, char *szTimeStamp );

 string&   replace_all(string&   str,const   string&   old_value,const   string&   new_value)  ;
 BOOL StringToWString(const std::string &str,std::wstring &wstr);

 void Wchar_tToString(std::string& szDst, wchar_t *wchar);



 //内部计算计算机唯一ID（根据主板和CPUID计算返回32位ID）
 BOOL GetPcPostId(char* szId, size_t ibuflen);


 //将"012345678"形式的数字字符串转换成“零一二三四五六七八”形式的中文字符串
 string NumberStringToChinese(char* strNumber);

 //将2017形式的数字字符串转换成“两千零一十七”形式的中文数字字符串
 string NumberValueToChinese(char* strNumber);

 //将2017.00形式的带小数点的数字字符串转换成“”形式的中文字符串
 string NumberFloatValueToChinese(char* strNumber);

 //将形如“YYYY-MM-DD HH:MM:SS”(例如：“2017-08-02 15:43:58”) 形式的时间转换成 "年月日时分秒的中文表述"（例如：“二零一七年八月二日一十五时四十三分五十八秒”）
 string NumberTimeToChineseTime(char* strNumber);

 //将在字符串strReplace 中查找所有的strSrc子串并且替换成strDes
 string ReplaceSubStr(char* strReplace, char* strSrc, char* strDes);


 //注册全局消息，房子低权限进程向高权限进程发送消息被拒绝（高权限进程调用）
 BOOL AllowMeesageForVista(UINT uMessageID, BOOL bAllow);

 //根据屏幕上的点，取得该点下面的窗口句柄方法一
 HWND GetWndFromPoint(POINT pt);

 //根据屏幕上的点，取得该点下面的窗口句柄方法二（fShowHidden：是否查找到不可见窗口）
 HWND GetWndFromPointSicond(POINT pt, BOOL fShowHidden);

 //画窗口的边框
 void InvertDrawFrame(HWND hWndPoint); 

 //根据进程PID，获取进程名字
 BOOL GetProcessNameById(DWORD pid, LPSTR pprocessNmae, DWORD dwLen);

 //从时间同步服务器获取时间信息   
 DWORD GetTimeFromServer(BOOL bUserProxy, char *ip_addr, char* ip_Proxy, int port_Proxy);
 //将从毫秒数转化为SYSTEMTIME
 SYSTEMTIME FormatServerTime(DWORD serverTime);

 SYSTEMTIME Time_tToSystemTime(time_t t);
 time_t SystemTimeToTime_t( const SYSTEMTIME& st );

 //比较两个系统时间
 int CompareSystemTime(SYSTEMTIME s1, SYSTEMTIME s2);


#endif