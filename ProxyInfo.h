#ifndef ProxyInfo_h__
#define ProxyInfo_h__

#include <string>

class CProxyInfo
{
public:
	CProxyInfo() {};

	CProxyInfo(bool bUseProxy, std::string proxyAddr, std::string proxyPort)
		: m_UseProxy(bUseProxy), m_ProxyAddr(proxyAddr), m_ProxyPort(proxyPort)
	{

	}

	std::string GetProxyAddr() const { return m_ProxyAddr; }	
	std::string GetProxyPort() const { return m_ProxyPort; }
	bool UseProxy() const { return m_UseProxy; }	

private:
	std::string m_ProxyAddr;
	std::string m_ProxyPort;
	bool m_UseProxy;
};

#endif // ProxyInfo_h__
