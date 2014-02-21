#pragma once

class CFileVersionInfo
{
public:
    CFileVersionInfo();
    virtual ~CFileVersionInfo();

    BOOL QueryVersionInfo(LPCTSTR lpszFileFullPath);
    CString GetFileDescription()const;
    CString GetFileVersion()const;
    CString GetProductName()const;
    CString GetProductVersion()const;
    CString GetInternalName()const;
    CString GetOriginalFilename()const;
    CString GetCompanyName()const;
    CString GetLegalCopyright()const;
    CString GetModifyData()const;
    BOOL GetFixedInfo(VS_FIXEDFILEINFO* vsffi)const;
protected:
    CString QueryValue(LPCTSTR lpszValueName)const;
private:
    LPVOID m_lpFileVerInfo;
};