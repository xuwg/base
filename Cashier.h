#ifndef Cashier_h__
#define Cashier_h__

#include <Windows.h>
#include <string>
#include "ThreadUtil.h"


//当前客户端启动的模式
enum CashierMode
{
    COMMON,              //普通模式
    PERMISSION           //收银员模式
};

//当前登录账户的身份
enum CashierRole
{
    SYSADMIN,  //管理员
    SHOPOWNER, //店长
    CLERK      //店员
};

class CCashier
{
public:    
	virtual ~CCashier(void);

	//收银员模式
    static bool IsClerk();
	//分权限模式并且非管理员用户
	static bool IsPermissionCashier();

	static std::string GetUserNum();
	void SetUserNum(const std::string &val);

	//读取模式
    static CashierMode GetMode();
    void SetMode(CashierMode val); //保存模式到成员变量和共享内存
	
	//读取身份
    static CashierRole GetRole();
	void SetRole(CashierRole val); //保存身份到共享内存

	
	//读取（设置）共享内存中的数据，更新成员变量，并且返回
	static std::string GetName();
	void SetName(const std::string &val);
	static std::string GetLoginId();
	void SetLoginId(const std::string &val);
	static std::string GetUserMachineNum();
	void SetUserMachineNum(const std::string &val);
	std::string GetUserSerialNum() const;
	void SetUserSerialNum(std::string val);

	static CCashier *GetInstance();  //单例控制

private:
	CCashier();
	CCashier(const CCashier&);

private:    
	HANDLE hMapping_; //第一个字节保存模式信息（普通模式，分权限模式），第二个字节保存用户角色信息（管理员，店长，店员） 后续loginId_(64字节) name_(128字节) userMachineNum_(64字节) userSerialNum_(64字节) UserNum_(64字节)
    CashierMode mode_;
    CashierRole role_;
	std::string userNum_;
	std::string loginId_;
	std::string name_;
	std::string userMachineNum_;       //虚拟机具号
	std::string userSerialNum_;        //虚拟序列号
	
	static const int kMappingSize;
	static CCriticalSection cs_;
	static CCashier *pInstance;				
};

#endif // Cashier_h__

