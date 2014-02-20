#pragma once
class CFileMap
{
public:
	CFileMap(void);
	virtual ~CFileMap(void);

	BOOL Create(LPCTSTR lpszFileName,DWORD dwProtect);
	void Close();
	LPVOID GetMapAddress();
	DWORD GetFileSize()const;
private:
	HANDLE m_hFileMap;
	LPVOID m_lpAddr;
	DWORD m_dwFileSize;
};

