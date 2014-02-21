#pragma once

class CFileVersionInfo
{
public:
	CFileVersionInfo();
	virtual ~CFileVersionInfo();

	BOOL QueryVersionInfo(LPCTSTR lpszFileFullPath);
	LPCTSTR GetDescription()const;
	LPCTSTR GetFileVersion()const;
	LPCTSTR GetProductName()const;
	LPCTSTR GetProductVersion()const;
	LPCTSTR GetCopyRight()const;
	LPCTSTR GetSize()const;
	LPCTSTR GetModifyData()const;
	LPCTSTR GetLanguage()const;
	LPCTSTR GetOrignName()const;
protected:
	LPCTSTR QueryValue(LPCTSTR lpszValueName)const;
private:
	LPVOID m_lpFileVerInfo;
};