#include "typeCast.h"

#include <Windows.h>

std::wstring i2wstring(int val)
{
	WCHAR buf[32] = {0};
	wsprintfW(buf, L"%d", val);
	return std::wstring(buf);
}
