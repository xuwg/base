#pragma once
#include "D_DataTypes.h"


class  D_Timestamp
{
public:
	D_Timestamp(void);  // current time
	explicit D_Timestamp(int64_t raw);
	explicit D_Timestamp(const FILETIME &ft);
	explicit D_Timestamp(const SYSTEMTIME &st);
	D_Timestamp(const D_Timestamp &other);

	D_Timestamp& operator = (const D_Timestamp &other);
	void swap(D_Timestamp &other);

	~D_Timestamp(void);

	/// return YYYY-MM-DD hh:mm:ss represention of D_Timestamp
	/// with local timezone settings
	std::string localTimeString() const;

	// 根据当前用户配置生成本地时间字符串，用于界面显示
	//std::wstring localTimeStrByLocale() const;

	/// update D_Timestamp with current time.
	void update();

	/// return time elapsed (in milliseconds) since the time denoted by the D_Timestamp
	int64_t elapsed() const;

	// 转换为SYSTEMTIME
	SYSTEMTIME toLocalSysTime() const;
	SYSTEMTIME toUTCSysTime() const;
	
	// UTC字符串转换 "1970-01-01 00:00:00"
	static D_Timestamp fromUTCStr(const std::string &utc);
	std::string toUTCStr() const;
	std::string D_Timestamp::toUTCStrMillisecond() const;
	std::wstring toUTCWStr() const;

	// ISO8601格式转换, 例: 2013-05-13T02:48:13Z
	std::string toISO8601(bool withMs = true) const;
	static D_Timestamp fromISO8601(const std::string &iso8601);

	// Unix time_t时间转换
	static D_Timestamp fromUnixTime(time_t seconds);
	static D_Timestamp fromUnixTimeMs(int64_t milliseconds);  //毫秒为单位
	time_t toUnixTime() const;
	int64_t toUnixTimeMs() const;

	// 转换为FILETYPE
	FILETIME toFileTime() const;

	// 原始格式，数据库中存储该格式以优化存取性能
	int64_t toRaw() const;
	static D_Timestamp fromRaw(int64_t raw);

	bool sec_equal(const D_Timestamp &other) const;
	bool operator == (const D_Timestamp &other) const;
	bool operator != (const D_Timestamp &other) const;
	bool operator >= (const D_Timestamp &other) const;
	bool operator >  (const D_Timestamp &other) const;
	bool operator <= (const D_Timestamp &other) const;
	bool operator <  (const D_Timestamp &other) const;

	int64_t   operator - (const D_Timestamp &other) const;
	D_Timestamp  operator - (const int64_t d) const;
	D_Timestamp  operator + (const int64_t d) const;

	D_Timestamp& operator +=(const int64_t d);
	D_Timestamp& operator -=(const int64_t d);

	/// units per second, this D_Timestamp has millisecond 
	/// resolution, so it always return 1000
	int resolution() const;  

	static D_Timestamp now();
	static D_Timestamp defaultValue;

private:
	void trim();

	/// internal represention of time, 
	/// which is 100 nanoseconds since windows epoch
	int64_t  ts_;   

#if defined(_DEBUG) || defined(DEBUG)
	// UTC时间，用于调试
	void updateTimeStr();
	char str_[32];  
#endif
};
