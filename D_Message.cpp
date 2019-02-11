#include "StdAfx.h"
#include "D_Message.h"


D_Message::D_Message(void)
{
}

D_Message::~D_Message(void)
{
}
string D_Message::GetMsg ( int nErrorCode )
{
	
	string strMessage;
	
	switch ( nErrorCode )
	{
	case D_ERROR_POINTERNULL:
		strMessage = "null pointer";
		break;

		strMessage = "succeeded to upload file";
		break;
	case D_UPLOAD_FAILED:
		strMessage = "failed to upload file";
		break;
	case D_ERROR_UPLOAD_RET:
		strMessage = "invalid return code for uploading";
		break;

		break;
	case D_SUCCESS_AUTH:
		strMessage = "succeeded to authenticate user";
		break;
	case D_ERROR_AUTHACCOUNT:
		strMessage = "user does not exist";
		break;
	case D_ERROR_AUTHPASSWORD:
		strMessage = "password is invalid";
		break;
	case D_ERROR_DOWNLOAD:
		strMessage = "failed to downLoad";
		break;
	

		break;
	case D_ERROR_SERVER:
		strMessage = "server error";
		break;
	case D_ERROR_URL:
		strMessage = "invalid url";
		break;

	
	case D_ERROR_NO_INTERNET_CONN:
		strMessage = "no connection to internet";
		break;

	
	



	case D_OPENAPI_BAD_REQUESET:
		strMessage = "Bad http request (400)";
		break;
	case D_OPENAPI_INVALID_TOKEN:
		strMessage = "OAuth authentication error (401)";
		break;
	case D_OPENAPI_INVALID_OAUTH_REQUEST:
		strMessage = "OAuth invalid oauth requeset (403)";
		break;
	case D_OPENAPI_FILE_NOT_FOUND:
		strMessage = "Http Not Found (404)";
		break;
	case D_OPENAPI_METHOD_UNEXPECTED:
		strMessage = "Http Method Unexpected (405)";
		break;
	case D_OPENAPI_FILE_OR_DIR_EXISTS:
		strMessage = "File or Directory Already existsing (409)";
		break;
	case D_OPENAPI_SERVER_FILE_CHANGED:
		strMessage = "Server file changed after last partial download";
		break;
	default:
		strMessage = "Unknown Error";
		break;
	}
	
	char buf[512];
	SafePrintf(buf, "%s: %u", strMessage.c_str(), nErrorCode);
	return buf;
}

wstring D_Message::GetWMsg ( int nErrorCode )
{
	return wide(GetMsg(nErrorCode));
}