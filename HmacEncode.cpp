// #include "openssl/sha.h"
#include "encodeDecode.h"

// size_t MzHmacEncode(const char *pData, size_t uInSize, char *pHmac)
// {
// 	size_t	uRtn = 0;
// 	unsigned char md[256] = { 0 };


// 	if (!pData || !uInSize)
// 	{
// 		return 0;
// 	}

// 	unsigned char *p = SHA1((unsigned char *)pData, uInSize, md);

// 	uRtn = MzStringToHex(md, 20, (unsigned char *)pHmac, 40);

// 	return uRtn;
// }