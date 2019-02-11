#include "StdAfx.h"
#include "D_Dir.h"
#include "D_Environment.h"

#include <time.h>
#define  APPDATA_NAME  L"DLBox"
#define  APPDATA_NAME_PATH  L"DLBox"


D_Dir::D_Dir(void)
{

	 m_DlbClientDir =   D_Environment::GetProductDir();
	 m_DlbClientDir =   m_DlbClientDir + L"\\";
	 m_ConfigIni          = m_DlbClientDir + L"D_Config.ini";
	 m_LogPath            = m_DlbClientDir + L"Log\\";


	CTime time = CTime::GetCurrentTime();
	CString strTime = time.Format(_T("%Y-%m-%d"));

	WCHAR szFullFileName[MAX_PATH] = { 0 };
	WCHAR szFileName[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, szFullFileName, MAX_PATH);
	_wsplitpath_s(szFullFileName, NULL, 0, NULL, 0, szFileName, MAX_PATH, NULL, 0);
	m_LogFileName = m_LogPath +  szFileName +L"_" + GetWStr(strTime) + L".log";
}

D_Dir::~D_Dir(void)
{
}


