#ifndef Cashier_h__
#define Cashier_h__

#include <Windows.h>
#include <string>
#include "ThreadUtil.h"


//��ǰ�ͻ���������ģʽ
enum CashierMode
{
    COMMON,              //��ͨģʽ
    PERMISSION           //����Աģʽ
};

//��ǰ��¼�˻������
enum CashierRole
{
    SYSADMIN,  //����Ա
    SHOPOWNER, //�곤
    CLERK      //��Ա
};

class CCashier
{
public:    
	virtual ~CCashier(void);

	//����Աģʽ
    static bool IsClerk();
	//��Ȩ��ģʽ���ҷǹ���Ա�û�
	static bool IsPermissionCashier();

	static std::string GetUserNum();
	void SetUserNum(const std::string &val);

	//��ȡģʽ
    static CashierMode GetMode();
    void SetMode(CashierMode val); //����ģʽ����Ա�����͹����ڴ�
	
	//��ȡ���
    static CashierRole GetRole();
	void SetRole(CashierRole val); //������ݵ������ڴ�

	
	//��ȡ�����ã������ڴ��е����ݣ����³�Ա���������ҷ���
	static std::string GetName();
	void SetName(const std::string &val);
	static std::string GetLoginId();
	void SetLoginId(const std::string &val);
	static std::string GetUserMachineNum();
	void SetUserMachineNum(const std::string &val);
	std::string GetUserSerialNum() const;
	void SetUserSerialNum(std::string val);

	static CCashier *GetInstance();  //��������

private:
	CCashier();
	CCashier(const CCashier&);

private:    
	HANDLE hMapping_; //��һ���ֽڱ���ģʽ��Ϣ����ͨģʽ����Ȩ��ģʽ�����ڶ����ֽڱ����û���ɫ��Ϣ������Ա���곤����Ա�� ����loginId_(64�ֽ�) name_(128�ֽ�) userMachineNum_(64�ֽ�) userSerialNum_(64�ֽ�) UserNum_(64�ֽ�)
    CashierMode mode_;
    CashierRole role_;
	std::string userNum_;
	std::string loginId_;
	std::string name_;
	std::string userMachineNum_;       //������ߺ�
	std::string userSerialNum_;        //�������к�
	
	static const int kMappingSize;
	static CCriticalSection cs_;
	static CCashier *pInstance;				
};

#endif // Cashier_h__

