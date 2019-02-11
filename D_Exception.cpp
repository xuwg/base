#include "StdAfx.h"
#include "D_Exception.h"

D_Exception::D_Exception ( unsigned int uErrorCode )
{
	m_uErrorCode = uErrorCode;
}

D_Exception::~D_Exception()
{
}

unsigned int D_Exception::GetErrorCode ()
{
	return m_uErrorCode;
}
string D_Exception::GetMessage ()
{
	return D_Message::GetMsg ( m_uErrorCode );
}