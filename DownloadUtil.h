#include "ProxyInfo.h"
/** 
*  @date        2017/08/10 15:34 
*  @brief       ��ѯurl��Ӧ����Դ��С 
*  @param[in]   url
*  @return      long       
*  @remarks      
*  @see          
*/ 
long GetDownloadFileLength(const char *url);

/** 
*  @date        2017/08/10 15:30 
*  @brief       ����url�����ļ���filePath��֧�ֶϵ����� 
*  @param[in]   url
*  @param[in]   fullFileName ȫ·���ļ���
*  @param[in]   progressCallback ���Ȼص�����
*  @param[in]   SuccessCallback ���سɹ��ص�����
*  @param[in]   failCallback ����ʧ�ܻص�����
*  @param[in]   param ���Ȼص���������
*  @param[in]   suspendFlag ��ͣ���ؿ��Ʊ�־
*  @return      bool       
*  @remarks      
*  @see          
*/
typedef void (*ProgressCallback)(void *p, long dltotal, long dlnow, long ultotal, long ulnow);
typedef void (*SuccessCallback)(void *p);
typedef void (*FailCallback)(void *p, const char * errorInfo);
bool DownloadFile(const char *url, const char *fullFileName, ProgressCallback progressCallback, SuccessCallback successCallback, FailCallback failCallback, void *param, bool *suspendFlag, const CProxyInfo &proxyInfo);