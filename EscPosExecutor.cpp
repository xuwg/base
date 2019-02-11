#include "EscPosExecutor.h"

const double CEscPosExecutor::m_dStep =  0.125;
enum EPosCmd
{
	ESC = 0x1B,
	GS = 0x1D,
	LF = 0x0A
};

CEscPosExecutor::CEscPosExecutor(void)
{
}


CEscPosExecutor::~CEscPosExecutor(void)
{
}

int CEscPosExecutor::SetLeftMargin( double dValue, char *szCmd )
{
	int nCmdSize = 4;
	int nIndex = 0;
	int nValue = dValue / m_dStep;
	int nL = nValue % 256;
	int nH = nValue / 256;	

	szCmd[nIndex++] = GS;
	szCmd[nIndex++] = 0x4C;
	szCmd[nIndex++] = nL;
	szCmd[nIndex++] = nH;  

	return nCmdSize;
}

int CEscPosExecutor::InitPrinter( char *szCmd )
{
	int nCmdSize = 2;
	int nIndex = 0;
	szCmd[nIndex++] = ESC;
	szCmd[nIndex++] = 0x40;  

	return nCmdSize;
}

int CEscPosExecutor::SetRelativePostion( double dValue, char *szCmd )
{
	int nCmdSize = 4;
	int nIndex = 0;
	int nValue = dValue / m_dStep;
	int nL = nValue % 256;
	int nH = nValue / 256;	

	szCmd[nIndex++] = ESC;
	szCmd[nIndex++] = 0x5C;
	szCmd[nIndex++] = nL;
	szCmd[nIndex++] = nH;  

	return nCmdSize;
}

int CEscPosExecutor::SetStandardLineDistance( char *szCmd )
{
	int nCmdSize = 2;
	int nIndex = 0;
	szCmd[nIndex++] = ESC;
	szCmd[nIndex++] = 0x32;  

	return nCmdSize;
}

int CEscPosExecutor::SetCustomLineDistance( double dValue, char *szCmd )
{
	int nCmdSize = 3;
	int nIndex = 0;
	int nValue = dValue / m_dStep;	

	szCmd[nIndex++] = ESC;
	szCmd[nIndex++] = 0x33;
	szCmd[nIndex++] = nValue;

	return nCmdSize;
}

int CEscPosExecutor::SetAlignment( EAligmentStyle eAligmentStyle, char *szCmd )
{
	int nCmdSize = 3;
	int nIndex = 0;
	int nValue = 0;

	szCmd[nIndex++] = ESC;
	szCmd[nIndex++] = 0x61;
	switch(eAligmentStyle)
	{
	case ELeft:
		nValue = 0;
		break;
	case EMid:
		nValue = 1;
		break;
	case ERight:
		nValue = 2;
		break;	
	}
	szCmd[nIndex++] = nValue;

	return nCmdSize;
}

int CEscPosExecutor::ZoomFont( int nMultiWidth, int nMultiHeight, char *szCmd )
{
	int nCmdSize = 3;
	int nIndex = 0;
	int nValue = 0;

	szCmd[nIndex++] = GS;
	szCmd[nIndex++] = 0x21;
	nValue = (nMultiHeight - 1) + ((nMultiWidth-1)<<4);
	szCmd[nIndex++] = nValue;
	return nCmdSize;
}

int CEscPosExecutor::PrintAndNewLine( char *szCmd, bool bLineFeed )
{
	int nCmdSize = bLineFeed ? 2 : 1;
	int nIndex = 0;

	if(bLineFeed)
	{
		szCmd[nIndex++] = 0x0D;
		szCmd[nIndex++] = LF;
	}
	else
	{
		szCmd[nIndex++] = LF;
	}

	return nCmdSize;
}

int CEscPosExecutor::SetAbsoluteposition( double dValue, char *szCmd )
{
	int nCmdSize = 4;
	int nIndex = 0;
	int nValue = dValue / m_dStep;
	int nL = nValue % 256;
	int nH = nValue / 256;	

	szCmd[nIndex++] = ESC;
	szCmd[nIndex++] = 0x24;
	szCmd[nIndex++] = nL;
	szCmd[nIndex++] = nH;  

	return nCmdSize;
}
