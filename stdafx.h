// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
//#include <windows.h>
//#include <afx.h>         // MFC core and standard components

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// #include <afxwin.h>         // MFC core and standard components
// #include <afxext.h>         // MFC extensions
// 
// #ifndef _AFX_NO_OLE_SUPPORT
// #include <afxole.h>         // MFC OLE classes
// #include <afxodlgs.h>       // MFC OLE dialog classes
// #include <afxdisp.h>        // MFC Automation classes
// #endif // _AFX_NO_OLE_SUPPORT
// 
// #ifndef _AFX_NO_DB_SUPPORT
// #include <afxdb.h>			// MFC ODBC database classes
// #endif // _AFX_NO_DB_SUPPORT
// 
// #ifndef _AFX_NO_DAO_SUPPORT
// #include <afxdao.h>			// MFC DAO database classes
// #endif // _AFX_NO_DAO_SUPPORT
// 
// #ifndef _AFX_NO_OLE_SUPPORT
// #include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
// #endif
// #ifndef _AFX_NO_AFXCMN_SUPPORT
// #include <afxcmn.h>			// MFC support for Windows Common Controls
// #endif // _AFX_NO_AFXCMN_SUPPORT

//#include <afx.h>

#ifdef _UNICODE
#define String wstring
#else
#define String string
#endif // _UNICODE

#pragma warning(disable: 4275)
#pragma warning(disable: 4251)
#pragma warning(disable: 4661)

#pragma warning ( disable : 4101 )
#pragma warning ( disable : 4800 )
#pragma warning ( disable : 4018 )
//#pragma warning ( disable : 4278 )
#pragma warning ( disable : 4307 )
#pragma warning ( disable : 4309 )
//#pragma warning ( disable : 4267 )
//#pragma warning ( disable : 4251 )
#pragma warning ( disable : 4996 )
#pragma   warning(   disable   :   4995   ) // name was marked as #pragma deprecated
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <atltime.h>
#include <algorithm>
#include <WinSpool.h>
#include <cctype>
#include <Aclapi.h>
#include <Psapi.h>
#include <shlobj.h>
#include <Shlwapi.h>
#include <Tlhelp32.h>
#include <strsafe.h>
#include <sys/stat.h>
#include <shlobj.h> 
#include <sys/stat.h>
#include <Psapi.h>
#include <Iphlpapi.h>
#include <locale.h>
#include <windows.h>
#include <cassert>



#include <errno.h>
#include <tchar.h> 
#include <math.h>
#include <io.h>
#include <direct.h>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <queue>
#include <list>
#include <stack>
#include <string>
#include <iostream>

using namespace std;


#include "D_DataTypes.h"
#include "D_Common.h"

#include "D_StringPrintf.h"
#include "D_StringUtils.h"
#include "D_Timestamp.h"
#include "D_Dir.h"
#include "D_Log.h"
#include "D_Lock.h"
#include "D_Thread.h"
#include "D_Event.h"
#include "D_Tool.h"
#include "D_Exception.h"
#include "D_Message.h"
#include "D_DataTypes.h"
#include "D_MzApl.h"


#pragma comment(lib, "Psapi.lib")
#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment (lib, "Dbghelp.lib")
#pragma   comment(lib, "Version.lib ") 






//#pragma   warning(   disable   :   4251   ) // ''class 'std::basic_string<_Elem,_Traits,_Ax>' needs to have dll-interface to be used by clients of class ''