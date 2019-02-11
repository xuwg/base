#ifndef  D_DIR_H
#define  D_DIR_H
#pragma once



class  D_Dir
{
public:
	D_Dir(void);
	enum
	{
		D_CONFIG = 0,
		D_DOWNLOAD,
		D_DATA,
		D_TEMP,	
		D_USERCHANNELINFOR,
		D_ALL
	} D_DIR;

public:
	~D_Dir(void);
	const std::wstring& GetConfigIni () const { return m_ConfigIni; }
	
	const std::wstring& GetLogFileName() const { return m_LogFileName; }
	 const std::wstring GetLogDir()  const{ return m_LogPath; }

private:
	std::wstring m_ExeDir;

	std::wstring m_ConfigIni;

	std::wstring m_DlbClientDir;
 
	std::wstring m_LogFileName;
	std::wstring m_LogPath;//ÈÕÖ¾Ä¿Â¼
	
};
#endif