/**********************************************************************
Copyright (c) 2016,���������������Ƽ����޹�˾
All rights reserved.
�ļ����ƣ�D_Environment.h
ժ Ҫ����������
�� ����1.0
�� �ߣ��콡�޸�
������ڣ�2016-08-20
*********************************************************************/
#ifndef  D_ENVIRONMENT_H
#define  D_ENVIRONMENT_H
#pragma once

// #include <string>
// #include "D_Common_export.h" 

class  D_Environment
{
public:
  static std::wstring GetAppDataDir(bool create);
	static std::wstring GetDesktopDir(bool create);
  static std::wstring GetCommonAppDataDir(bool create);
  static std::wstring GetExePath();
  static std::wstring GetExeDir();
  static std::wstring   GetProductDir();

};
#endif