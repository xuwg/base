#include "stdafx.h"
#include "Cashier.h"

#include <tchar.h>

const int CCashier::kMappingSize = 512; //共享内存大小
CCriticalSection CCashier::cs_;
CCashier * CCashier::pInstance;

CCashier::CCashier()
	:mode_(COMMON), role_(SYSADMIN)
{	
	hMapping_ = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE | SEC_COMMIT, 0, kMappingSize, _T("Assistant"));
}

CCashier * CCashier::GetInstance()
{
	if(pInstance == NULL)
	{
		cs_.Enter();
		if(pInstance == NULL)
		{
			pInstance = new CCashier();
		}
		cs_.Leave();
	}

	return pInstance;
}

CCashier::~CCashier(void)
{
	if(hMapping_ != NULL)
	{
		CloseHandle(hMapping_);
	}
}

bool CCashier::IsClerk()
{
	return (GetMode() == PERMISSION && GetRole() == CLERK);
}

bool CCashier::IsPermissionCashier()
{
	return (GetMode() == PERMISSION && GetRole() != SYSADMIN);
}

std::string CCashier::GetUserNum()
{
	string userNum;
	HANDLE handle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE | SEC_COMMIT, 0, kMappingSize, _T("Assistant"));
	if(handle != NULL)
	{
		char *pView = (char *) MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, kMappingSize);
		if(pView != NULL)
		{
			userNum = string(pView+2+64+128+64+64);
			UnmapViewOfFile(pView);
		}
		CloseHandle(handle);
	}

	return userNum;
}

void CCashier::SetUserNum(const std::string &val)
{
	userNum_ = val;

	if(hMapping_ != NULL)
	{
		char *pView = (char *) MapViewOfFile(hMapping_, FILE_MAP_ALL_ACCESS, 0, 0, kMappingSize);
		if(pView != NULL)
		{
			strncpy_s(pView+2+64+128+64+64, 64, userNum_.c_str(), userNum_.size());
			UnmapViewOfFile(pView);
		}		
	}
}

CashierMode CCashier::GetMode()
{	
	CashierMode mode = COMMON;

	HANDLE handle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE | SEC_COMMIT, 0, kMappingSize, _T("Assistant"));
	if(handle != NULL)
	{
		char *pView = (char *) MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, kMappingSize);
		if(pView != NULL)
		{
			mode = CashierMode(*pView);
			UnmapViewOfFile(pView);
		}
		CloseHandle(handle);
	}

	return mode;
}

void CCashier::SetMode(CashierMode val)
{
	mode_ = val;
	if(hMapping_ != NULL)
	{
		char *pView = (char *) MapViewOfFile(hMapping_, FILE_MAP_ALL_ACCESS, 0, 0, kMappingSize);
		if(pView != NULL)
		{
			*pView = val;
			UnmapViewOfFile(pView);
		}		
	}
}

CashierRole CCashier::GetRole()
{
	CashierRole role = SYSADMIN;

	HANDLE handle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE | SEC_COMMIT, 0, kMappingSize, _T("Assistant"));
	if(handle != NULL)
	{
		char *pView = (char *) MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, kMappingSize);
		if(pView != NULL)
		{
			role = CashierRole(*(pView+1));
			UnmapViewOfFile(pView);
		}
		CloseHandle(handle);
	}	
	return role;
}

void CCashier::SetRole(CashierRole val)
{
	role_ = val;
	if(hMapping_ != NULL)
	{
		char *pView = (char *) MapViewOfFile(hMapping_, FILE_MAP_ALL_ACCESS, 0, 0, kMappingSize);
		if(pView != NULL)
		{
			*(pView+1) = val;
			UnmapViewOfFile(pView);
		}		
	}
}

std::string CCashier::GetName()
{
	string name;
	HANDLE handle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE | SEC_COMMIT, 0, kMappingSize, _T("Assistant"));
	if(handle != NULL)
	{
		char *pView = (char *) MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, kMappingSize);
		if(pView != NULL)
		{
			name = string(pView+2+64);
			UnmapViewOfFile(pView);
		}
		CloseHandle(handle);
	}

	return name;
}

void CCashier::SetName(const std::string &val)
{
	name_ = val;

	if(hMapping_ != NULL)
	{
		char *pView = (char *) MapViewOfFile(hMapping_, FILE_MAP_ALL_ACCESS, 0, 0, kMappingSize);
		if(pView != NULL)
		{
			strncpy_s(pView+2+64, 128, name_.c_str(), name_.size());
			UnmapViewOfFile(pView);
		}		
	}
}

std::string CCashier::GetLoginId()
{
	string loginId;
	HANDLE handle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE | SEC_COMMIT, 0, kMappingSize, _T("Assistant"));
	if(handle != NULL)
	{
		char *pView = (char *) MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, kMappingSize);
		if(pView != NULL)
		{
			loginId = string(pView+2);
			UnmapViewOfFile(pView);
		}
		CloseHandle(handle);
	}

	return loginId;
}

void CCashier::SetLoginId(const std::string &val)
{
	loginId_ = val;

	if(hMapping_ != NULL)
	{
		char *pView = (char *) MapViewOfFile(hMapping_, FILE_MAP_ALL_ACCESS, 0, 0, kMappingSize);
		if(pView != NULL)
		{
			strncpy_s(pView+2, 64, loginId_.c_str(), loginId_.size());
			UnmapViewOfFile(pView);
		}		
	}
}

std::string CCashier::GetUserMachineNum()
{
	string userMachineNum;
	HANDLE handle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE | SEC_COMMIT, 0, kMappingSize, _T("Assistant"));
	if(handle != NULL)
	{
		char *pView = (char *) MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, kMappingSize);
		if(pView != NULL)
		{
			userMachineNum = string(pView+2+64+128);
			UnmapViewOfFile(pView);
		}
		CloseHandle(handle);
	}

	return userMachineNum;
}

void CCashier::SetUserMachineNum(const std::string &val)
{
	userMachineNum_ = val;

	if(hMapping_ != NULL)
	{
		char *pView = (char *) MapViewOfFile(hMapping_, FILE_MAP_ALL_ACCESS, 0, 0, kMappingSize);
		if(pView != NULL)
		{
			strncpy_s(pView+2+64+128, 64, userMachineNum_.c_str(), userMachineNum_.size());
			UnmapViewOfFile(pView);
		}		
	}
}

std::string CCashier::GetUserSerialNum() const
{
	string userSerialNum;
	HANDLE handle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE | SEC_COMMIT, 0, kMappingSize, _T("Assistant"));
	if(handle != NULL)
	{
		char *pView = (char *) MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, kMappingSize);
		if(pView != NULL)
		{
			userSerialNum = string(pView+2+64+128+64);
			UnmapViewOfFile(pView);
		}
		CloseHandle(handle);
	}

	return userSerialNum;
}

void CCashier::SetUserSerialNum(std::string val)
{
	userSerialNum_ = val;

	if(hMapping_ != NULL)
	{
		char *pView = (char *) MapViewOfFile(hMapping_, FILE_MAP_ALL_ACCESS, 0, 0, kMappingSize);
		if(pView != NULL)
		{
			strncpy_s(pView+2+64+128+64, 64, userSerialNum_.c_str(), userSerialNum_.size());
			UnmapViewOfFile(pView);
		}		
	}
}