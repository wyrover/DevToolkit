#include "stdafx.h"
#include "FileMap.h"


CFileMap::CFileMap(void): m_hFileMap(NULL), m_lpAddr(NULL),m_dwFileSize(0)
{
}


CFileMap::~CFileMap(void)
{
}

BOOL CFileMap::Create(LPCTSTR lpszFileName, DWORD dwProtect)
{
    if(!PathFileExists(lpszFileName))
    {
        return FALSE;
    }

    if(NULL != m_hFileMap)
    {
        return TRUE;
    }

    HANDLE hFile =::CreateFile(lpszFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if(INVALID_HANDLE_VALUE == hFile)
    {
        return FALSE;
    }

	m_dwFileSize=::GetFileSize(hFile,0);

    m_hFileMap =::CreateFileMapping(hFile, NULL, dwProtect, 0, 0, NULL);

    if(NULL == m_hFileMap)
    {
        CloseHandle(hFile);
        return FALSE;
    }

    m_lpAddr =::MapViewOfFile(m_hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

    if(NULL == m_lpAddr)
    {
        Close();
        CloseHandle(hFile);
        return FALSE;
    }

    CloseHandle(hFile);

    return TRUE;
}

void CFileMap::Close()
{
    if(NULL != m_lpAddr)
    {
        UnmapViewOfFile(m_lpAddr);
        m_lpAddr = NULL;
    }

    if(NULL != m_hFileMap)
    {
        CloseHandle(m_hFileMap);
        m_hFileMap = NULL;
    }

	m_dwFileSize=0;
}

LPVOID CFileMap::GetMapAddress()
{
    return m_lpAddr;
}

DWORD CFileMap::GetFileSize() const
{
	return m_dwFileSize;
}
