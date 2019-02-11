#include "ProxyInfo.h"
/** 
*  @date        2017/08/10 15:34 
*  @brief       查询url对应的资源大小 
*  @param[in]   url
*  @return      long       
*  @remarks      
*  @see          
*/ 
long GetDownloadFileLength(const char *url);

/** 
*  @date        2017/08/10 15:30 
*  @brief       根据url下载文件到filePath，支持断点续传 
*  @param[in]   url
*  @param[in]   fullFileName 全路径文件名
*  @param[in]   progressCallback 进度回调函数
*  @param[in]   SuccessCallback 下载成功回调函数
*  @param[in]   failCallback 下载失败回调函数
*  @param[in]   param 进度回调函数参数
*  @param[in]   suspendFlag 暂停下载控制标志
*  @return      bool       
*  @remarks      
*  @see          
*/
typedef void (*ProgressCallback)(void *p, long dltotal, long dlnow, long ultotal, long ulnow);
typedef void (*SuccessCallback)(void *p);
typedef void (*FailCallback)(void *p, const char * errorInfo);
bool DownloadFile(const char *url, const char *fullFileName, ProgressCallback progressCallback, SuccessCallback successCallback, FailCallback failCallback, void *param, bool *suspendFlag, const CProxyInfo &proxyInfo);