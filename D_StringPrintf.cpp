#include "StdAfx.h"
#include "D_StringPrintf.h"
#include <vector>

inline int vsnprintT(char* buf, size_t buf_len, const char* fmt, va_list ap) {
	va_list ap_copy;
	ap_copy = ap;
	int result = _vsnprintf_s(buf, buf_len, _TRUNCATE, fmt, ap);
	va_end(ap);
	return result;
}

inline int vsnprintT(wchar_t* buf, size_t buf_len, const wchar_t* fmt, va_list ap) {
	va_list ap_copy;
	ap_copy = ap;
	int result = _vsnwprintf_s(buf, buf_len, _TRUNCATE, fmt, ap);
	va_end(ap);
	return result;
}

template <typename CharType>
inline int SafePrintVT(CharType* buf, size_t buf_len, const CharType* fmt, va_list ap) {
	va_list ap_copy = ap;
	int result = vsnprintT(buf, buf_len, fmt, ap);
	va_end(ap_copy);
	return result;
}

template <class StrType, typename CharType>
inline void StringAppendVT(StrType* str, const CharType* fmt, va_list ap) {
	const int kStackBufSize = 1024;
	CharType stack_buf[kStackBufSize];

	int result = vsnprintT(stack_buf, kStackBufSize, fmt, ap);
	if (result > 0 && result < kStackBufSize) {
		str->append(stack_buf, result);
		return;
	}

	int buf_len = kStackBufSize;
	while (true) {
		buf_len *= 2;
		// Buffer is too big
		if (buf_len > 16*1024*1024)
			return;

		CharType* buf = new CharType[buf_len];
		if (buf == NULL)
			return;

		result = vsnprintT(buf, buf_len, fmt, ap);
		if (result > 0 && result < buf_len) {
			str->append(buf, result);
			delete[] buf;
			return;
		}

		delete[] buf;
	}
}

int SafePrintf(char* buf, size_t buf_len, const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int result = SafePrintV(buf, buf_len, fmt, ap);
	va_end(ap);
	return result;
}

inline int SafePrintV(char* buf, size_t buf_len, const char* fmt, va_list ap) {
	return SafePrintVT(buf, buf_len, fmt, ap);
}

int SafePrintf(wchar_t* buf, size_t buf_len, const wchar_t* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int result = SafePrintV(buf, buf_len, fmt, ap);
	va_end(ap);
	return result;
}

int SafePrintV(wchar_t* buf, size_t buf_len, const wchar_t* fmt, va_list ap) {
	return SafePrintVT(buf, buf_len, fmt, ap);
}

std::string StringPrintf(const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	std::string result;
	StringAppendV(&result, fmt, ap);
	va_end(ap);
	return result;
}

std::string StringPrintV(const char* fmt, va_list ap) {
	std::string result;
	StringAppendV(&result, fmt, ap);
	return result;
}

std::wstring StringPrintf(const wchar_t* fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	std::wstring result;
	StringAppendV(&result, fmt, ap);
	va_end(ap);
	return result;
}

std::wstring StringPrintV(const wchar_t* fmt, va_list ap) {
	std::wstring result;
	StringAppendV(&result, fmt, ap);
	return result;
}

void StringAppendF(std::string* result, const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	StringAppendV(result, fmt, ap);
	va_end(ap);
}

void StringAppendV(std::string* result, const char* fmt, va_list ap) {
	StringAppendVT(result, fmt, ap);
}

void StringAppendF(std::wstring* result, const wchar_t* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	StringAppendV(result, fmt, ap);
	va_end(ap);
}

void StringAppendV(std::wstring* result, const wchar_t* fmt, va_list ap) {
	StringAppendVT(result, fmt, ap);
}
