#include "StdAfx.h"
#include "D_Timestamp.h"
#include <time.h>
#include <ATLComTime.h>

namespace
{
	const int64_t k100nanosecondsPerSecond = 10 * 1000 * 1000;
	const int64_t k100nanosecondsPerMilliSecond = 10 * 1000;
	const int64_t kUnixSecondsSinceFileTimeEpoch = 11644473600;
	const int64_t kUnixMilliSecondsSinceFileTimeEpoch = 11644473600000;

	int64_t FiletimeToint64_t(const FILETIME &ft)
	{
		int64_t v = ft.dwHighDateTime;
		v = (v << 32) + ft.dwLowDateTime;
		return v;
	}

	FILETIME int64_tToFiletime(int64_t v)
	{
		FILETIME ft;
		ft.dwHighDateTime = v >> 32;
		ft.dwLowDateTime = v & 0xFFFFFFFF;
		return ft;
	}
}

D_Timestamp D_Timestamp::defaultValue = D_Timestamp::fromISO8601("1970-01-01T00:00:00Z");

#if defined(_DEBUG) || defined(DEBUG)
void D_Timestamp::updateTimeStr()
{
	SYSTEMTIME st = toUTCSysTime();
	_snprintf_s(str_, 32, _TRUNCATE, "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ",
		st.wYear, st.wMonth, st.wDay, 
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
}
#endif

D_Timestamp::D_Timestamp(void)
{
	update();
}

D_Timestamp::~D_Timestamp(void)
{
}

D_Timestamp::D_Timestamp(int64_t raw) 
{ 
	ts_ = raw; 
	trim();
#if defined(_DEBUG) || defined(DEBUG)
	updateTimeStr();
#endif
}

int64_t D_Timestamp::toRaw() const { 
	return ts_; 
}

D_Timestamp D_Timestamp::fromRaw(int64_t raw) { 
	return D_Timestamp(raw); 
}

D_Timestamp::D_Timestamp(const FILETIME &ft)
{
	ts_ = FiletimeToint64_t(ft);
	trim();
#if defined(_DEBUG) || defined(DEBUG)
	updateTimeStr();
#endif
}

D_Timestamp::D_Timestamp(const SYSTEMTIME &st)
{
	FILETIME ft;
	::SystemTimeToFileTime(&st, &ft);
	ts_ = FiletimeToint64_t(ft);
	trim();
#if defined(_DEBUG) || defined(DEBUG)
	updateTimeStr();
#endif
}

D_Timestamp::D_Timestamp(const D_Timestamp &other)
{
	ts_ = other.ts_;
#if defined(_DEBUG) || defined(DEBUG)
	updateTimeStr();
#endif
}

D_Timestamp& D_Timestamp::operator = (const D_Timestamp &other)
{
	ts_ = other.ts_;
#if defined(_DEBUG) || defined(DEBUG)
	updateTimeStr();
#endif
	return *this;
}

void D_Timestamp::swap(D_Timestamp &other)
{
	std::swap(ts_, other.ts_);
#if defined(_DEBUG) || defined(DEBUG)
	updateTimeStr();
#endif
}

SYSTEMTIME D_Timestamp::toLocalSysTime() const
{
	SYSTEMTIME st;
	FILETIME utc = int64_tToFiletime(ts_);
	FILETIME local;
	::FileTimeToLocalFileTime(&utc, &local);
	::FileTimeToSystemTime(&local, &st);
	return st;
}

SYSTEMTIME D_Timestamp::toUTCSysTime() const
{
	SYSTEMTIME st;
	FILETIME ft = int64_tToFiletime(ts_);
	::FileTimeToSystemTime(&ft, &st);
	return st;
}


std::string D_Timestamp::localTimeString() const
{
	SYSTEMTIME st = toLocalSysTime();
	char buf[128] = {0};
	_snprintf_s(buf, 128, _TRUNCATE, "%04d-%02d-%02d %02d:%02d:%02d",
		st.wYear, st.wMonth, st.wDay, 
		st.wHour, st.wMinute, st.wSecond);

	return buf;
}

	// 根据当前用户配置生成本地时间字符串，用于界面显示
// std::wstring D_Timestamp::localTimeStrByLocale() const
// {
// 	static COleDateTime defaultOleDate((time_t)0);
// 	static CString defaultDateStr = defaultOleDate.Format();
// 
// 	COleDateTime oleDate(this->toLocalSysTime());
// 	if (oleDate.GetStatus() == COleDateTime::valid)
// 	{
// 		CString userDefaultDateStr = oleDate.Format();
// 		return (LPCTSTR)userDefaultDateStr;
// 	}
// 	return (LPCTSTR)defaultDateStr;
// }

std::string D_Timestamp::toUTCStr() const
{
	SYSTEMTIME st = toUTCSysTime();
	char buf[128] = {0};
	_snprintf_s(buf, 128, _TRUNCATE, "%04d-%02d-%02d %02d:%02d:%02d",
		st.wYear, st.wMonth, st.wDay, 
		st.wHour, st.wMinute, st.wSecond);
	return buf;
}

std::string D_Timestamp::toUTCStrMillisecond() const
{
	SYSTEMTIME st = toUTCSysTime();
	char buf[128] = {0};
	_snprintf_s(buf, 128, _TRUNCATE, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
		st.wYear, st.wMonth, st.wDay, 
		st.wHour, st.wMinute, st.wSecond,st.wMilliseconds);
	return buf;
}

std::wstring D_Timestamp::toUTCWStr() const
{
	SYSTEMTIME st = toUTCSysTime();
	wchar_t buf[128] = {0};
	_snwprintf_s(buf, 128, _TRUNCATE, L"%04d-%02d-%02d %02d:%02d:%02d",
		st.wYear, st.wMonth, st.wDay, 
		st.wHour, st.wMinute, st.wSecond);
	return buf;
}
D_Timestamp D_Timestamp::fromUTCStr(const std::string &utc)
{
	int year     = 0;
	int month    = 0;
	int days     = 0;
	int hours    = 0;
	int minutes  = 0;
	int seconds  = 0;

	int count = sscanf(utc.c_str(), "%d-%d-%d %d:%d:%d", &year, &month, &days,
		&hours, &minutes, &seconds);
	if (count == 6)
	{	
		SYSTEMTIME systm;
		systm.wYear = year;
		systm.wMonth = month;
		systm.wDay = days;
		systm.wHour = hours;
		systm.wMinute = minutes;
		systm.wSecond = seconds;
		systm.wMilliseconds = 0;
		systm.wDayOfWeek = 0;
		return D_Timestamp(systm);
	} else {
		return D_Timestamp::defaultValue;
	}
}

void D_Timestamp::trim()
{
	this->ts_ -= (this->ts_ % k100nanosecondsPerMilliSecond);
}

void D_Timestamp::update()
{
	SYSTEMTIME st;
	FILETIME ft;
	::GetSystemTime(&st);
	::SystemTimeToFileTime(&st, &ft);
	ts_ = FiletimeToint64_t(ft);
	trim();
#if defined(_DEBUG) || defined(DEBUG)
	updateTimeStr();
#endif
}

std::string D_Timestamp::toISO8601(bool withMs /* = true */) const
{
	SYSTEMTIME st = toUTCSysTime();
	char buf[128] = {0};
	if (withMs)
	{
		_snprintf_s(buf, 128, _TRUNCATE, "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ",
			st.wYear, st.wMonth, st.wDay, 
			st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	} else {
		_snprintf_s(buf, 128, _TRUNCATE, "%04d-%02d-%02dT%02d:%02d:%02dZ",
			st.wYear, st.wMonth, st.wDay, 
			st.wHour, st.wMinute, st.wSecond);
	}

	return buf;
}

D_Timestamp D_Timestamp::fromISO8601(const std::string &str)
{
	int year     = 0;
	int month    = 0;
	int days     = 0;
	int hours    = 0;
	int minutes  = 0;
	int seconds  = 0;
	int milliseconds = 0;

	bool success = true;
	if (str.size() > 20) {
		int count = sscanf(str.c_str(), "%d-%d-%dT%d:%d:%d.%dT", &year, &month, &days,
			&hours, &minutes, &seconds, &milliseconds);
		success = (count == 7);
	} else {
		int count = sscanf(str.c_str(), "%d-%d-%dT%d:%d:%dT", &year, &month, &days,
			&hours, &minutes, &seconds, &milliseconds);
		success = (count == 6);
	}

	if (success)
	{	
		SYSTEMTIME systm;
		systm.wYear = year;
		systm.wMonth = month;
		systm.wDay = days;
		systm.wHour = hours;
		systm.wMinute = minutes;
		systm.wSecond = seconds;
		systm.wMilliseconds = milliseconds;
		systm.wDayOfWeek = 0;
		return D_Timestamp(systm);
	} else {
		return D_Timestamp::defaultValue;
	}
}

D_Timestamp D_Timestamp::fromUnixTime(const time_t seconds)
{
	int64_t raw = (kUnixSecondsSinceFileTimeEpoch + seconds) * k100nanosecondsPerSecond;
	return D_Timestamp(raw);
}

D_Timestamp D_Timestamp::fromUnixTimeMs(int64_t milliseconds)
{
	time_t seconds = milliseconds / 1000;
	int64_t fraction = milliseconds - (seconds * 1000);

	D_Timestamp ret = fromUnixTime(seconds);
	ret += fraction;
#if defined(_DEBUG) || defined(DEBUG)
	ret.updateTimeStr();
#endif
	return ret;
}

time_t D_Timestamp::toUnixTime() const
{
	return ts_ / k100nanosecondsPerSecond - kUnixSecondsSinceFileTimeEpoch;
}

int64_t D_Timestamp::toUnixTimeMs() const
{
	return ts_ / k100nanosecondsPerMilliSecond - kUnixMilliSecondsSinceFileTimeEpoch;
}

FILETIME D_Timestamp::toFileTime() const
{
	return int64_tToFiletime(ts_);
}

int64_t D_Timestamp::elapsed() const
{
	return now() - *this;
}

bool D_Timestamp::sec_equal(const D_Timestamp &other) const
{
	// 只精确到秒
	return ts_/k100nanosecondsPerSecond == other.ts_/k100nanosecondsPerSecond;
}

bool D_Timestamp::operator == (const D_Timestamp &other) const
{
	return ts_ == other.ts_;
}

bool D_Timestamp::operator != (const D_Timestamp &other) const
{
	return ts_ != other.ts_;
}
bool D_Timestamp::operator >= (const D_Timestamp &other) const
{
	return ts_ >= other.ts_;
}
bool D_Timestamp::operator >  (const D_Timestamp &other) const
{
	return ts_ > other.ts_;
}
bool D_Timestamp::operator <= (const D_Timestamp &other) const
{
	return ts_ <= other.ts_;
}
bool D_Timestamp::operator <  (const D_Timestamp &other) const
{
	return ts_ < other.ts_;
}

int64_t D_Timestamp::operator - (const D_Timestamp &other) const
{
	return (ts_ - other.ts_) / k100nanosecondsPerMilliSecond;
}

D_Timestamp D_Timestamp::operator - (const int64_t d) const
{
	int64_t raw = ts_ - d * k100nanosecondsPerMilliSecond;
	return D_Timestamp(raw);;
}

D_Timestamp D_Timestamp::operator + (const int64_t d) const
{
	int64_t raw = ts_ + d * k100nanosecondsPerMilliSecond;
	return D_Timestamp(raw);;
}

D_Timestamp& D_Timestamp::operator +=(const int64_t d)
{
	ts_ += d * k100nanosecondsPerMilliSecond;
	return *this;
}

D_Timestamp& D_Timestamp::operator -=(const int64_t d)
{
	ts_ -= d * k100nanosecondsPerMilliSecond;
	return *this;
}

int D_Timestamp::resolution() const 
{
	return 1000;
}

D_Timestamp D_Timestamp::now()
{
	return D_Timestamp();
}
