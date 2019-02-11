// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� D_COMMON_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
//  ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
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

#define  PC_CODE_SIZE        (32)      //PCΨһ��ʶ�ĳ���


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
#define  D_CONFIG_HOSTINFO          (("HOSTINFO"))       //��¼ʱ��ȡ�ķ�������Ϣ��
#define  D_HOSTINFO_VERSION         (("Version"))        //�ͻ��˰汾

//�ж������ַ����Ƿ����
#define IEQUAL(str1, str2) (str1 && str2 && (0 == _stricmp(str1, str2)))

#define IEQUALW(str1, str2) (str1 && str2 && (0 == _wcsicmp(str1, str2)))

//ZwQueryInfo�ĺ���ָ��
typedef LONG (CALLBACK* ZWQUERYOBJECT)(  
	HANDLE ObjectHandle,  
	ULONG  ObjectInformationClass,  
	PVOID  ObjectInformation,  
	ULONG  ObjectInformationLength,  
	PULONG ReturnLength  
	); 

//��ʵ���������е�UNICODE_STRING
typedef struct _MYUNICODE_STRING_ {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} MYUNICODE_STRING, *PMYUNICODE_STRING;

//
typedef BOOL (WINAPI *_ChangeWindowMessageFilter)( UINT , DWORD);



 //����ָ�����ֵķ���
 BOOL StartServerByName(LPCSTR lpServerName);




//���ݾ����ȡ��������
 BOOL GetFileNameFromHandle(HANDLE hFile, LPCSTR lpFilePath, DWORD dwBufCount);

//��õ�ǰ���̵�����
 BOOL GetCurrentProcessName(LPSTR lpProcessName, DWORD dwBufCount);


//���ݴ�ӡ�����ֻ�ȡ��ӡ�����
 HDC GetPrinterDC(char* pPrinterName);

 //�����ӡ�����֣��ͳ�ʱʱ�䣨s����ɾ���������ʱ�䲢���Ѿ���ɵĴ�ӡ����
 VOID DelPrintedJosTimeOut(char* pPrinterName, DWORD dwTimeSecond);


//��ȡ��ǰĿ¼
 BOOL GetModelDir(LPSTR lpPath, UINT uiBufSize);

/*****************************************
��������CreateDirectoryPath
���ܣ�����һ��·��
������_pszPathҪ������·��
����ֵ���ɹ���TRUE ʧ�ܣ�FALSE
*****************************************/
 BOOL CreateDirectoryPath(LPCSTR _pszPath);
 BOOL CreateDirectoryPathW(LPCWSTR _pwzPath);

/*****************************************
��������CopyFileMust
���ܣ������ļ�
������lpSrcFiles��Դ�ļ�·�� LpDestPath��Ŀ���ļ�·��
����ֵ���ɹ���TRUE ʧ�ܣ�FALSE
��Դ�ļ�������ʧ�ܣ� Ŀ��·�������ڲ��Ҵ���������ʧ�ܣ� �����ļ�ʧ��ʧ�ܣ�
*****************************************/
 BOOL CopyFileMust(LPCSTR lpSrcFiles, LPCSTR LpDestPath);

/*****************************************
��������RemoveDir
���ܣ��ݹ�ɾ��Ŀ¼
����ֵ���ɹ���TRUE ʧ�ܣ�FALSE
******************************************/
 BOOL RemoveDir(const char* szFileDir);

/*****************************************
��������RemoveUpdataTempDir
���ܣ�ɾ������ʱ������ԭ�������ļ���Ŀ¼
����ֵ���ɹ���TRUE ʧ�ܣ�FALSE
����Ϊԭ�������ļ�����ʱ����ɾ�������ƶ����������ٴ�����ʱɾ���ϴ�ԭ���ļ���Ŀ¼RemoveDir��
*****************************************/
 BOOL RemoveUpdataTempDir();



/****************************************************
��������MoveFileWithFlag
���ܣ������������ļ�·�����ƶ��ļ����ļ���ʽ���� XXX0.exe XXX1.exe ---->(XXX.exe)�� 
	0:����ǰִ�г����Ŀ¼
	1������ϵͳĿ¼��system32��
������lpFilePath��Դ�ļ�·��
     lpVersion: ���°汾��(���磺1.0.0.0)
����ֵ���ɹ���TRUE ʧ�ܣ�FALSE
*****************************************************/
 BOOL MoveFileWithFlag(LPCSTR lpFilePath, LPCSTR lpVersion);




/****************************************************
��������MoveFileWithDirFlag
���ܣ������������ļ�·�����ƶ��ļ����ļ���ʽ���� //SYSDIR_TEMP//XXX.exe //MODULE_TEMP//XXX.exe ---->(XXX.exe)�� 
	MODULE_TEMP:����ǰִ�г����Ŀ¼
	SYSDIR_TEMP������ϵͳĿ¼��system32��
������lpFilePath��Դ�ļ�·��
����ֵ���ɹ���TRUE ʧ�ܣ�FALSE
*****************************************************/
 BOOL MoveFileWithDirFlag(LPCSTR lpFilePath);


/****************************************************
��������MoveDirWithDirFlag
���ܣ�������Ŀ¼�������ļ�����Ŀ¼����ת�浽ָ��Ŀ¼���ļ���ʽ���磺..\SYSDIR_TEMP---->ϵͳsystem32�� 
                                                         ..\MODULE_TEMP---->��Ʒ��װĿ¼��
														 ..\DIRVER_TEMP---->DriversĿ¼�£�
������lpFilePath��Դ�ļ�·��
����ֵ���ɹ���TRUE ʧ�ܣ�FALSE
****************************************************/
 BOOL MoveDirWithDirFlag(LPCSTR lpDirPath);


/*
���ַ�����ʽ�ڴ�װ��Ϊ�ڴ�
*/
 int String2Bytes(unsigned char* szSrc, unsigned char* pDst, int nDstMaxLen);


/*
���ĳ���ĳ���ж�����
*/
 WORD GetMonthDay(WORD wYear, WORD wMonth);

//��ȡ����������������һ������
 void GetMondy(SYSTEMTIME *pST);
//��ȡ��ǰ���ڵ���һ��
 void GetNextDay(SYSTEMTIME *pST);
/*
��ȡ��װ·������ȡע����ã�
*/
 BOOL GetProductDirA(char* pDir, DWORD dwSize);
 BOOL GetProductDirW(wchar_t* pwDir, DWORD dwSize);

 //��ȡ�����ĸ�������
 BOOL GetRandName(LPSTR pRandName, DWORD dwBufLen);

 //��ȡ���������������ע������汣����������������
 BOOL GetDlbRegRandName(char* pRandName, DWORD dwSize);

 //�������������������д��ע���
 BOOL SetAutoRun(char* pRandName, DWORD dwCont);

/*
ɾ����һ�ܵ���־�ļ�
*/
 VOID DeletLogWithTime();

/*
inet_ntop :������ַ��ֻ��vista����֧�֣��Լ�ʵ��
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

//base�����ࣨ���������ļ���������BASE64���룩
class  ZBase64
{
public:
    /*����
    DataByte
        [in]��������ݳ���,���ֽ�Ϊ��λ
    */
    string Encode(const unsigned char* Data,int DataByte);
    /*����
    DataByte
        [in]��������ݳ���,���ֽ�Ϊ��λ
    OutByte
        [out]��������ݳ���,���ֽ�Ϊ��λ,�벻Ҫͨ������ֵ����
        ������ݵĳ���
    */
    string Decode(const char* Data,int DataByte,int& OutByte);
};


//������ǰ����Ȩ��
 bool AdjustProcessTokenPrivlege();


/*****************************************
���ݽ�������ý��̾��
RUN:���̾����ʧ�ܷ���INVALID_HANDLE_VALUE
*****************************************/
 HANDLE GetprocessHandByName(PWCHAR lpProcessname);

 /*
 ���ݽ������ֻ�ȡ����PID
 lpProcessname����������
 dwCount      : ���Σ�����ͬ����������
 ����ֵ:���ܷ��ض��pid ,ÿ��pidռһ��DWORD�Ŀռ�, �ں����ڲ�new��,��Ҫ�ⲿ�ͷţ����ΪNULL��֤����ȡʧ�ܣ�����û��
 */
PDWORD GetAllProcessIdByName(PWCHAR lpProcessname, PDWORD pdwCount);

 /*
 ����·��������ǰ�Ƿ���ڵ�ǰ·���¿�ִ���ļ������ߵ�ǰĿ¼�����ļ����µĿ�ִ���ļ�
 lpProcessname��·������
 dwCount      : ���Σ�����ͬ����������
 ����ֵ:���ܷ��ض��pid ,ÿ��pidռһ��DWORD�Ŀռ�, �ں����ڲ�new��,��Ҫ�ⲿ�ͷţ����ΪNULL��֤����ȡʧ�ܣ�����û��
 */
PDWORD GetAllProcessIdByPath(PWCHAR lpPath, PDWORD pdwCount);

//�жϵ�ǰ�����Ƿ������ָ��ģ��
 BOOL IsLoadModule(HANDLE hProcess, LPCSTR lpModulePath);

//�ж�ָ�������Ƿ����
BOOL WhaterProcess(LPCSTR pprocessNmae);// �������жϽ����Ƿ����

DWORD GetProcessIdByName(LPCSTR pprocessNmae);//�����û����ֻ�ȡ�û�pid

//ǿ��ɱ����
BOOL KillProcess(LPCSTR pprocessNmae); //ɱ������ָ�����ֵĽ���

//���û�ģʽ�������̣��ڷ���ȿ�SESSION������ʹ�ã�
 int CreateUserProcess(LPTSTR filename, STARTUPINFO &siStartInfo, PROCESS_INFORMATION &piProcInfo);

//��ȡʱ�������
 __int64 SecondsBase1970(const char *szSysTime);
 void TimeFromBase1970(double dDiff, char *szSysTime);

// ��ȫ�ͷ��ڴ�


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
// Method:    �õ���ӡ��ֽ�ŵĿ��
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



 //�ڲ���������ΨһID�����������CPUID���㷵��32λID��
 BOOL GetPcPostId(char* szId, size_t ibuflen);


 //��"012345678"��ʽ�������ַ���ת���ɡ���һ�����������߰ˡ���ʽ�������ַ���
 string NumberStringToChinese(char* strNumber);

 //��2017��ʽ�������ַ���ת���ɡ���ǧ��һʮ�ߡ���ʽ�����������ַ���
 string NumberValueToChinese(char* strNumber);

 //��2017.00��ʽ�Ĵ�С����������ַ���ת���ɡ�����ʽ�������ַ���
 string NumberFloatValueToChinese(char* strNumber);

 //�����硰YYYY-MM-DD HH:MM:SS��(���磺��2017-08-02 15:43:58��) ��ʽ��ʱ��ת���� "������ʱ��������ı���"�����磺������һ������¶���һʮ��ʱ��ʮ������ʮ���롱��
 string NumberTimeToChineseTime(char* strNumber);

 //�����ַ���strReplace �в������е�strSrc�Ӵ������滻��strDes
 string ReplaceSubStr(char* strReplace, char* strSrc, char* strDes);


 //ע��ȫ����Ϣ�����ӵ�Ȩ�޽������Ȩ�޽��̷�����Ϣ���ܾ�����Ȩ�޽��̵��ã�
 BOOL AllowMeesageForVista(UINT uMessageID, BOOL bAllow);

 //������Ļ�ϵĵ㣬ȡ�øõ�����Ĵ��ھ������һ
 HWND GetWndFromPoint(POINT pt);

 //������Ļ�ϵĵ㣬ȡ�øõ�����Ĵ��ھ����������fShowHidden���Ƿ���ҵ����ɼ����ڣ�
 HWND GetWndFromPointSicond(POINT pt, BOOL fShowHidden);

 //�����ڵı߿�
 void InvertDrawFrame(HWND hWndPoint); 

 //���ݽ���PID����ȡ��������
 BOOL GetProcessNameById(DWORD pid, LPSTR pprocessNmae, DWORD dwLen);

 //��ʱ��ͬ����������ȡʱ����Ϣ   
 DWORD GetTimeFromServer(BOOL bUserProxy, char *ip_addr, char* ip_Proxy, int port_Proxy);
 //���Ӻ�����ת��ΪSYSTEMTIME
 SYSTEMTIME FormatServerTime(DWORD serverTime);

 SYSTEMTIME Time_tToSystemTime(time_t t);
 time_t SystemTimeToTime_t( const SYSTEMTIME& st );

 //�Ƚ�����ϵͳʱ��
 int CompareSystemTime(SYSTEMTIME s1, SYSTEMTIME s2);


#endif