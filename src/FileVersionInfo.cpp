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

CString CFileVersionInfo::QueryValue(LPCTSTR lpszValueName) const
{
	CString sValueNmae = lpszValueName;

	if(sValueNmae.IsEmpty() || NULL == m_lpFileVerInfo)
	{
		return _T("");
	}

	struct LANGANDCODEPAGE
	{
		WORD wLanguage;
		WORD wCodePage;
	} *lpTranslate;

	UINT cbTranslate = 0;
	// Read the list of languages and code pages.

	VerQueryValue(m_lpFileVerInfo, TEXT("\\VarFileInfo\\Translation"), (LPVOID*)&lpTranslate, &cbTranslate);

	for(UINT i = 0; i < (cbTranslate / sizeof(struct LANGANDCODEPAGE)); i++)
	{
		CString strBlockName;
		strBlockName.Format(_T("\\StringFileInfo\\%04x%04x\\%s"), lpTranslate[i].wLanguage,
			lpTranslate[i].wCodePage, lpszValueName);

		LPVOID lpResult = NULL;
		UINT uIntResultLen = 0;

		if(::VerQueryValue(m_lpFileVerInfo, strBlockName, &lpResult, &uIntResultLen))
		{
			CString sResult;
			sResult.Format(_T("%s"), lpResult);
			return sResult;
		}
	}

	return _T("");
}

CString CFileVersionInfo::GetFileDescription() const
{
    return QueryValue(_T("FileDescription"));
}

CString CFileVersionInfo::GetFileVersion() const
{
    return QueryValue(_T("FileVersion"));
}

CString CFileVersionInfo::GetProductName() const
{
    return QueryValue(_T("ProductName"));
}

CString CFileVersionInfo::GetProductVersion() const
{
    return QueryValue(_T("ProductVersion"));
}

CString CFileVersionInfo::GetLegalCopyright() const
{
    return QueryValue(_T("LegalCopyright"));
}

CString CFileVersionInfo::GetInternalName() const
{
    return QueryValue(_T("InternalName"));
}

CString CFileVersionInfo::GetCompanyName() const
{
    return QueryValue(_T("CompanyName"));
}

CString CFileVersionInfo::GetOriginalFilename() const
{
    return QueryValue(_T("OriginalFilename"));
}

CString CFileVersionInfo::GetModifyData() const
{
	throw _T("No Impl");
    return _T("");
}

BOOL CFileVersionInfo::GetFixedInfo(VS_FIXEDFILEINFO* vsffi) const
{
    if(m_lpFileVerInfo == NULL)
        return FALSE;

    if(NULL == vsffi)
        return FALSE;

    UINT nQuerySize;

    if(::VerQueryValue((void **)m_lpFileVerInfo, _T("\\"), (LPVOID*)&vsffi, &nQuerySize))
        return TRUE;

    return FALSE;
}
