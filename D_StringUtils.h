
/**********************************************************************
Copyright (c) 2016,多啦宝（北京）科技有限公司
All rights reserved.
文件名称：D_Event.h
摘 要：字符处理基类
版 本：1.0
作 者：朱健修改
完成日期：2016-08-20
*********************************************************************/
#ifndef D_STRINGUTILS_H
#define D_STRINGUTILS_H

#pragma once


class  D_Encoding
{
public:
  static std::string WCharToUtf8(const std::wstring &wstr);
  static std::string WCharToUtf8(const wchar_t *wstr);

  /// ANSI means the default encoding used by Windows, 
  /// for example, in CHS Windows, this should be GBK
  static std::string WCharToAnsi(const std::wstring &wstr);
  static std::string WCharToAnsi(const wchar_t *wstr);

  static std::wstring Utf8ToWChar(const std::string &utf8str);
  static std::wstring Utf8ToWChar(const char *utf8str);

  static std::wstring AnsiToWChar(const std::string &ansi);
  static std::wstring AnsiToWChar(const char *ansi);

};

inline std::string utf8(const std::wstring &wstr)
{
	return D_Encoding::WCharToUtf8(wstr);
}

inline std::string utf8(const wchar_t *wstr)
{
	return D_Encoding::WCharToUtf8(wstr);
}

inline std::wstring wide(const std::string &utf8str)
{
	return D_Encoding::Utf8ToWChar(utf8str);
}

inline std::wstring wide(const char *utf8str)
{
	return D_Encoding::Utf8ToWChar(utf8str);
}

class   D_NumFmt
{
public:
  template <typename T>
  D_NumFmt(const char *, T num);
  const char *c_str() const { return data_; }
  const std::string str() const { return data_; }
private:
  char data_[64];
};

template <typename T>
inline D_NumFmt::D_NumFmt(const char *fmt, T num)
{
  _snprintf_s(data_, 64, _TRUNCATE, fmt, num);
}

inline std::ostream& operator << (std::ostream &stream, const D_NumFmt &fmt)
{
  stream << fmt.c_str();
  return stream;
}


inline std::string& operator += (std::string &str, const D_NumFmt &fmt)
{
  str += fmt.str();
  return str;
}

inline std::string  operator +  (std::string &str, const D_NumFmt &fmt)
{
  return str + fmt.str();
}

/*
void exmample()
{
  std::string s;
  s += NumFmt("%d", 100);

  std::string k = s + NumFmt("%d", 300);
  std::ostringstream oss;
  oss << NumFmt("%d", 100);
}
*/

  bool StartsWith(const std::wstring &str, const std::wstring &prefix, bool ignoreCase = false);

  bool iequals(const std::wstring &ws1, const std::wstring &ws2);

  std::vector<std::wstring> SplitString(const std::wstring &src, wchar_t sep, bool ignoreEmptyItem = true);
 #endif