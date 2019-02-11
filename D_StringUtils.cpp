#include "StdAfx.h"
#include "D_StringUtils.h"
#include <windows.h>
using namespace std;

namespace {
	template <typename CharType>
	class StackBuffer
	{
	public:
		StackBuffer(int len)
			:heap_(len > 256 ? new CharType[len] : NULL) { }

		~StackBuffer() { if (heap_ != NULL) delete [] heap_; }

		CharType *data() { return (heap_ == NULL ? stack_ : heap_); }
		const CharType *data() const { return (heap_ == NULL ? stack_ : heap_); }

	private:
		CharType stack_[256];
		CharType *heap_;
	};

	string WCharToMultibyte(UINT codepage, const wchar_t *wstr)
	{
		int len = ::WideCharToMultiByte(codepage, 0, wstr, -1, NULL, 0, NULL, NULL);
		StackBuffer<char> buf(len);
		::WideCharToMultiByte(codepage, 0, wstr, -1, buf.data(), len, NULL, NULL);
		return buf.data();
	}

	wstring MultibyteToWChar(UINT codepage, const char *str)
	{
		int len = ::MultiByteToWideChar(codepage, 0, str, -1, NULL, 0);
		StackBuffer<wchar_t> buf(len);
		::MultiByteToWideChar(codepage, 0, str, -1, buf.data(), len);
		return buf.data();
	}
}
std::string D_Encoding::WCharToUtf8(const wstring &wstr)
{
  return WCharToMultibyte(CP_UTF8, wstr.c_str());
}

std::string D_Encoding::WCharToUtf8(const wchar_t *wstr)
{
  return WCharToMultibyte(CP_UTF8, wstr);
}

string D_Encoding::WCharToAnsi(const wstring &wstr)
{
  return WCharToMultibyte(CP_ACP, wstr.c_str());
}

string D_Encoding::WCharToAnsi(const wchar_t *wstr)
{
  return WCharToMultibyte(CP_ACP, wstr);
}

wstring D_Encoding::Utf8ToWChar(const std::string &utf8str)
{
  return MultibyteToWChar(CP_UTF8, utf8str.c_str());
}

wstring D_Encoding::Utf8ToWChar(const char *utf8str)
{
  return MultibyteToWChar(CP_UTF8, utf8str);
}

wstring D_Encoding::AnsiToWChar(const string &ansi)
{
  return MultibyteToWChar(CP_ACP, ansi.c_str());
}

wstring D_Encoding::AnsiToWChar(const char *ansi)
{
  return MultibyteToWChar(CP_ACP, ansi);
}

 bool StartsWith(const std::wstring &str, const std::wstring &prefix, bool ignoreCase)
{
  if (str.length() < prefix.length())
    return false;

  if (ignoreCase)
  {
    for (size_t i = 0; i < prefix.length(); ++i)
      if (tolower(str[i]) != tolower(prefix[i]))
        return false;
  } else {
    for (size_t i = 0; i < prefix.length(); ++i)
      if (str[i] != prefix[i])
        return false;
  }
  return true;
}

 bool iequals(const std::wstring &ws1, const std::wstring &ws2)
{
	if (ws1.length() != ws2.length())
		return false;

	for (unsigned int i = 0; i < ws1.length(); ++i)
	{
		wchar_t c1 = ws1[i];
		wchar_t c2 = ws2[i];
		if (c1 != c2 && tolower(c1) != tolower(c2))
			return false;
	}
	return true;
}


 std::vector<std::wstring> SplitString(const std::wstring &src, wchar_t sep, bool ignoreEmptyItem)
{
	std::vector<std::wstring> out;
	std::wstring::size_type last = 0, cur = 0;

	while (true)
	{
		cur = src.find_first_of(sep, last);
		if (cur == std::wstring::npos)
		{
			if (last != src.size())
				out.push_back(src.substr(last));
			return out;
		}

		std::wstring item = src.substr(last, cur-last);
		last = cur + 1;

		if (!ignoreEmptyItem || !item.empty())
			out.push_back(item);
	}
}