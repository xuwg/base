/**********************************************************************
Copyright (c) 2016,多啦宝（北京）科技有限公司
All rights reserved.
文件名称：D_StringPrintf.h
摘 要：字符格式化处理
版 本：1.0
作 者：朱健修改
完成日期：2016-08-20
*********************************************************************/
#ifndef D_STRINGPRINTF_H
#define D_STRINGPRINTF_H
#pragma once









// SafePrintf() series: will truncate if buffer is too small, 
// returns the number of characters stored in buffer, NOT counting 
// the terminating null character
 int SafePrintf(char* buf, size_t buf_len, const char* fmt, ...);
 int SafePrintV(char* buf, size_t buf_len, const char* fmt, va_list ap);

template <size_t N>
int SafePrintf(char(&buf)[N], const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int result = SafePrintV(buf, N, fmt, ap);
	va_end(ap);
	return result;
}

 int SafePrintf(wchar_t* buf, size_t buf_len, const wchar_t* fmt, ...);
 int SafePrintV(wchar_t* buf, size_t buf_len, const wchar_t* fmt, va_list ap);

template <size_t N>
int SafePrintf(wchar_t(&buf)[N], const wchar_t* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int result = SafePrintV(buf, N, fmt, ap);
	va_end(ap);
	return result;
}

// Print to and return a string object
 std::string StringPrintf(const char* fmt, ...);
 std::string StringPrintV(const char* fmt, va_list ap);
 std::wstring StringPrintf(const wchar_t* fmt, ...);
 std::wstring StringPrintV(const wchar_t* fmt, va_list ap);

// Apend to a string
 void StringAppendF(std::string* result, const char* fmt, ...);
 void StringAppendV(std::string* result, const char* fmt, va_list ap);
 void StringAppendF(std::wstring* result, const wchar_t* fmt, ...);
 void StringAppendV(std::wstring* result, const wchar_t* fmt, va_list ap);
#endif