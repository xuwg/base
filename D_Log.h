/**********************************************************************
Copyright (c) 2016,多啦宝（北京）科技有限公司
All rights reserved.
文件名称：D_Event.h
摘 要：日志基类
版 本：1.0
作 者：朱健修改
完成日期：2016-08-20
*********************************************************************/
#ifndef _D_LOG_H__
#define _D_LOG_H__
#pragma once

#include "D_Timestamp.h"


#define LOG_LEVEL (4)
class  D_Log
{
public:
  //日志记录类型
  typedef enum _LogLevel
  {
    LOG_DEBUG = 0,	//调试
    LOG_NORMAL,		//一般
    LOG_MEDIUM,		//中
    LOG_HIGH		//高
  } ELogLevel;

private:
  D_Log(void);
public:
  ~D_Log(void);
public:
  static void WriteLog ( ELogLevel eLogLevel, const char* format, ...);
  static void WriteLog ( ELogLevel eLogLevel, const wchar_t* format, ... );
  static void WriteClientLog ( ELogLevel eLogLevel, const char* format, ...);
  static void WriteClientLog ( ELogLevel eLogLevel, const wchar_t* format, ... );

  static void WriteSerivceLog ( ELogLevel eLogLevel, const char* format, ...);
  static void WriteSerivceLog ( ELogLevel eLogLevel, const wchar_t* format, ... );

  static void DeleteOldLog ();

  static void WriteModuleMoLog ( const char* Module,ELogLevel eLogLevel, const char* format, ...);

private:
  static BOOL IsNeedDelete( const wchar_t* pszLogName );
  static ELogLevel GetLogLevel();
  static int debug_l;
  
  static void WriteLogImpl (const char *module, ELogLevel eLogLevel, const char* format, va_list arglist);
  static void WriteLogImpl (const char *module, ELogLevel eLogLevel, const wchar_t* format, va_list arglist);
};

#define WRITELOG(eLogLevel, pszErrorMsg) \
  D_Log::WriteLog(eLogLevel, "%s", pszErrorMsg);

#define LOG_DEBUG(fmt, ...) \
  D_Log::WriteLog(D_Log::LOG_DEBUG, fmt, ##__VA_ARGS__)
#define LOG_NORMAL(fmt, ...) \
  D_Log::WriteLog(D_Log::LOG_NORMAL, fmt, ##__VA_ARGS__)
#define LOG_HIGH(fmt, ...) \
  D_Log::WriteLog(D_Log::LOG_HIGH, fmt, ##__VA_ARGS__)

class D_ScopeTimer
{
public:
  D_ScopeTimer(D_Log::ELogLevel level, const std::string &function, const std::string &file, int line);
  ~D_ScopeTimer();
private:
  D_Timestamp start_;
  std::string func_;
  std::string file_;
  int         line_;
  D_Log::ELogLevel level_;
};

inline D_ScopeTimer::D_ScopeTimer(D_Log::ELogLevel level, const std::string &function, const std::string &file, int line)
: level_(level),
  func_(function),
  line_(line)
{
  const char *lastslash = strrchr(file.c_str(), '\\');
  if (lastslash != 0)
    file_ = lastslash + 1;
  else
    file_ = file;
  D_Log::WriteLog(level_, "[IN]        %-40s -%s:%d", func_.c_str(), file_.c_str(), line_);
}

inline D_ScopeTimer::~D_ScopeTimer()
{
	D_Log::WriteLog(level_, "[OUT %5d] %-40s -%s:%d", 
		(int)start_.elapsed(), func_.c_str(), file_.c_str(), line_);
}

class Sync_ScopeTimer
{
public:
	Sync_ScopeTimer(D_Log::ELogLevel level, const std::string &function, const std::string &file, int line);
	~Sync_ScopeTimer();
private:
	D_Timestamp start_;
	std::string func_;
	std::string file_;
	int         line_;
	D_Log::ELogLevel level_;
};

inline Sync_ScopeTimer::Sync_ScopeTimer(D_Log::ELogLevel level, const std::string &function, const std::string &file, int line)
: level_(level),
func_(function),
line_(line)
{
	const char *lastslash = strrchr(file.c_str(), '\\');
	if (lastslash != 0)
		file_ = lastslash + 1;
	else
		file_ = file;
	D_Log::WriteClientLog(level_, "[IN]        %-40s -%s:%d", func_.c_str(), file_.c_str(), line_);
}

inline Sync_ScopeTimer::~Sync_ScopeTimer()
{
	D_Log::WriteClientLog(level_, "[OUT %5d] %-40s -%s:%d", 
		(int)start_.elapsed(), func_.c_str(), file_.c_str(), line_);
}

#define DEFINE_FUNC_TIMER D_ScopeTimer __st##__LINE__(D_Log::LOG_DEBUG, __FUNCTION__, __FILE__, __LINE__);
#define DEFINE_SCOPE_TIMER(name) D_ScopeTimer __st##__LINE__(D_Log::LOG_DEBUG, name, __FILE__, __LINE__);
#define DEFINE_SYNC_FUNC_TIMER Sync_ScopeTimer __st##__LINE__(D_Log::LOG_DEBUG, __FUNCTION__, __FILE__, __LINE__);
#define DEFINE_SYNC_SCOPE_TIMER(name) Sync_ScopeTimer __st##__LINE__(D_Log::LOG_DEBUG, name, __FILE__, __LINE__);

#endif


