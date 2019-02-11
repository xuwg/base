#include <windows.h>
#include "encodeDecode.h"

#include <assert.h>

// #include "openssl/buffer.h"
// #include "openssl/evp.h"
// #include "openssl/bio.h"




// size_t MzBase64Encode(const char *pData, size_t uInbufSize, char *pOutBuf, size_t uOutBufSize)
// {
// 	BIO		*bMemory	= NULL;
// 	BIO		*bEncode	= NULL;
// 	BUF_MEM *bPonter	= NULL;
// 	size_t	uRtn		= 0;

// 	if (!pData || !uInbufSize || !uOutBufSize)
// 	{
// 		return 0;
// 	}

// 	bEncode = BIO_new(BIO_f_base64());
// 	if (!bEncode)
// 	{
// 		return 0;
// 	}
// 	// ������ÿ64���ַ�����һ�� (0x0D 0x0A),�������Ҫ���У���Ҫ��������
// 	BIO_set_flags(bEncode, BIO_FLAGS_BASE64_NO_NL);

// 	bMemory = BIO_new(BIO_s_mem());
// 	bEncode = BIO_push(bEncode, bMemory);
// 	BIO_write(bEncode, pData, uInbufSize);
// 	BIO_flush(bEncode);
// 	BIO_get_mem_ptr(bEncode, &bPonter);

// 	if (bPonter->data && bPonter->length < uOutBufSize)
// 	{
// 		uRtn = bPonter->length;
// 		memcpy(pOutBuf, bPonter->data, bPonter->length);
// 	}

// 	BIO_free_all(bEncode);

// 	return uRtn;
// }

// size_t MzBase64Decode(const char *pData, size_t uInbufSize, char *pOutBuf, size_t uOutBufSize)
// {
// 	BIO 	*bDecode	= NULL;
// 	BIO 	*bMemory	= NULL;
// 	size_t	uSize	= 0;

// 	if (!pData || !uInbufSize || !uOutBufSize)
// 	{
// 		return 0;
// 	}

// 	bDecode = BIO_new(BIO_f_base64());
// 	BIO_set_flags(bDecode, BIO_FLAGS_BASE64_NO_NL);

// 	bMemory = BIO_new_mem_buf((void *)pData, uInbufSize);
// 	bMemory = BIO_push(bDecode, bMemory);
// 	// ��������
// 	uSize = BIO_read(bMemory, pOutBuf, uOutBufSize);

// 	BIO_free_all(bMemory);

// 	return uSize;
// }



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ��������	MzAsc2WideString
//	����  ��	��ASCII���ַ�����ת��ΪWide�ַ���
//
//  ����  ��	[IN]		pInput			ASCII�ַ���
//				[IN][OUT]	dwSize			���صĿ��ַ����ĳ��ȣ����ֽ�Ϊ��λ
//  ����ֵ��	ת����Ŀ��ַ���
//
//	ע�⣺		�ⲿ�����ڲ���Ҫ��������Ҫ����ȥ�ͷ�������ַ����Ļ���
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t MzStringAsc2Wide(const char * pInput, size_t uInSize, wchar_t *pOutBuf, size_t uOutSize)
{
	size_t	dwNeedSize = 0;

	if (!pInput || !uInSize || !pOutBuf || !uOutSize)
	{
		return 0;
	}
	// MultiByteToWideChar�е�ASCII�ַ�����С�ĵ�λ��Byte�����Ƿ���ֵ�Լ����һ��������Wide Char�ĸ���
	//	ע�⣬���һ�������ǿ��ֽڵĸ���
	dwNeedSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pInput, uInSize , pOutBuf, uOutSize / sizeof(WCHAR));

	return dwNeedSize;
}

size_t MzStringUtf8ToWide(const char * pInput, size_t uInSize, wchar_t * pOutBuf, size_t uOutSize)
{
	size_t	dwNeedSize = 0;

	if (!pInput || !uInSize || !pOutBuf || !uOutSize)
	{
		return 0;
	}

	// MultiByteToWideChar�е�ASCII�ַ�����С�ĵ�λ��Byte�����Ƿ���ֵ�Լ����һ��������Wide Char�ĸ���
	//	ע�⣬���һ�������ǿ��ֽڵĸ���
	dwNeedSize = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pInput,uInSize, pOutBuf, uOutSize / sizeof(WCHAR));

	return dwNeedSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ��������	MzStringWide2Asc
//	����  ��	��Wide���ַ�����ת��ΪASCII�ַ���
//
//  ����  ��	[IN]		pWideString			Wide�ַ���
//				[IN][OUT]	dwSize				���ص�ASCII�ַ����ĳ��ȣ����ֽ�Ϊ��λ
//  ����ֵ��	ת�����ASCII�ַ���
//
//	ע�⣺		�ⲿ�����ڲ���Ҫ��������Ҫ����ȥ�ͷ�������ַ����Ļ���
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t MzStringWide2Asc(const wchar_t * pWideString, size_t uInSize, char *pOutBuf, size_t uOutSize)
{
	size_t	dwNeedSize = 0;

	if (!pWideString || !uInSize || !pOutBuf || !uOutSize)
	{
		return 0;
	}
	// WideCharToMultiByte���ַ����Ĵ�С����Wide CharΪ��λ�ģ����ص�ASCII�ַ����Ĵ�С����ByteΪ��λ
	dwNeedSize = WideCharToMultiByte(CP_ACP, 0, pWideString, wcslen(pWideString), pOutBuf, uOutSize, NULL, NULL);

	return dwNeedSize;
}

size_t MzStringWide2Utf8(const wchar_t *pWideString, size_t uInSize, char *pOutBuf, size_t uOutSize)
{
	size_t	dwNeedSize = 0;

	if (!pWideString || !uInSize || !pOutBuf || !uOutSize)
	{
		return 0;
	}
	// WideCharToMultiByte���ַ����Ĵ�С����Wide CharΪ��λ�ģ����ص�ASCII�ַ����Ĵ�С����ByteΪ��λ
	dwNeedSize = WideCharToMultiByte(CP_UTF8, 0, pWideString, uInSize, pOutBuf, uOutSize, NULL, NULL);

	return dwNeedSize;
}

size_t MzStringAsc2Utf8(const char *pAscString,size_t uAscSize,char *pUtf,size_t uUtfSize)
{  
	// convert an MBCS string to widechar   
	size_t	nRtnSize	= 0;
	PWCHAR	pWideString	= NULL;

	if( !pAscString || !pUtf || !uAscSize )
	{
		return 0;
	}

	pWideString = (PWCHAR)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,uAscSize*4);
	if( pWideString )
	{
		nRtnSize = MzStringAsc2Wide(pAscString,uAscSize,pWideString,uAscSize*4);
		if( nRtnSize )
		{
			nRtnSize = MzStringWide2Utf8(pWideString,nRtnSize,pUtf,uUtfSize);
		}
		
		if( NULL != pWideString)
		{
			HeapFree(GetProcessHeap(), 0, pWideString);
		}
	}

	return nRtnSize;  
}  

size_t MzStringUtf8ToAsc(const char *pUtf8String,size_t uInSize,char *pOutBuf,size_t uOutSize)
{
	size_t	nRtnSize	= 0;
	PWCHAR	pWideString	= NULL;
	char*	pAscString = NULL;

	if( !pUtf8String || !uInSize || !pOutBuf || !uOutSize)
	{
		return 0;
	}
	pWideString = (PWCHAR)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,uInSize*4);
	if( pWideString )
	{
		nRtnSize =  MzStringUtf8ToWide(pUtf8String,uInSize,pWideString,uInSize*4);
		if( nRtnSize )
		{
			nRtnSize = MzStringWide2Asc(pWideString,nRtnSize,pOutBuf,uOutSize);
		}

		if( NULL != pWideString)
		{
			HeapFree(GetProcessHeap(), 0, pWideString);
		}
	}
	return nRtnSize;
}

int MzStringToHex(unsigned char *pSrc, int nSrcLen, unsigned char *pDst, int nDstMaxLen)
{
	const char szTable[20] = "0123456789ABCDEF";
	int i = 0;

	if (pDst != NULL)
	{
		*pDst = 0;
	}
	if (pSrc == NULL || nSrcLen <= 0 || pDst == NULL || nDstMaxLen < nSrcLen * 2)
	{
		return 0;
	}
	//0x0-0xf���ַ����ұ�

	for (i = 0; i < nSrcLen; i++)
	{
		//�����4λ
		*pDst++ = szTable[pSrc[i] >> 4];
		// �����4λ
		*pDst++ = szTable[pSrc[i] & 0x0f];
	}

	//����Ŀ���ַ�������
	return  nSrcLen * 2;
}

char hex2binary(char c)
{
	if(c>='0' && c<='9')
	{
		return c-L'0';
	}
	if(c>='A' && c<='Z')
	{
		return c-L'A'+10;
	}
	if(c>='a' && c<='z')
	{
		return c-'a'+10;
	}
	return 0;
}

std::string Hex2Binary(const std::string &hexString)
{
	assert(hexString.size()%2 == 0);

	std::string binaryStr;
	for(int i=0; i<hexString.size(); i+=2)
	{				
		char up = hex2binary(hexString[i]);
		char low = hex2binary(hexString[i+1]);	
		char c = (up*16+low);
		binaryStr += c;			
	}

	return binaryStr;
}

std::string MzStringUtf8ToAscStr(std::string& strUtf8)
{
	std::string strAnsi="";
	char* pszAnsiBuffer = NULL;
	DWORD dwLen = 0;
	do 
	{
		if (strUtf8.size() <= 0)
		{
			break;
		}
		dwLen = strUtf8.size()*4;
		pszAnsiBuffer = new char[dwLen];
		if (pszAnsiBuffer == NULL)
		{
			break;
		}
		ZeroMemory(pszAnsiBuffer, dwLen);
		if(MzStringUtf8ToAsc(const_cast<char *>(strUtf8.c_str()), strUtf8.size(), pszAnsiBuffer, dwLen) <= 0)
		{
			break;
		}
		strAnsi = pszAnsiBuffer;
	} while (false);
	if (pszAnsiBuffer != NULL)
	{
		delete[] pszAnsiBuffer;
		pszAnsiBuffer == NULL;
	}
	return strAnsi;
}


unsigned char ToHex(unsigned char x)
{
	return  x > 9 ? x + 55 : x + 48;
}


std::string UrlEncode(const std::string &str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (isalnum((unsigned char)str[i]) ||
			(str[i] == '-') ||
			(str[i] == '_') ||
			(str[i] == '.') ||
			(str[i] == '+') ||
			(str[i] == '*') ||
			(str[i] == '~'))
			strTemp += str[i];
		else if (str[i] == ' ')
			strTemp += "+";
		else
		{
			strTemp += '%';
			strTemp += ToHex((unsigned char)str[i] >> 4);
			strTemp += ToHex((unsigned char)str[i] % 16);
		}
	}
	return strTemp;
}