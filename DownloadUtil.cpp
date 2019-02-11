#include "stdafx.h"

#include "DownloadUtil.h"

#include <io.h>
#include <string>

#include <tchar.h>
#include <shlwapi.h>

// #define CURL_STATICLIB
// #include "curl/curl.h"

// using std::string;

// #define MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL     3

// struct MyProgress
// {
// 	double lastruntime;
// 	CURL *curl;
	
// 	ProgressCallback progressCallback;
// 	SuccessCallback successCallback;
// 	FailCallback failCallback;
// 	void *param;

// 	bool *suspendFlag;
// };

// struct MyProgress prog;
 
//  /* this is how the CURLOPT_XFERINFOFUNCTION callback works */
//  static int xferinfo(void *p,
//                      curl_off_t dltotal, curl_off_t dlnow,
//                      curl_off_t ultotal, curl_off_t ulnow)
//  {
//      struct MyProgress *myp = (struct MyProgress *)p;
//      CURL *curl = myp->curl;
//      double curtime = 0;
 
//      curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &curtime);
 
//      /* under certain circumstances it may be desirable for certain functionality
//         to only run every N seconds, in order to do this the transaction time can
//         be used */
//      if ((curtime - myp->lastruntime) >= MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL)
//      {
//          myp->lastruntime = curtime;
//          //fprintf(stderr, "TOTAL TIME: %f \r\n", curtime);
//      }
 
// 	 if(myp->progressCallback != NULL)
// 	 {
// 		 myp->progressCallback(myp->param, dltotal, dlnow, ultotal, ulnow);
// 	 }	 

// //      fprintf(stderr, "UP: %" CURL_FORMAT_CURL_OFF_T " of %" CURL_FORMAT_CURL_OFF_T
// //              "  DOWN: %" CURL_FORMAT_CURL_OFF_T " of %" CURL_FORMAT_CURL_OFF_T
// //              "\r\n",
// //              ulnow, ultotal, dlnow, dltotal);
 
//      if (myp->suspendFlag != NULL && *(myp->suspendFlag))
//      {
//          return 1;//停止下载
//      }
//      return 0;
//  }
 
//  /* for libcurl older than 7.32.0 (CURLOPT_PROGRESSFUNCTION) */
//  static int OlderProgress(void *p,
//                           double dltotal, double dlnow,
//                           double ultotal, double ulnow)
//  {
//      return xferinfo(p,
//                      (curl_off_t)dltotal,
//                      (curl_off_t)dlnow,
//                      (curl_off_t)ultotal,
//                      (curl_off_t)ulnow);
//  }

// size_t WriteFunc(void *ptr, size_t size, size_t nmemb, FILE *stream)
// {
//     return fwrite(ptr, size, nmemb, stream);
// }

// static size_t SaveHeader(void *ptr, size_t size, size_t nmemb, void *data)
// {
//     return (size_t)(size * nmemb);
// }

// long GetDownloadFileLength(const char *url)
// {
//     double len = 0.0;

//     CURL *curl = curl_easy_init();

//     curl_easy_setopt(curl, CURLOPT_URL, url);

//     curl_easy_setopt(curl, CURLOPT_HEADER, 1);    //只要求header头

//     curl_easy_setopt(curl, CURLOPT_NOBODY, 1);    //不需求body

//     curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, SaveHeader);

//     if (curl_easy_perform(curl) == CURLE_OK)
//     {
//         if (CURLE_OK != curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &len))
//         {
//             OutputDebugString(_T("curl_easy_getinfo failed!\n"));
//         }
//     }
//     else
//     {
//         len = -1;
//     }
//     curl_easy_cleanup(curl);

//     return len;
// }

// bool DownloadFile(const char *url, const char *fullFileName, ProgressCallback progressCallback, SuccessCallback successCallback, FailCallback failCallback, void *param, bool *suspendFlag, const CProxyInfo &proxyInfo)
// {	
// 	long fileSize = 0;
	
//     if (PathFileExistsA(fullFileName))
//     {
//         FILE *file = fopen(fullFileName, "r");
//         if (file == NULL)
//         {
//             return false;
//         }
//         fileSize = filelength(fileno(file));
//         fclose(file);
//     }

//     CURL *curl = curl_easy_init();
//     bool result = false;
//     if (curl)
//     {
// 		prog.lastruntime = 0;
// 		prog.curl = curl;
// 		prog.suspendFlag = suspendFlag;
// 		prog.progressCallback = progressCallback;
// 		prog.param = param;

//         curl_easy_setopt(curl, CURLOPT_URL, url);

//         char szBuf[64] = { 0 };
//         wsprintfA(szBuf, "%d-", fileSize);
//         curl_easy_setopt(curl, CURLOPT_RANGE, szBuf);

//         curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFunc);

//         FILE *outfile = fopen(fullFileName, "ab");
//         curl_easy_setopt(curl, CURLOPT_WRITEDATA, outfile);
//         curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, OlderProgress);

        
//         /* pass the struct pointer into the progress function */
//         curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, prog);
// #if LIBCURL_VERSION_NUM >= 0x072000
//         /* xferinfo was introduced in 7.32.0, no earlier libcurl versions will
//            compile as they won't have the symbols around.

//            If built with a newer libcurl, but running with an older libcurl:
//            curl_easy_setopt() will fail in run-time trying to set the new
//            callback, making the older callback get used.

//            New libcurls will prefer the new callback and instead use that one even
//            if both callbacks are set. */

//         curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, xferinfo);
//         /* pass the struct pointer into the xferinfo function, note that this is
//            an alias to CURLOPT_PROGRESSDATA */
//         curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &prog);
// #endif

//         curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
//         curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);



// 		//代理
// 		if(proxyInfo.UseProxy())
// 		{
// 			string proxy = proxyInfo.GetProxyAddr()+":"+proxyInfo.GetProxyPort();
// 			curl_easy_setopt(curl, CURLOPT_PROXY, proxy.c_str());
// 			curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5_HOSTNAME);
// 			curl_easy_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, 1L);
// 		}

//         CURLcode res = curl_easy_perform(curl);

//         fclose(outfile);

//         if (res != CURLE_OK)
//         {
// 			if(failCallback != NULL)
// 			{
// 				failCallback(param, curl_easy_strerror(res));
// 				//fprintf(stderr, "%s\n", curl_easy_strerror(res));
// 			}			
//         }
//         else
// 		{
// 			if(successCallback != NULL)
// 			{
// 				successCallback(param);
// 			}
			
//             result = true;
//         }

//         /* always cleanup */
//         curl_easy_cleanup(curl);
//     }
//     return result;
// }
