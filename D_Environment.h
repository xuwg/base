/**********************************************************************
Copyright (c) 2016,多啦宝（北京）科技有限公司
All rights reserved.
文件名称：D_Environment.h
摘 要：环境基类
版 本：1.0
作 者：朱健修改
完成日期：2016-08-20
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