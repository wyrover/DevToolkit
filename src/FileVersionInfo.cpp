#include "stdafx.h"
#include "FileVersionInfo.h"
#pragma comment(lib,"Version.lib")

CFileVersionInfo::CFileVersionInfo(): m_lpFileVerInfo(NULL)
{

}

CFileVersionInfo::~CFileVersionInfo()
{
    if(NULL != m_lpFileVerInfo)
    {
        delete[] m_lpFileVerInfo;
        m_lpFileVerInfo = NULL;
    }
}

BOOL CFileVersionInfo::QueryVersionInfo(LPCTSTR lpszFileFullPath)
{
    if(!PathFileExists(lpszFileFullPath))
    {
        return FALSE;
    }

    DWORD dwFileVerSize =::GetFileVersionInfoSize(lpszFileFullPath, NULL);

    if(dwFileVerSize <= 0)
    {
        return FALSE;
    }

    m_lpFileVerInfo = new BYTE[dwFileVerSize];
    ZeroMemory(m_lpFileVerInfo, dwFileVerSize);

    if(!::GetFileVersionInfo(lpszFileFullPath, NULL, dwFileVerSize, m_lpFileVerInfo))
    {
        delete[] m_lpFileVerInfo;
        m_lpFileVerInfo = NULL;
        return FALSE;
    }

    return TRUE;
}

LPCTSTR CFileVersionInfo::GetDescription() const
{
    return QueryValue(_T("FileDescription"));
}

LPCTSTR CFileVersionInfo::GetProductName() const
{
    return QueryValue(_T("ProductName"));
}

LPCTSTR CFileVersionInfo::GetProductVersion() const
{
    return _T("");
}

LPCTSTR CFileVersionInfo::GetCopyRight() const
{
    return _T("");
}

LPCTSTR CFileVersionInfo::GetSize() const
{
    return _T("");
}

LPCTSTR CFileVersionInfo::GetModifyData() const
{
    return _T("");
}

LPCTSTR CFileVersionInfo::GetLanguage() const
{
    return _T("");
}

LPCTSTR CFileVersionInfo::GetOrignName() const
{
    return _T("");
}

LPCTSTR CFileVersionInfo::GetFileVersion() const
{
    return QueryValue(_T("FileVersion"));
}

LPCTSTR CFileVersionInfo::QueryValue(LPCTSTR lpszValueName) const
{
    CString sValueNmae = lpszValueName;

    if(sValueNmae.IsEmpty() || NULL == m_lpFileVerInfo)
    {
        return _T("");
    }

    DWORD* pTransTable = NULL;
    UINT dwResultLen = 0;

    if(!VerQueryValue(m_lpFileVerInfo, _T("\\VarFileInfo\\Translation"), (LPVOID*)&pTransTable, &dwResultLen))
    {
        return _T("");
    }

    LONG dwLanguageCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));

    CString strBlockName;
    strBlockName.Format(_T("//StringFileInfo//%08lx//%s"), dwLanguageCharset, lpszValueName);

    LPVOID lpResult = NULL;
    UINT uIntResultLen = 0;

    if(::VerQueryValue(m_lpFileVerInfo, strBlockName, &lpResult, &uIntResultLen))
    {
        CString sResult;
        sResult.Format(_T("%s"), pTransTable);
        return sResult;
    }

    return _T("");
}

