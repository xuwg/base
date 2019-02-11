#ifndef encodeDecode_h__
#define encodeDecode_h__
#include <iostream>
#include <string>

size_t MzBase64Encode(const char *pData, size_t uInbufSize, char *pOutBuf, size_t uOutBufSize);
size_t MzBase64Decode(const char *pData, size_t uInbufSize, char *pOutBuf, size_t uOutBufSize);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  函数名：	MzStringToHex
//	功能  ：	将二进制数据转换为16进制数组
//			 
//  参数  ：	[IN]		pString				二进制数据
//				[IN]		uLength				二进制数据大小
//				[OUT]		uSize				返回大小
//  返回值：	转换后的ASCII字符串			
//
//	注意：		传入的第一个参数必须是无符号字符串，否则f0转换后就是个负数，导致溢出
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MzStringToHex(unsigned char *pSrc, int nSrcLen, unsigned char *pDst, int nDstMaxLen);

//十六进制字符串转二进制字符串
std::string Hex2Binary(const std::string &hexString);

size_t MzStringAsc2Wide(const char * pInput, size_t uInSize, wchar_t *pOutBuf, size_t uOutSize);
size_t MzStringUtf8ToWide(const char * pInput, size_t uInSize, wchar_t * pOutBuf, size_t uOutSize);
size_t MzStringWide2Asc(const wchar_t * pWideString, size_t uInSize, char *pOutBuf, size_t uOutSize);
size_t MzStringWide2Utf8(const wchar_t *pWideString, size_t uInSize, char *pOutBuf, size_t uOutSize);
size_t MzStringAsc2Utf8(const char *pAscString, size_t uAscSize, char *pUtf, size_t uUtfSize);
size_t MzStringUtf8ToAsc(const char *pUtf8String, size_t uInSize, char *pOutBuf, size_t uOutSize);

std::string MzStringUtf8ToAscStr(std::string& strUtf8);


std::string UrlEncode(const std::string &str);

#endif // encodeDecode_h__