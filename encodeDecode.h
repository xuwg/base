#ifndef encodeDecode_h__
#define encodeDecode_h__
#include <iostream>
#include <string>

size_t MzBase64Encode(const char *pData, size_t uInbufSize, char *pOutBuf, size_t uOutBufSize);
size_t MzBase64Decode(const char *pData, size_t uInbufSize, char *pOutBuf, size_t uOutBufSize);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ��������	MzStringToHex
//	����  ��	������������ת��Ϊ16��������
//			 
//  ����  ��	[IN]		pString				����������
//				[IN]		uLength				���������ݴ�С
//				[OUT]		uSize				���ش�С
//  ����ֵ��	ת�����ASCII�ַ���			
//
//	ע�⣺		����ĵ�һ�������������޷����ַ���������f0ת������Ǹ��������������
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MzStringToHex(unsigned char *pSrc, int nSrcLen, unsigned char *pDst, int nDstMaxLen);

//ʮ�������ַ���ת�������ַ���
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